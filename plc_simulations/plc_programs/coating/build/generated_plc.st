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
      seed:=23
  );
  white_std := uniform_rand.out;


  (* Brown noise step std → small drift increments *)
  uniform_rand(
      mean:=0.05,
      std:=0.02,
      min_value:=0.001,
      max_value:=0.2,
      seed:=25
  );
  brown_std := uniform_rand.out;


  (* AR noise std → moderate correlated disturbance *)
  uniform_rand(
      mean:=0.3,
      std:=0.1,
      min_value:=0.05,
      max_value:=1.0,
      seed:=27
  );
  ar_std := uniform_rand.out;


  (* AR rho → must stay between 0 and 1 for stability *)
  uniform_rand(
      mean:=0.8,
      std:=0.1,
      min_value:=0.5,
      max_value:=0.99,
      seed:=29
  );
  ar_rho := uniform_rand.out;


  (* Periodic noise amplitude *)
  uniform_rand(
      mean:=1.0,
      std:=0.5,
      min_value:=0.1,
      max_value:=2.0,
      seed:=31
  );
  periodic_amp := uniform_rand.out;


  (* Periodic frequency in Hz *)
  uniform_rand(
      mean:=3.0,
      std:=1.0,
      min_value:=0.1,
      max_value:=5.0,
      seed:=33
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
    initialized : BOOL := FALSE;
    param_initializer : ParameterInitializer;
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
    v_max : REAL;
    tau : REAL;
  END_VAR

  v_max := 100.0;
  tau := 1.0;

  out:= v_max*(1.0-EXP(-time_t/tau));

  functionX1 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX1
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 5.0;
    std : REAL := 0.5;
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
    F_set : REAL;
    t_rise : REAL;
  END_VAR

  F_set := 150.0;
  t_rise := 2.0;

  IF time_t < t_rise THEN
    out:= F_set*(time_t/t_rise);
  ELSE
    out:= F_set;
  END_IF;

  functionX2 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX2
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 5.0;
    std : REAL := 0.5;
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
    v_max : REAL;
  END_VAR

  v_max:=8.0;
  out:= v_max;

  functionX3 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX3
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 8.0;
    std : REAL := 0.8;
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
    freq : REAL;
    PI : REAL;
  END_VAR

  F_set := 150.0;
  freq := 0.2; 
  PI := 3.14159265;

  out:= F_set*(1.0+0.05*SIN(2.0*PI*0.2*time_t));

  functionX4 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX4
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 8.0;
    std : REAL := 0.8;
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
    Q_max : REAL;
    tau : REAL;
  END_VAR

  Q_max:= 2.5;
  tau := 0.5;

  out:= Q_max*(1.0-EXP(-time_t/tau));

  functionX5 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX5
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 8.0;
    std : REAL := 0.8;
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
    t_open : REAL;
  END_VAR

  t_open := 0.8;

  IF time_t < t_open THEN
    out:= 100.0*(time_t/t_open);
  ELSE
    out:= 100.0;
  END_IF;


  functionX6 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX6
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 8.0;
    std : REAL := 0.8;
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
    v_max : REAL;
    tau : REAL;
  END_VAR

  v_max := 8.0;
  tau := 1.5;

  out:= v_max*(1.0-EXP(-time_t/tau));

  functionX7 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX7
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
    F_set : REAL;
  END_VAR

  F_set := 140.0;
  out:= F_set;

  functionX8 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX8
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
    theta_max : REAL;
    tau : REAL;
  END_VAR

  theta_max:=120.0;
  tau:=3.0;
  out:= theta_max*(1.0-EXP(-time_t/tau));

  functionX9 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX9
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 12.0;
    std : REAL := 1.2;
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
    t_tramp : REAL;
    u_max : REAL;
  END_VAR

  t_tramp:=2.0;
  u_max:=5.0;

  IF time_t < t_tramp THEN
    out:= u_max*(time_t/t_tramp);
  ELSE
    out:=u_max;
  END_IF;

  functionX10 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX10
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 12.0;
    std : REAL := 1.2;
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
    F_set : REAL;
    F_final : REAL;
    tau : REAL;
  END_VAR

  F_set:=160.0;
  F_final:=140.0;
  tau:=4.0;

  out:= F_set*EXP(-time_t/tau)+F_final*(1.0-EXP(-time_t/tau));

  functionX11 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX11
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 11.0;
    std : REAL := 0.9;
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
    u_max : REAL;
    cycle_T : REAL;
    PI : REAL;
  END_VAR

  u_max:=4.0;
  cycle_T:=10.0;
  PI:= 3.14159265;
  out:=u_max*(1.0-COS(PI*time_t/cycle_T))/2.0;

  functionX12 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX12
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 11.0;
    std : REAL := 0.9;
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
    v_max : REAL;
  END_VAR

  v_max:=5.0;
  out:= v_max;

  functionX13 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX13
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
    F_set : REAL;
    A : REAL;
    f : REAL;
    PI : REAL;
  END_VAR

  F_set:=130.0;
  A:=5.0;
  f:=0.5;
  PI:= 3.14159265;

  out:= F_set+A*SIN(2.0*PI*f*time_t);

  functionX14 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX14
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
  END_VAR

  out:= 1.0;
  functionX15 := out;
END_FUNCTION

FUNCTION_BLOCK ProcessVariationGeneratorX15
  VAR_IN_OUT
    state : BOOL;
  END_VAR
  VAR
    mean : REAL := 4.5;
    std : REAL := 0.5;
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
    inX0 AT %QX0.0 : BOOL := TRUE;
  END_VAR
  VAR
    inX1 AT %QX0.1 : BOOL;
    inX2 AT %QX0.2 : BOOL;
    inX3 AT %QX0.3 : BOOL;
    inX4 AT %QX0.4 : BOOL;
    inX5 AT %QX0.5 : BOOL;
    inX6 AT %QX0.6 : BOOL;
    inX7 AT %QX0.7 : BOOL;
    inX8 AT %QX1.0 : BOOL;
    inX9 AT %QX1.1 : BOOL;
    inX10 AT %QX1.2 : BOOL;
    inX11 AT %QX1.3 : BOOL;
    inX12 AT %QX1.4 : BOOL;
    inX13 AT %QX1.5 : BOOL;
    inX14 AT %QX1.6 : BOOL;
    inX15 AT %QX1.7 : BOOL;
    outX0 AT %QX2.0 : BOOL;
    outX1 AT %QX2.1 : BOOL;
    outX2 AT %QX2.2 : BOOL;
    outX3 AT %QX2.3 : BOOL;
    outX4 AT %QX2.4 : BOOL;
    outX5 AT %QX2.5 : BOOL;
    outX6 AT %QX2.6 : BOOL;
    outX7 AT %QX2.7 : BOOL;
    outX8 AT %QX3.0 : BOOL;
    outX9 AT %QX3.1 : BOOL;
    outX10 AT %QX3.2 : BOOL;
    outX11 AT %QX3.3 : BOOL;
    outX12 AT %QX3.4 : BOOL;
    outX13 AT %QX3.5 : BOOL;
    outX14 AT %QX3.6 : BOOL;
    outX15 AT %QX3.7 : BOOL;
    signalX0 AT %MD0 : REAL;
    signalX1 AT %MD1 : REAL;
    signalX2 AT %MD2 : REAL;
    signalX3 AT %MD3 : REAL;
    signalX4 AT %MD4 : REAL;
    signalX5 AT %MD5 : REAL;
    signalX6 AT %MD6 : REAL;
    signalX7 AT %MD7 : REAL;
    signalX8 AT %MD8 : REAL;
    signalX9 AT %MD9 : REAL;
    signalX10 AT %MD10 : REAL;
    signalX11 AT %MD11 : REAL;
    signalX12 AT %MD12 : REAL;
    signalX13 AT %MD13 : REAL;
    signalX14 AT %MD14 : REAL;
    signalX15 AT %MD15 : REAL;
    T0 AT %QX4.0 : BOOL;
    T1 AT %QX4.1 : BOOL;
    T2 AT %QX4.2 : BOOL;
    T3 AT %QX4.3 : BOOL;
    T4 AT %QX4.4 : BOOL;
    T5 AT %QX4.5 : BOOL;
    T6 AT %QX4.6 : BOOL;
    start AT %QX4.7 : BOOL;
    ConvWeb AT %QX5.0 : BOOL;
    TensWeb AT %QX5.1 : BOOL;
    ConvCoat AT %QX5.2 : BOOL;
    TensCoat AT %QX5.3 : BOOL;
    PumpCoat AT %QX5.4 : BOOL;
    ValveCoat AT %QX5.5 : BOOL;
    ConvLevel AT %QX5.6 : BOOL;
    TensLevel AT %QX5.7 : BOOL;
    TensDryB AT %QX6.0 : BOOL;
    AirB AT %QX6.1 : BOOL;
    Heater AT %QX6.2 : BOOL;
    AirA AT %QX6.3 : BOOL;
    ConvQC AT %QX6.4 : BOOL;
    TensQC AT %QX6.5 : BOOL;
    Reset AT %QX6.6 : BOOL;
    termX0 AT %QX6.7 : BOOL;
    termX1 AT %QX7.0 : BOOL;
    termX2 AT %QX7.1 : BOOL;
    termX3 AT %QX7.2 : BOOL;
    termX4 AT %QX7.3 : BOOL;
    termX5 AT %QX7.4 : BOOL;
    termX6 AT %QX7.5 : BOOL;
    termX7 AT %QX7.6 : BOOL;
    termX8 AT %QX7.7 : BOOL;
    termX9 AT %QX8.0 : BOOL;
    termX10 AT %QX8.1 : BOOL;
    termX11 AT %QX8.2 : BOOL;
    termX12 AT %QX8.3 : BOOL;
    termX13 AT %QX8.4 : BOOL;
    termX14 AT %QX8.5 : BOOL;
    termX15 AT %QX8.6 : BOOL;
  END_VAR
  VAR
    _TMP_AND184_OUT : BOOL;
    _TMP_AND185_OUT : BOOL;
    _TMP_AND186_OUT : BOOL;
    _TMP_AND188_OUT : BOOL;
    _TMP_AND189_OUT : BOOL;
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
  start := termX0;
  _TMP_AND184_OUT := AND(termX1, termX2);
  ConvWeb := _TMP_AND184_OUT;
  TensWeb := _TMP_AND184_OUT;
  _TMP_AND185_OUT := AND(termX3, termX4, termX5, termX6);
  ValveCoat := _TMP_AND185_OUT;
  PumpCoat := _TMP_AND185_OUT;
  TensCoat := _TMP_AND185_OUT;
  ConvCoat := _TMP_AND185_OUT;
  _TMP_AND186_OUT := AND(termX7, termX8);
  TensLevel := _TMP_AND186_OUT;
  ConvLevel := _TMP_AND186_OUT;
  _TMP_AND188_OUT := AND(termX9, termX10, termX11, termX12);
  AirB := _TMP_AND188_OUT;
  TensDryB := _TMP_AND188_OUT;
  AirA := _TMP_AND188_OUT;
  Heater := _TMP_AND188_OUT;
  _TMP_AND189_OUT := AND(termX13, termX14);
  TensQC := _TMP_AND189_OUT;
  ConvQC := _TMP_AND189_OUT;
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
  IF T5 THEN
    inX15 := TRUE; (*set*)
  END_IF;
  IF T4 THEN
    inX9 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX10 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX12 := FALSE; (*reset*)
  END_IF;
  IF T4 THEN
    inX11 := FALSE; (*reset*)
  END_IF;
  termX15 := PVG15.term;
  Reset := termX15;
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
