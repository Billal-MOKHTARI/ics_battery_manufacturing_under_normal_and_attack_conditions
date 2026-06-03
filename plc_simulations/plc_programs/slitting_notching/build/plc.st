TYPE
  LOGLEVEL : (CRITICAL, WARNING, INFO, DEBUG) := INFO;
END_TYPE

FUNCTION_BLOCK LOGGER
  VAR_INPUT
    TRIG : BOOL;
    MSG : STRING;
    LEVEL : LOGLEVEL := INFO;
  END_VAR
  VAR
    TRIG0 : BOOL;
  END_VAR

  IF TRIG AND NOT TRIG0 THEN
  {{
   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  }}
  END_IF;
  TRIG0:=TRIG;
END_FUNCTION_BLOCK


FUNCTION_BLOCK GaussianRand
  VAR_INPUT
    mean : REAL;
    std : REAL;
  END_VAR
  VAR
    u1 : REAL;
    u2 : REAL;
    z : REAL;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    s1 : DINT;
    s2 : DINT;
  END_VAR
  VAR_INPUT
    seed : DINT := 12345;
  END_VAR

  (* Advance seed twice to get two independent uniforms *)
  seed := (seed * 1103515245 + 12345) MOD 2147483647;
  IF seed < 0 THEN seed := -seed; END_IF;
  s1 := seed;

  seed := (seed * 1103515245 + 12345) MOD 2147483647;
  IF seed < 0 THEN seed := -seed; END_IF;
  s2 := seed;

  (* Normalize to (0.0001, 1.0) to avoid LN(0) *)
  u1 := DINT_TO_REAL(s1) / 2147483647.0;
  u2 := DINT_TO_REAL(s2) / 2147483647.0;
  IF u1 < 0.0001 THEN u1 := 0.0001; END_IF;
  IF u2 < 0.0001 THEN u2 := 0.0001; END_IF;

  (* Box-Muller transform *)
  z := SQRT(-2.0 * LN(u1)) * COS(6.2832 * u2);

  out := mean + std * z;
END_FUNCTION_BLOCK

FUNCTION_BLOCK UniformRand
  VAR
    gaussian_rand : GaussianRand;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR_INPUT
    min_value : REAL;
    max_value : REAL;
    mean : REAL;
    std : REAL;
    seed : DINT;
  END_VAR

  gaussian_rand(mean:=mean, std:=std, seed:=seed);
  out := min_value + (max_value-min_value)*gaussian_rand.out;
END_FUNCTION_BLOCK

FUNCTION_BLOCK ParameterInitializer
  VAR
    uniform_rand : UniformRand;
  END_VAR
  VAR_OUTPUT
    white_std : REAL;
    brown_std : REAL;
    ar_std : REAL;
    ar_rho : REAL;
    periodic_amp : REAL;
    periodic_freq : REAL;
  END_VAR

  (* White noise std → small sensor noise *)
  uniform_rand(
      mean:=0.5,
      std:=0.2,
      min_value:=0.1,
      max_value:=2.0,
      seed:=1
  );
  white_std := uniform_rand.out;


  (* Brown noise step std → small drift increments *)
  uniform_rand(
      mean:=0.05,
      std:=0.02,
      min_value:=0.001,
      max_value:=0.2,
      seed:=2
  );
  brown_std := uniform_rand.out;


  (* AR noise std → moderate correlated disturbance *)
  uniform_rand(
      mean:=0.3,
      std:=0.1,
      min_value:=0.05,
      max_value:=1.0,
      seed:=3
  );
  ar_std := uniform_rand.out;


  (* AR rho → must stay between 0 and 1 for stability *)
  uniform_rand(
      mean:=0.8,
      std:=0.1,
      min_value:=0.5,
      max_value:=0.99,
      seed:=4
  );
  ar_rho := uniform_rand.out;


  (* Periodic noise amplitude *)
  uniform_rand(
      mean:=1.0,
      std:=0.5,
      min_value:=0.1,
      max_value:=2.0,
      seed:=5
  );
  periodic_amp := uniform_rand.out;


  (* Periodic frequency in Hz *)
  uniform_rand(
      mean:=3.0,
      std:=1.0,
      min_value:=0.1,
      max_value:=5.0,
      seed:=6
  );
  periodic_freq := uniform_rand.out;
END_FUNCTION_BLOCK

FUNCTION_BLOCK BrownNoiseGenerator
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR_INPUT
    step_std : REAL;
  END_VAR
  VAR
    white_noise : GaussianRand;
    noise_step : REAL;
  END_VAR

  white_noise(mean:=0.0, std:=step_std);
  noise_step := white_noise.out;
  out := out + noise_step;
END_FUNCTION_BLOCK

FUNCTION_BLOCK ARNoiseGenerator
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR_INPUT
    rho : REAL;
    std : REAL;
  END_VAR
  VAR
    white : REAL;
    white_noise : GaussianRand;
  END_VAR

  white_noise(mean:=0.0, std:=std);
  white := white_noise.out;
  out := rho * out + white;
END_FUNCTION_BLOCK

FUNCTION_BLOCK PeriodicNoiseGenerator
  VAR
    PI : REAL;
  END_VAR
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR_INPUT
    freq : REAL;
    amplitude : REAL;
  END_VAR

  PI := 3.14159265;
  out := amplitude * SIN(2.0 * PI * freq * time_t);
END_FUNCTION_BLOCK

FUNCTION_BLOCK SignalGenerator
  VAR
    delta_t : REAL := 0.1;
  END_VAR
  VAR_IN_OUT
    out : REAL;
    state : BOOL;
  END_VAR
  VAR_INPUT
    gauss : REAL;
  END_VAR
  VAR_OUTPUT
    time_t : REAL := 0.0;
    terminated : BOOL := FALSE;
  END_VAR
  VAR
    param_initializer : ParameterInitializer;
    initialized : BOOL := FALSE;
    white_noise : GaussianRand;
    brown_noise : BrownNoiseGenerator;
    ar_noise : ARNoiseGenerator;
    periodic_noise : PeriodicNoiseGenerator;
  END_VAR

  (* Increment t each scan cycle *)


  IF time_t > gauss/delta_t THEN
        out := 0.0;
        terminated := TRUE;
  END_IF; 

  IF terminated THEN
    out:=0.0;
  END_IF;

  IF state THEN
      
      IF NOT initialized THEN
        param_initializer();
        initialized := TRUE;
      END_IF;
      
      white_noise(mean:=0.0, std:=param_initializer.white_std);
      brown_noise(step_std:=param_initializer.brown_std);
      ar_noise(std:=param_initializer.ar_std, rho:=param_initializer.ar_rho);
      periodic_noise(amplitude:=param_initializer.periodic_amp, freq:=param_initializer.periodic_freq, time_t:=time_t);
      
      out := out + white_noise.out+ 
                    brown_noise.out+
                    ar_noise.out;
      time_t := time_t + delta_t;

  ELSE
      out := 0.0;
      time_t := 0.0;
      terminated := FALSE;
  END_IF;
END_FUNCTION_BLOCK

FUNCTION functionX22 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
  END_VAR

  IF time_t < 0.5 THEN
    out:= 1.0;
  ELSE
    out:= 0.0;
  END_IF;

  functionX22 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX22
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 1.5;
    std : REAL := 0.15;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX229_OUT : REAL;
  END_VAR

  _TMP_functionX229_OUT := functionX22(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX229_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX21 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
  END_VAR

  out:= 5.0;
  functionX21 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX21
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 0.5;
    std : REAL := 0.05;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX219_OUT : REAL;
  END_VAR

  _TMP_functionX219_OUT := functionX21(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX219_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX20 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    n_max : REAL;
    t_ramp : REAL;
  END_VAR

  n_max := 60.0;
  t_ramp := 1.0;

  IF time_t < t_ramp THEN
    out:= n_max*(time_t/t_ramp);
  ELSE
    out:= n_max;
  END_IF;

  functionX20 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX20
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX209_OUT : REAL;
  END_VAR

  _TMP_functionX209_OUT := functionX20(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX209_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX19 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
  END_VAR

  out:= 8.0;
  functionX19 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX19
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX199_OUT : REAL;
  END_VAR

  _TMP_functionX199_OUT := functionX19(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX199_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX18 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    f_cam : REAL;
    tau : REAL;
  END_VAR

  f_cam := 30.0;
  tau := 0.2;

  out:= f_cam*(1.0-EXP(-time_t/tau));

  functionX18 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX18
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 3.0;
    std : REAL := 0.3;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX189_OUT : REAL;
  END_VAR

  _TMP_functionX189_OUT := functionX18(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX189_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX17 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
  END_VAR

  out:= 10.0;
  functionX17 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX17
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.5;
    std : REAL := 0.25;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX179_OUT : REAL;
  END_VAR

  _TMP_functionX179_OUT := functionX17(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX179_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX16 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    F_punch : REAL;
    t_rise : REAL;
  END_VAR

  F_punch := 50.0;
  t_rise := 0.8;

  IF time_t < t_rise THEN
    out:= F_punch*(time_t/t_rise);
  ELSE
    out:= F_punch;
  END_IF;

  functionX16 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX16
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 4.0;
    std : REAL := 0.4;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX169_OUT : REAL;
  END_VAR

  _TMP_functionX169_OUT := functionX16(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX169_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX0 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
  END_VAR

  out:= 1.0;

  functionX0 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX0
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 1.0;
    std : REAL := 0.1;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX09_OUT : REAL;
  END_VAR

  _TMP_functionX09_OUT := functionX0(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX09_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX1 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    omega_max : REAL;
    tau : REAL;
  END_VAR

  omega_max := 150.0;
  tau := 2.0;

  out:= omega_max*(1.0-EXP(-time_t/tau));

  functionX1 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX1
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX19_OUT : REAL;
  END_VAR

  _TMP_functionX19_OUT := functionX1(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX19_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX2 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    v_car : REAL;
    t_tramp : REAL;
  END_VAR

  v_car := 5.0;
  t_tramp := 1.5;

  IF time_t < t_tramp THEN
    out:= v_car*(time_t/t_tramp);
  ELSE
    out:= v_car;
  END_IF;

  functionX2 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX2
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.5;
    std : REAL := 0.25;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX29_OUT : REAL;
  END_VAR

  _TMP_functionX29_OUT := functionX2(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX29_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX3 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    A : REAL;
    cycle_T : REAL;
    PI : REAL;
  END_VAR

  A := 10.0;
  cycle_T := 2.0; 
  PI := 3.14159265;

  out:= A*(1.0-COS(PI*time_t/cycle_T))/2.0;

  functionX3 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX3
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 4.0;
    std : REAL := 0.4;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX39_OUT : REAL;
  END_VAR

  _TMP_functionX39_OUT := functionX3(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX39_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX4 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    F_set : REAL;
    PI : REAL;
    tau : REAL;
    F_osc : REAL;
    freq : REAL;
  END_VAR

  F_set := 180.0;
  tau := 1.2;
  PI := 3.14159265;
  freq := 0.3;
  F_osc := 5.0;

  out:= F_set*(1.0-EXP(-time_t/tau))+F_osc*SIN(2.0*PI*freq*time_t);

  functionX4 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX4
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 3.0;
    std : REAL := 0.3;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX49_OUT : REAL;
  END_VAR

  _TMP_functionX49_OUT := functionX4(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX49_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX5 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    g_max : REAL;
    t_close : REAL;
  END_VAR

  g_max := 100.0;
  t_close := 1.0;

  IF time_t < t_close THEN
    out:= g_max*(time_t/t_close);
  ELSE
    out:= g_max;
  END_IF;

  functionX5 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX5
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX59_OUT : REAL;
  END_VAR

  _TMP_functionX59_OUT := functionX5(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX59_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX6 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    tau : REAL;
    g_max : REAL;
  END_VAR

  g_max := 100.0;
  tau := 0.8;

  out:= g_max*(1.0-EXP(-time_t/tau));


  functionX6 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX6
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX69_OUT : REAL;
  END_VAR

  _TMP_functionX69_OUT := functionX6(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX69_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX7 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    v_slit : REAL;
    t_acc : REAL;
  END_VAR

  v_slit := 30.0;
  t_acc := 2.5;

  IF time_t < t_acc THEN
    out:= v_slit*(time_t/t_acc)*(time_t/t_acc);
  ELSE
    out:= v_slit;
  END_IF;

  functionX7 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX7
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 6.0;
    std : REAL := 0.6;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX79_OUT : REAL;
  END_VAR

  _TMP_functionX79_OUT := functionX7(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX79_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX8 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    p_max : REAL;
    tau : REAL;
  END_VAR

  p_max := 500.0;
  tau := 1.0;

  out:= p_max*(1.0-EXP(-time_t/tau));

  functionX8 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX8
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 6.0;
    std : REAL := 0.6;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX89_OUT : REAL;
  END_VAR

  _TMP_functionX89_OUT := functionX8(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX89_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX9 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    omega_wind : REAL;
    PI : REAL;
    freq : REAL;
  END_VAR

  omega_wind:=120.0;
  PI:=3.0;
  freq:=0.2;

  out:=omega_wind*(1.0+0.1*SIN(2.0*PI*freq*time_t));

  functionX9 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX9
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 1.5;
    std : REAL := 0.15;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX99_OUT : REAL;
  END_VAR

  _TMP_functionX99_OUT := functionX9(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX99_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX10 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
  END_VAR

  out:=1.0;

  functionX10 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX10
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 3.0;
    std : REAL := 0.3;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX109_OUT : REAL;
  END_VAR

  _TMP_functionX109_OUT := functionX10(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX109_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX11 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    omega_max : REAL;
    omega_final : REAL;
    tau : REAL;
  END_VAR

  omega_max:=100.0;
  omega_final:=80.0;
  tau:=3.0;

  out:= omega_max*EXP(-time_t/tau)+omega_final*(1.0-EXP(-time_t/tau));

  functionX11 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX11
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX119_OUT : REAL;
  END_VAR

  _TMP_functionX119_OUT := functionX11(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX119_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX12 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    A : REAL;
    cycle_T : REAL;
    PI : REAL;
  END_VAR

  A:=5.0;
  cycle_T:=2.5;
  PI:= 3.14159265;
  out:=A*SIN(PI*time_t/cycle_T)/2.0;

  functionX12 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX12
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 3.0;
    std : REAL := 0.3;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX129_OUT : REAL;
  END_VAR

  _TMP_functionX129_OUT := functionX12(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX129_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX13 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    A : REAL;
    PI : REAL;
    freq : REAL;
    F_base : REAL;
  END_VAR

  A:=10.0;
  PI:= 3.14159265;
  freq:=1.0;
  F_base:=120.0;

  out:=A*SIN(2.0*PI*freq*time_t);

  functionX13 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX13
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.5;
    std : REAL := 0.25;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX139_OUT : REAL;
  END_VAR

  _TMP_functionX139_OUT := functionX13(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX139_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX14 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    L_max : REAL;
    t_fill : REAL;
  END_VAR

  L_max := 2.0;
  t_fill := 2.0;

  IF time_t < t_fill THEN
    out:= L_max*(time_t/t_fill);
  ELSE
    out:= L_max;
  END_IF;

  functionX14 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX14
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 2.0;
    std : REAL := 0.2;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX149_OUT : REAL;
  END_VAR

  _TMP_functionX149_OUT := functionX14(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX149_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION functionX15 : REAL
  VAR_INPUT
    time_t : REAL;
  END_VAR
  VAR
    out : REAL;
    p_max : REAL;
    tau : REAL;
  END_VAR

  p_max := 1.0;
  tau := 0.5;

  out:= p_max*(1.0-EXP(-time_t/tau));

  functionX15 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX15
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 3.0;
    std : REAL := 0.3;
  END_VAR
  VAR_OUTPUT
    out : REAL;
  END_VAR
  VAR
    SignalGenerator0 : SignalGenerator;
    GaussianRand0 : GaussianRand;
  END_VAR
  VAR_OUTPUT
    term : BOOL;
  END_VAR
  VAR
    _TMP_functionX159_OUT : REAL;
  END_VAR

  _TMP_functionX159_OUT := functionX15(SignalGenerator0.time_t);
  GaussianRand0(mean := mean, std := std);
  SignalGenerator0(out := _TMP_functionX159_OUT, state := state, gauss := GaussianRand0.out);
  state := SignalGenerator0.state;
  out := SignalGenerator0.out;
  term := SignalGenerator0.terminated;
END_FUNCTION_BLOCK

FUNCTION_BLOCK BernoulliGenerator
  VAR_INPUT
    in : BOOL;
  END_VAR
  VAR_OUTPUT
    out_1 : BOOL;
    out_2 : BOOL;
  END_VAR
  VAR
    p : REAL;
  END_VAR
  VAR_INPUT
    seed : INT;
  END_VAR
  VAR
    rand_val : REAL;
  END_VAR

  seed := (seed * 37 + 11) MOD 1000;
  rand_val := INT_TO_REAL(seed) / 1000.0;

  IF in THEN
    IF rand_val < p THEN
        out_1 := TRUE;
        out_2 := FALSE;
    ELSE
        out_1 := FALSE;
        out_2 := TRUE;
    END_IF;
  ELSE
    out_1 := FALSE;
    out_2 := FALSE;
  END_IF;
END_FUNCTION_BLOCK

PROGRAM main
  VAR
    PVG0 : ProcessVariationGeneratorX0;
    PVG1 : ProcessVariationGeneratorX1;
    PVG2 : ProcessVariationGeneratorX2;
    PVG3 : ProcessVariationGeneratorX3;
    PVG4 : ProcessVariationGeneratorX4;
    PVG5 : ProcessVariationGeneratorX5;
    PVG6 : ProcessVariationGeneratorX6;
    PVG7 : ProcessVariationGeneratorX7;
    PVG8 : ProcessVariationGeneratorX8;
    PVG9 : ProcessVariationGeneratorX9;
    PVG10 : ProcessVariationGeneratorX10;
    PVG11 : ProcessVariationGeneratorX11;
    PVG12 : ProcessVariationGeneratorX12;
    PVG13 : ProcessVariationGeneratorX13;
    PVG14 : ProcessVariationGeneratorX14;
    PVG15 : ProcessVariationGeneratorX15;
  END_VAR
  VAR
    inX0 : BOOL := TRUE;
    inX1 : BOOL;
    inX2 : BOOL;
    inX3 : BOOL;
    inX4 : BOOL;
    inX5 : BOOL;
    inX6 : BOOL;
    inX7 : BOOL;
    inX8 : BOOL;
    inX9 : BOOL;
    inX10 : BOOL;
    inX11 : BOOL;
    inX12 : BOOL;
    inX13 : BOOL;
    inX14 : BOOL;
    inX15 : BOOL;
    inX16 : BOOL;
    inX17 : BOOL;
    inX18 : BOOL;
    inX19 : BOOL;
    inX20 : BOOL;
    inX21 : BOOL;
    inX22 : BOOL;
    outX0 : BOOL;
    outX1 : BOOL;
    outX2 : BOOL;
    outX3 : BOOL;
    outX4 : BOOL;
    outX5 : BOOL;
    outX6 : BOOL;
    outX7 : BOOL;
    outX8 : BOOL;
    outX9 : BOOL;
    outX10 : BOOL;
    outX11 : BOOL;
    outX12 : BOOL;
    outX13 : BOOL;
    outX14 : BOOL;
    outX15 : BOOL;
    outX16 : BOOL;
    outX17 : BOOL;
    outX18 : BOOL;
    outX19 : BOOL;
    outX20 : BOOL;
    outX21 : BOOL;
    outX22 : BOOL;
    signalX0 : REAL;
    signalX1 : REAL;
    signalX2 : REAL;
    signalX3 : REAL;
    signalX4 : REAL;
    signalX5 : REAL;
    signalX6 : REAL;
    signalX7 : REAL;
    signalX8 : REAL;
    signalX9 : REAL;
    signalX10 : REAL;
    signalX11 : REAL;
    signalX12 : REAL;
    signalX13 : REAL;
    signalX14 : REAL;
    signalX15 : REAL;
    signalX16 : REAL;
    signalX17 : REAL;
    signalX18 : REAL;
    signalX19 : REAL;
    signalX20 : REAL;
    signalX21 : REAL;
    signalX22 : REAL;
    T0 : BOOL;
    T1 : BOOL;
    T2 : BOOL;
    T3 : BOOL;
    T4 : BOOL;
    T5 : BOOL;
    T6 : BOOL;
    T7 : BOOL;
    T8 : BOOL;
    T9 : BOOL;
    T10 : BOOL;
    T11 : BOOL;
    T12 : BOOL;
    T13 : BOOL;
    T14 : BOOL;
    start : BOOL;
    Unwind : BOOL;
    CoilCar : BOOL;
    WebGuide : BOOL;
    TensionSetup : BOOL;
    TopKnife : BOOL;
    BottomKnife : BOOL;
    MainDrive : BOOL;
    PinchRoll : BOOL;
    SlitWind : BOOL;
    CoreShaft : BOOL;
    NotchUnwind : BOOL;
    NotchEdge : BOOL;
    Dancer : BOOL;
    Accum : BOOL;
    Mechanical : BOOL;
    Laser : BOOL;
    OQCConv : BOOL;
    Vision : BOOL;
    ElectrodePass : BOOL;
    Stack : BOOL;
    UnloadConv : BOOL;
    RejGate : BOOL;
    RejConv : BOOL;
    Press : BOOL;
    LaserSensor : BOOL;
    termX0 : BOOL;
    termX1 : BOOL;
    termX2 : BOOL;
    termX3 : BOOL;
    termX4 : BOOL;
    termX5 : BOOL;
    termX6 : BOOL;
    termX7 : BOOL;
    termX8 : BOOL;
    termX9 : BOOL;
    termX10 : BOOL;
    termX11 : BOOL;
    termX12 : BOOL;
    termX13 : BOOL;
    termX14 : BOOL;
    termX15 : BOOL;
    termX16 : BOOL;
    termX17 : BOOL;
    termX18 : BOOL;
    termX19 : BOOL;
    termX20 : BOOL;
    termX21 : BOOL;
    termX22 : BOOL;
  END_VAR
  VAR
    PVG16 : ProcessVariationGeneratorX16;
    PVG17 : ProcessVariationGeneratorX17;
    PVG18 : ProcessVariationGeneratorX18;
    PVG19 : ProcessVariationGeneratorX19;
    PVG20 : ProcessVariationGeneratorX20;
    PVG21 : ProcessVariationGeneratorX21;
    PVG22 : ProcessVariationGeneratorX22;
    BernoulliGenerator0 : BernoulliGenerator;
    BernoulliGenerator1 : BernoulliGenerator;
  END_VAR
  VAR
    seed_0 : INT := 17;
    seed_1 : INT := 20;
  END_VAR
  VAR
    _TMP_AND266_OUT : BOOL;
    _TMP_AND271_OUT : BOOL;
    _TMP_AND274_OUT : BOOL;
    _TMP_AND277_OUT : BOOL;
    _TMP_AND280_OUT : BOOL;
    _TMP_AND287_OUT : BOOL;
    _TMP_AND288_OUT : BOOL;
    _TMP_AND283_OUT : BOOL;
    _TMP_AND286_OUT : BOOL;
  END_VAR

  PVG0(state := inX0);
  outX0 := PVG0.state;
  PVG1(state := inX1);
  outX1 := PVG1.state;
  PVG2(state := inX2);
  outX2 := PVG2.state;
  PVG3(state := inX3);
  outX3 := PVG3.state;
  PVG4(state := inX4);
  outX4 := PVG4.state;
  PVG5(state := inX5);
  outX5 := PVG5.state;
  PVG6(state := inX6);
  outX6 := PVG6.state;
  PVG7(state := inX7);
  outX7 := PVG7.state;
  PVG8(state := inX8);
  outX8 := PVG8.state;
  PVG9(state := inX9);
  outX9 := PVG9.state;
  PVG10(state := inX10);
  outX10 := PVG10.state;
  PVG11(state := inX11);
  outX11 := PVG11.state;
  PVG12(state := inX12);
  outX12 := PVG12.state;
  PVG13(state := inX13);
  outX13 := PVG13.state;
  PVG14(state := inX14);
  outX14 := PVG14.state;
  PVG15(state := inX15);
  outX15 := PVG15.state;
  signalX3 := PVG3.out;
  signalX4 := PVG4.out;
  signalX5 := PVG5.out;
  signalX6 := PVG6.out;
  signalX7 := PVG7.out;
  signalX8 := PVG8.out;
  signalX14 := PVG14.out;
  signalX13 := PVG13.out;
  signalX12 := PVG12.out;
  signalX11 := PVG11.out;
  signalX10 := PVG10.out;
  signalX9 := PVG9.out;
  signalX15 := PVG15.out;
  signalX2 := PVG2.out;
  signalX1 := PVG1.out;
  signalX0 := PVG0.out;
  T0 := start AND inX0;
  T1 := Unwind AND CoilCar AND WebGuide AND inX3 AND inX2 AND inX1;
  T2 := TensionSetup AND inX4;
  T3 := TopKnife AND BottomKnife AND inX6 AND inX5;
  T4 := MainDrive AND PinchRoll AND inX8 AND inX7;
  T5 := SlitWind AND CoreShaft AND inX10 AND inX9;
  T6 := NotchUnwind AND NotchEdge AND inX12 AND inX11;
  termX0 := PVG0.term;
  termX1 := PVG1.term;
  termX2 := PVG2.term;
  termX3 := PVG3.term;
  termX4 := PVG4.term;
  termX5 := PVG5.term;
  termX6 := PVG6.term;
  termX7 := PVG7.term;
  termX8 := PVG8.term;
  termX9 := PVG9.term;
  termX10 := PVG10.term;
  termX11 := PVG11.term;
  termX12 := PVG12.term;
  termX13 := PVG13.term;
  termX14 := PVG14.term;
  termX15 := PVG15.term;
  PVG16(state := inX16);
  outX16 := PVG16.state;
  PVG17(state := inX17);
  outX17 := PVG17.state;
  PVG18(state := inX18);
  outX18 := PVG18.state;
  PVG19(state := inX19);
  outX19 := PVG19.state;
  PVG20(state := inX20);
  outX20 := PVG20.state;
  PVG21(state := inX21);
  outX21 := PVG21.state;
  PVG22(state := inX22);
  outX22 := PVG22.state;
  signalX16 := PVG16.out;
  termX16 := PVG16.term;
  signalX17 := PVG17.out;
  termX17 := PVG17.term;
  signalX18 := PVG18.out;
  termX18 := PVG18.term;
  signalX19 := PVG19.out;
  termX19 := PVG19.term;
  signalX20 := PVG20.out;
  termX20 := PVG20.term;
  signalX21 := PVG21.out;
  termX21 := PVG21.term;
  signalX22 := PVG22.out;
  termX22 := PVG22.term;
  T7 := Dancer AND Accum AND Mechanical AND inX14 AND inX13;
  T8 := Dancer AND Accum AND Laser AND inX14 AND inX13;
  T9 := Press AND inX15;
  T10 := LaserSensor AND inX16;
  T11 := OQCConv AND Vision AND ElectrodePass AND inX18 AND inX17;
  T12 := OQCConv AND Vision AND NOT(ElectrodePass) AND inX18 AND inX17;
  T13 := Stack AND UnloadConv AND inX20 AND inX19;
  T14 := RejGate AND RejConv AND inX22 AND inX21;
  IF T0 THEN
    inX0 := FALSE; (*reset*)
  END_IF;
  IF T0 THEN
    inX1 := TRUE; (*set*)
  END_IF;
  IF T0 THEN
    inX3 := TRUE; (*set*)
  END_IF;
  IF T0 THEN
    inX2 := TRUE; (*set*)
  END_IF;
  IF T1 THEN
    inX3 := FALSE; (*reset*)
  END_IF;
  IF T1 THEN
    inX4 := TRUE; (*set*)
  END_IF;
  IF T1 THEN
    inX2 := FALSE; (*reset*)
  END_IF;
  IF T1 THEN
    inX1 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX5 := TRUE; (*set*)
  END_IF;
  IF T2 THEN
    inX4 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX6 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX6 := FALSE; (*reset*)
  END_IF;
  IF T3 THEN
    inX7 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX5 := FALSE; (*reset*)
  END_IF;
  IF T3 THEN
    inX8 := TRUE; (*set*)
  END_IF;
  IF T4 THEN
    inX8 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX9 := TRUE; (*set*)
  END_IF;
  IF T4 THEN
    inX7 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX10 := TRUE; (*set*)
  END_IF;
  IF T5 THEN
    inX10 := FALSE; (*reset*)
  END_IF;
  IF T5 THEN
    inX11 := TRUE; (*set*)
  END_IF;
  IF T5 THEN
    inX9 := FALSE; (*reset*)
  END_IF;
  IF T5 THEN
    inX12 := TRUE; (*set*)
  END_IF;
  IF T6 THEN
    inX12 := FALSE; (*reset*)
  END_IF;
  IF T6 THEN
    inX13 := TRUE; (*set*)
  END_IF;
  IF T6 THEN
    inX11 := FALSE; (*reset*)
  END_IF;
  IF T6 THEN
    inX14 := TRUE; (*set*)
  END_IF;
  IF T7 THEN
    inX14 := FALSE; (*reset*)
  END_IF;
  IF T7 THEN
    inX13 := FALSE; (*reset*)
  END_IF;
  IF T7 THEN
    inX15 := TRUE; (*set*)
  END_IF;
  IF T8 THEN
    inX14 := FALSE; (*reset*)
  END_IF;
  IF T8 THEN
    inX13 := FALSE; (*reset*)
  END_IF;
  IF T8 THEN
    inX16 := TRUE; (*set*)
  END_IF;
  IF T9 THEN
    inX15 := FALSE; (*reset*)
  END_IF;
  IF T9 THEN
    inX18 := TRUE; (*set*)
  END_IF;
  IF T9 THEN
    inX17 := TRUE; (*set*)
  END_IF;
  IF T10 THEN
    inX16 := FALSE; (*reset*)
  END_IF;
  IF T10 THEN
    inX18 := TRUE; (*set*)
  END_IF;
  IF T10 THEN
    inX17 := TRUE; (*set*)
  END_IF;
  IF T11 THEN
    inX18 := FALSE; (*reset*)
  END_IF;
  IF T11 THEN
    inX19 := TRUE; (*set*)
  END_IF;
  IF T11 THEN
    inX17 := FALSE; (*reset*)
  END_IF;
  IF T11 THEN
    inX20 := TRUE; (*set*)
  END_IF;
  IF T12 THEN
    inX18 := FALSE; (*reset*)
  END_IF;
  IF T12 THEN
    inX21 := TRUE; (*set*)
  END_IF;
  IF T12 THEN
    inX17 := FALSE; (*reset*)
  END_IF;
  IF T12 THEN
    inX22 := TRUE; (*set*)
  END_IF;
  IF T13 THEN
    inX0 := TRUE; (*set*)
  END_IF;
  IF T13 THEN
    inX19 := FALSE; (*reset*)
  END_IF;
  IF T13 THEN
    inX20 := FALSE; (*reset*)
  END_IF;
  IF T14 THEN
    inX0 := TRUE; (*set*)
  END_IF;
  IF T14 THEN
    inX21 := FALSE; (*reset*)
  END_IF;
  IF T14 THEN
    inX22 := FALSE; (*reset*)
  END_IF;
  start := termX0;
  _TMP_AND266_OUT := AND(termX1, termX2, termX3);
  WebGuide := _TMP_AND266_OUT;
  CoilCar := _TMP_AND266_OUT;
  Unwind := _TMP_AND266_OUT;
  TensionSetup := termX4;
  _TMP_AND271_OUT := AND(termX5, termX6);
  BottomKnife := _TMP_AND271_OUT;
  TopKnife := _TMP_AND271_OUT;
  _TMP_AND274_OUT := AND(termX7, termX8);
  MainDrive := _TMP_AND274_OUT;
  PinchRoll := _TMP_AND274_OUT;
  _TMP_AND277_OUT := AND(termX9, termX10);
  SlitWind := _TMP_AND277_OUT;
  CoreShaft := _TMP_AND277_OUT;
  _TMP_AND280_OUT := AND(termX11, termX12);
  NotchEdge := _TMP_AND280_OUT;
  NotchUnwind := _TMP_AND280_OUT;
  Press := termX15;
  LaserSensor := termX16;
  _TMP_AND287_OUT := AND(termX19, termX20);
  UnloadConv := _TMP_AND287_OUT;
  Stack := _TMP_AND287_OUT;
  _TMP_AND288_OUT := AND(termX21, termX22);
  RejConv := _TMP_AND288_OUT;
  RejGate := _TMP_AND288_OUT;
  _TMP_AND283_OUT := AND(termX13, termX14);
  BernoulliGenerator0(in := _TMP_AND283_OUT, seed := seed_0);
  Mechanical := BernoulliGenerator0.out_1;
  Accum := BernoulliGenerator0.out_1 OR BernoulliGenerator0.out_2;
  Laser := BernoulliGenerator0.out_2;
  _TMP_AND286_OUT := AND(termX17, termX18);
  BernoulliGenerator1(in := _TMP_AND286_OUT, seed := seed_1);
  ElectrodePass := BernoulliGenerator1.out_1;
  Vision := BernoulliGenerator1.out_1 OR BernoulliGenerator1.out_2;
  ElectrodePass := NOT(BernoulliGenerator1.out_2);
  Dancer := BernoulliGenerator0.out_1 OR BernoulliGenerator0.out_2;
  OQCConv := BernoulliGenerator1.out_1 OR BernoulliGenerator1.out_2;
END_PROGRAM

PROGRAM main2
  VAR
    inX0 : BOOL := TRUE;
    inX1 : BOOL;
    inX2 : BOOL;
    inX3 : BOOL;
    inX4 : BOOL;
    inX5 : BOOL;
    inX6 : BOOL;
    inX7 : BOOL;
    inX8 : BOOL;
    inX9 : BOOL;
    inX10 : BOOL;
    inX11 : BOOL;
    inX12 : BOOL;
    inX13 : BOOL;
    inX14 : BOOL;
    inX15 : BOOL;
    outX0 : BOOL;
    outX1 : BOOL;
    outX2 : BOOL;
    outX3 : BOOL;
    outX4 : BOOL;
    outX5 : BOOL;
    outX6 : BOOL;
    outX7 : BOOL;
    outX8 : BOOL;
    outX9 : BOOL;
    outX10 : BOOL;
    outX11 : BOOL;
    outX12 : BOOL;
    outX13 : BOOL;
    outX14 : BOOL;
    outX15 : BOOL;
    signalX0 : REAL;
    signalX1 : REAL;
    signalX2 : REAL;
    signalX3 : REAL;
    signalX4 : REAL;
    signalX5 : REAL;
    signalX6 : REAL;
    signalX7 : REAL;
    signalX8 : REAL;
    signalX9 : REAL;
    signalX10 : REAL;
    signalX11 : REAL;
    signalX12 : REAL;
    signalX13 : REAL;
    signalX14 : REAL;
    signalX15 : REAL;
    T0 : BOOL;
    T1 : BOOL;
    T2 : BOOL;
    T3 : BOOL;
    T4 : BOOL;
    T5 : BOOL;
    T6 : BOOL;
    T7 : BOOL;
    start : BOOL;
    ConvWeb : BOOL;
    TensWeb : BOOL;
    ConvCoat : BOOL;
    TensCoat : BOOL;
    PumpCoat : BOOL;
    ValveCoat : BOOL;
    ConvLevel : BOOL;
    TensLevel : BOOL;
    TensDryB : BOOL;
    AirB : BOOL;
    Heater : BOOL;
    AirA : BOOL;
    ConvQC : BOOL;
    TensQC : BOOL;
    Reset : BOOL;
    termX0 : BOOL;
    termX1 : BOOL;
    termX2 : BOOL;
    termX3 : BOOL;
    termX4 : BOOL;
    termX5 : BOOL;
    termX6 : BOOL;
    termX7 : BOOL;
    termX8 : BOOL;
    termX9 : BOOL;
    termX10 : BOOL;
    termX11 : BOOL;
    termX12 : BOOL;
    termX13 : BOOL;
    termX14 : BOOL;
    termX15 : BOOL;
    PVG0 : ProcessVariationGeneratorX0;
    PVG1 : ProcessVariationGeneratorX1;
    PVG2 : ProcessVariationGeneratorX2;
    PVG3 : ProcessVariationGeneratorX3;
    PVG4 : ProcessVariationGeneratorX4;
    PVG5 : ProcessVariationGeneratorX5;
    PVG6 : ProcessVariationGeneratorX6;
    PVG7 : ProcessVariationGeneratorX7;
    PVG8 : ProcessVariationGeneratorX8;
    PVG9 : ProcessVariationGeneratorX9;
    PVG10 : ProcessVariationGeneratorX10;
    PVG11 : ProcessVariationGeneratorX11;
    PVG12 : ProcessVariationGeneratorX12;
    PVG13 : ProcessVariationGeneratorX13;
    PVG14 : ProcessVariationGeneratorX14;
    PVG15 : ProcessVariationGeneratorX15;
  END_VAR

  PVG0(state := inX0);
  outX0 := PVG0.state;
  PVG1(state := inX1);
  outX1 := PVG1.state;
  PVG2(state := inX2);
  outX2 := PVG2.state;
  PVG3(state := inX3);
  outX3 := PVG3.state;
  PVG4(state := inX4);
  outX4 := PVG4.state;
  PVG5(state := inX5);
  outX5 := PVG5.state;
  PVG6(state := inX6);
  outX6 := PVG6.state;
  PVG7(state := inX7);
  outX7 := PVG7.state;
  PVG8(state := inX8);
  outX8 := PVG8.state;
  PVG9(state := inX9);
  outX9 := PVG9.state;
  PVG10(state := inX10);
  outX10 := PVG10.state;
  PVG11(state := inX11);
  outX11 := PVG11.state;
  PVG12(state := inX12);
  outX12 := PVG12.state;
  PVG13(state := inX13);
  outX13 := PVG13.state;
  PVG14(state := inX14);
  outX14 := PVG14.state;
  PVG15(state := inX15);
  outX15 := PVG15.state;
  signalX3 := PVG3.out;
  signalX4 := PVG4.out;
  signalX5 := PVG5.out;
  signalX6 := PVG6.out;
  signalX7 := PVG7.out;
  signalX8 := PVG8.out;
  signalX14 := PVG14.out;
  signalX13 := PVG13.out;
  signalX12 := PVG12.out;
  signalX11 := PVG11.out;
  signalX10 := PVG10.out;
  signalX9 := PVG9.out;
  signalX15 := PVG15.out;
  signalX2 := PVG2.out;
  signalX1 := PVG1.out;
  signalX0 := PVG0.out;
  T0 := start AND inX0;
  T1 := TensWeb AND ConvWeb AND inX2 AND inX1;
  T2 := ValveCoat AND PumpCoat AND TensCoat AND ConvCoat AND inX6 AND inX5 AND inX4 AND inX3;
  T3 := TensLevel AND ConvLevel AND inX8 AND inX7;
  T4 := AirB AND TensDryB AND AirA AND Heater AND inX12 AND inX11 AND inX10 AND inX9;
  T5 := TensQC AND ConvQC AND inX14 AND inX13;
  T6 := Reset AND inX15;
  IF T0 THEN
    inX1 := TRUE; (*set*)
  END_IF;
  IF T0 THEN
    inX2 := TRUE; (*set*)
  END_IF;
  IF T0 THEN
    inX0 := FALSE; (*reset*)
  END_IF;
  IF T1 THEN
    inX3 := TRUE; (*set*)
  END_IF;
  IF T1 THEN
    inX4 := TRUE; (*set*)
  END_IF;
  IF T1 THEN
    inX5 := TRUE; (*set*)
  END_IF;
  IF T1 THEN
    inX6 := TRUE; (*set*)
  END_IF;
  IF T1 THEN
    inX1 := FALSE; (*reset*)
  END_IF;
  IF T1 THEN
    inX2 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX8 := TRUE; (*set*)
  END_IF;
  IF T2 THEN
    inX3 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX4 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX5 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX6 := FALSE; (*reset*)
  END_IF;
  IF T2 THEN
    inX7 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX9 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX10 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX11 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX12 := TRUE; (*set*)
  END_IF;
  IF T3 THEN
    inX7 := FALSE; (*reset*)
  END_IF;
  IF T3 THEN
    inX8 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX13 := TRUE; (*set*)
  END_IF;
  IF T4 THEN
    inX14 := TRUE; (*set*)
  END_IF;
  IF T5 THEN
    inX13 := FALSE; (*reset*)
  END_IF;
  IF T5 THEN
    inX14 := FALSE; (*reset*)
  END_IF;
  IF T6 THEN
    inX0 := TRUE; (*set*)
  END_IF;
  IF T6 THEN
    inX15 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX9 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX10 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX11 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX12 := FALSE; (*reset*)
  END_IF;
  IF T5 THEN
    inX15 := TRUE; (*set*)
  END_IF;
  termX15 := PVG15.term;
  termX7 := PVG7.term;
  termX0 := PVG0.term;
  termX1 := PVG1.term;
  termX2 := PVG2.term;
  termX3 := PVG3.term;
  termX4 := PVG4.term;
  termX5 := PVG5.term;
  termX12 := PVG12.term;
  termX13 := PVG13.term;
  termX11 := PVG11.term;
  termX14 := PVG14.term;
  termX10 := PVG10.term;
  termX9 := PVG9.term;
  termX8 := PVG8.term;
  termX6 := PVG6.term;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : main;
  END_RESOURCE
END_CONFIGURATION
