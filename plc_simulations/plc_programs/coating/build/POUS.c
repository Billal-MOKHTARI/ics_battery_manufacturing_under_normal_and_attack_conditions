void LOGGER_init__(LOGGER *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->TRIG,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MSG,__STRING_LITERAL(0,""),retain)
  __INIT_VAR(data__->LEVEL,LOGLEVEL__INFO,retain)
  __INIT_VAR(data__->TRIG0,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void LOGGER_body__(LOGGER *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  if ((__GET_VAR(data__->TRIG,) && !(__GET_VAR(data__->TRIG0,)))) {
    #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
    #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

   LogMessage(GetFbVar(LEVEL),(char*)GetFbVar(MSG, .body),GetFbVar(MSG, .len));
  
    #undef GetFbVar
    #undef SetFbVar
;
  };
  __SET_VAR(data__->,TRIG0,,__GET_VAR(data__->TRIG,));

  goto __end;

__end:
  return;
} // LOGGER_body__() 





void GAUSSIANRAND_init__(GAUSSIANRAND *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->MEAN,0,retain)
  __INIT_VAR(data__->STD,0,retain)
  __INIT_VAR(data__->U1,0,retain)
  __INIT_VAR(data__->U2,0,retain)
  __INIT_VAR(data__->Z,0,retain)
  __INIT_VAR(data__->OUT,0,retain)
  __INIT_VAR(data__->S1,0,retain)
  __INIT_VAR(data__->S2,0,retain)
  __INIT_VAR(data__->SEED,12345,retain)
}

// Code part
void GAUSSIANRAND_body__(GAUSSIANRAND *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,SEED,,((2147483647 == 0)?0:(((__GET_VAR(data__->SEED,) * 1103515245) + 12345) % 2147483647)));
  if ((__GET_VAR(data__->SEED,) < 0)) {
    __SET_VAR(data__->,SEED,, -(__GET_VAR(data__->SEED,)));
  };
  __SET_VAR(data__->,S1,,__GET_VAR(data__->SEED,));
  __SET_VAR(data__->,SEED,,((2147483647 == 0)?0:(((__GET_VAR(data__->SEED,) * 1103515245) + 12345) % 2147483647)));
  if ((__GET_VAR(data__->SEED,) < 0)) {
    __SET_VAR(data__->,SEED,, -(__GET_VAR(data__->SEED,)));
  };
  __SET_VAR(data__->,S2,,__GET_VAR(data__->SEED,));
  __SET_VAR(data__->,U1,,(DINT_TO_REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (DINT)__GET_VAR(data__->S1,)) / 2147483647.0));
  __SET_VAR(data__->,U2,,(DINT_TO_REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (DINT)__GET_VAR(data__->S2,)) / 2147483647.0));
  if ((__GET_VAR(data__->U1,) < 0.0001)) {
    __SET_VAR(data__->,U1,,0.0001);
  };
  if ((__GET_VAR(data__->U2,) < 0.0001)) {
    __SET_VAR(data__->,U2,,0.0001);
  };
  __SET_VAR(data__->,Z,,(SQRT__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)(-2.0 * LN__REAL__REAL(
      (BOOL)__BOOL_LITERAL(TRUE),
      NULL,
      (REAL)__GET_VAR(data__->U1,)))) * COS__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)(6.2832 * __GET_VAR(data__->U2,)))));
  __SET_VAR(data__->,OUT,,(__GET_VAR(data__->MEAN,) + (__GET_VAR(data__->STD,) * __GET_VAR(data__->Z,))));

  goto __end;

__end:
  return;
} // GAUSSIANRAND_body__() 





void UNIFORMRAND_init__(UNIFORMRAND *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  GAUSSIANRAND_init__(&data__->GAUSSIAN_RAND,retain);
  __INIT_VAR(data__->OUT,0,retain)
  __INIT_VAR(data__->MIN_VALUE,0,retain)
  __INIT_VAR(data__->MAX_VALUE,0,retain)
  __INIT_VAR(data__->MEAN,0,retain)
  __INIT_VAR(data__->STD,0,retain)
  __INIT_VAR(data__->SEED,0,retain)
}

// Code part
void UNIFORMRAND_body__(UNIFORMRAND *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->GAUSSIAN_RAND.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIAN_RAND.,STD,,__GET_VAR(data__->STD,));
  __SET_VAR(data__->GAUSSIAN_RAND.,SEED,,__GET_VAR(data__->SEED,));
  GAUSSIANRAND_body__(&data__->GAUSSIAN_RAND);
  __SET_VAR(data__->,OUT,,(__GET_VAR(data__->MIN_VALUE,) + ((__GET_VAR(data__->MAX_VALUE,) - __GET_VAR(data__->MIN_VALUE,)) * __GET_VAR(data__->GAUSSIAN_RAND.OUT,))));

  goto __end;

__end:
  return;
} // UNIFORMRAND_body__() 





void PARAMETERINITIALIZER_init__(PARAMETERINITIALIZER *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  UNIFORMRAND_init__(&data__->UNIFORM_RAND,retain);
  __INIT_VAR(data__->WHITE_STD,0,retain)
  __INIT_VAR(data__->BROWN_STD,0,retain)
  __INIT_VAR(data__->AR_STD,0,retain)
  __INIT_VAR(data__->AR_RHO,0,retain)
  __INIT_VAR(data__->PERIODIC_AMP,0,retain)
  __INIT_VAR(data__->PERIODIC_FREQ,0,retain)
}

// Code part
void PARAMETERINITIALIZER_body__(PARAMETERINITIALIZER *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->UNIFORM_RAND.,MIN_VALUE,,0.1);
  __SET_VAR(data__->UNIFORM_RAND.,MAX_VALUE,,2.0);
  __SET_VAR(data__->UNIFORM_RAND.,MEAN,,0.5);
  __SET_VAR(data__->UNIFORM_RAND.,STD,,0.2);
  __SET_VAR(data__->UNIFORM_RAND.,SEED,,23);
  UNIFORMRAND_body__(&data__->UNIFORM_RAND);
  __SET_VAR(data__->,WHITE_STD,,__GET_VAR(data__->UNIFORM_RAND.OUT,));
  __SET_VAR(data__->UNIFORM_RAND.,MIN_VALUE,,0.001);
  __SET_VAR(data__->UNIFORM_RAND.,MAX_VALUE,,0.2);
  __SET_VAR(data__->UNIFORM_RAND.,MEAN,,0.05);
  __SET_VAR(data__->UNIFORM_RAND.,STD,,0.02);
  __SET_VAR(data__->UNIFORM_RAND.,SEED,,25);
  UNIFORMRAND_body__(&data__->UNIFORM_RAND);
  __SET_VAR(data__->,BROWN_STD,,__GET_VAR(data__->UNIFORM_RAND.OUT,));
  __SET_VAR(data__->UNIFORM_RAND.,MIN_VALUE,,0.05);
  __SET_VAR(data__->UNIFORM_RAND.,MAX_VALUE,,1.0);
  __SET_VAR(data__->UNIFORM_RAND.,MEAN,,0.3);
  __SET_VAR(data__->UNIFORM_RAND.,STD,,0.1);
  __SET_VAR(data__->UNIFORM_RAND.,SEED,,27);
  UNIFORMRAND_body__(&data__->UNIFORM_RAND);
  __SET_VAR(data__->,AR_STD,,__GET_VAR(data__->UNIFORM_RAND.OUT,));
  __SET_VAR(data__->UNIFORM_RAND.,MIN_VALUE,,0.5);
  __SET_VAR(data__->UNIFORM_RAND.,MAX_VALUE,,0.99);
  __SET_VAR(data__->UNIFORM_RAND.,MEAN,,0.8);
  __SET_VAR(data__->UNIFORM_RAND.,STD,,0.1);
  __SET_VAR(data__->UNIFORM_RAND.,SEED,,29);
  UNIFORMRAND_body__(&data__->UNIFORM_RAND);
  __SET_VAR(data__->,AR_RHO,,__GET_VAR(data__->UNIFORM_RAND.OUT,));
  __SET_VAR(data__->UNIFORM_RAND.,MIN_VALUE,,0.1);
  __SET_VAR(data__->UNIFORM_RAND.,MAX_VALUE,,2.0);
  __SET_VAR(data__->UNIFORM_RAND.,MEAN,,1.0);
  __SET_VAR(data__->UNIFORM_RAND.,STD,,0.5);
  __SET_VAR(data__->UNIFORM_RAND.,SEED,,31);
  UNIFORMRAND_body__(&data__->UNIFORM_RAND);
  __SET_VAR(data__->,PERIODIC_AMP,,__GET_VAR(data__->UNIFORM_RAND.OUT,));
  __SET_VAR(data__->UNIFORM_RAND.,MIN_VALUE,,0.1);
  __SET_VAR(data__->UNIFORM_RAND.,MAX_VALUE,,5.0);
  __SET_VAR(data__->UNIFORM_RAND.,MEAN,,3.0);
  __SET_VAR(data__->UNIFORM_RAND.,STD,,1.0);
  __SET_VAR(data__->UNIFORM_RAND.,SEED,,33);
  UNIFORMRAND_body__(&data__->UNIFORM_RAND);
  __SET_VAR(data__->,PERIODIC_FREQ,,__GET_VAR(data__->UNIFORM_RAND.OUT,));

  goto __end;

__end:
  return;
} // PARAMETERINITIALIZER_body__() 





void BROWNNOISEGENERATOR_init__(BROWNNOISEGENERATOR *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->OUT,0,retain)
  __INIT_VAR(data__->STEP_STD,0,retain)
  GAUSSIANRAND_init__(&data__->WHITE_NOISE,retain);
  __INIT_VAR(data__->NOISE_STEP,0,retain)
}

// Code part
void BROWNNOISEGENERATOR_body__(BROWNNOISEGENERATOR *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->WHITE_NOISE.,MEAN,,0.0);
  __SET_VAR(data__->WHITE_NOISE.,STD,,__GET_VAR(data__->STEP_STD,));
  GAUSSIANRAND_body__(&data__->WHITE_NOISE);
  __SET_VAR(data__->,NOISE_STEP,,__GET_VAR(data__->WHITE_NOISE.OUT,));
  __SET_VAR(data__->,OUT,,(__GET_VAR(data__->OUT,) + __GET_VAR(data__->NOISE_STEP,)));

  goto __end;

__end:
  return;
} // BROWNNOISEGENERATOR_body__() 





void ARNOISEGENERATOR_init__(ARNOISEGENERATOR *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->OUT,0,retain)
  __INIT_VAR(data__->RHO,0,retain)
  __INIT_VAR(data__->STD,0,retain)
  __INIT_VAR(data__->WHITE,0,retain)
  GAUSSIANRAND_init__(&data__->WHITE_NOISE,retain);
}

// Code part
void ARNOISEGENERATOR_body__(ARNOISEGENERATOR *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->WHITE_NOISE.,MEAN,,0.0);
  __SET_VAR(data__->WHITE_NOISE.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->WHITE_NOISE);
  __SET_VAR(data__->,WHITE,,__GET_VAR(data__->WHITE_NOISE.OUT,));
  __SET_VAR(data__->,OUT,,((__GET_VAR(data__->RHO,) * __GET_VAR(data__->OUT,)) + __GET_VAR(data__->WHITE,)));

  goto __end;

__end:
  return;
} // ARNOISEGENERATOR_body__() 





void PERIODICNOISEGENERATOR_init__(PERIODICNOISEGENERATOR *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->PI,0,retain)
  __INIT_VAR(data__->TIME_T,0,retain)
  __INIT_VAR(data__->OUT,0,retain)
  __INIT_VAR(data__->FREQ,0,retain)
  __INIT_VAR(data__->AMPLITUDE,0,retain)
}

// Code part
void PERIODICNOISEGENERATOR_body__(PERIODICNOISEGENERATOR *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,PI,,3.14159265);
  __SET_VAR(data__->,OUT,,(__GET_VAR(data__->AMPLITUDE,) * SIN__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)(((2.0 * __GET_VAR(data__->PI,)) * __GET_VAR(data__->FREQ,)) * __GET_VAR(data__->TIME_T,)))));

  goto __end;

__end:
  return;
} // PERIODICNOISEGENERATOR_body__() 





void SIGNALGENERATOR_init__(SIGNALGENERATOR *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->DELTA_T,0.1,retain)
  __INIT_VAR(data__->OUT,0,retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->GAUSS,0,retain)
  __INIT_VAR(data__->TIME_T,0.0,retain)
  __INIT_VAR(data__->TERMINATED,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INITIALIZED,__BOOL_LITERAL(FALSE),retain)
  PARAMETERINITIALIZER_init__(&data__->PARAM_INITIALIZER,retain);
  GAUSSIANRAND_init__(&data__->WHITE_NOISE,retain);
  BROWNNOISEGENERATOR_init__(&data__->BROWN_NOISE,retain);
  ARNOISEGENERATOR_init__(&data__->AR_NOISE,retain);
  PERIODICNOISEGENERATOR_init__(&data__->PERIODIC_NOISE,retain);
}

// Code part
void SIGNALGENERATOR_body__(SIGNALGENERATOR *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  if ((__GET_VAR(data__->TIME_T,) > (__GET_VAR(data__->GAUSS,) / __GET_VAR(data__->DELTA_T,)))) {
    __SET_VAR(data__->,OUT,,0.0);
    __SET_VAR(data__->,TERMINATED,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->TERMINATED,)) {
    __SET_VAR(data__->,OUT,,0.0);
  };
  if (__GET_VAR(data__->STATE,)) {
    if (!(__GET_VAR(data__->INITIALIZED,))) {
      PARAMETERINITIALIZER_body__(&data__->PARAM_INITIALIZER);
      __SET_VAR(data__->,INITIALIZED,,__BOOL_LITERAL(TRUE));
    };
    __SET_VAR(data__->WHITE_NOISE.,MEAN,,0.0);
    __SET_VAR(data__->WHITE_NOISE.,STD,,__GET_VAR(data__->PARAM_INITIALIZER.WHITE_STD,));
    GAUSSIANRAND_body__(&data__->WHITE_NOISE);
    __SET_VAR(data__->BROWN_NOISE.,STEP_STD,,__GET_VAR(data__->PARAM_INITIALIZER.BROWN_STD,));
    BROWNNOISEGENERATOR_body__(&data__->BROWN_NOISE);
    __SET_VAR(data__->AR_NOISE.,RHO,,__GET_VAR(data__->PARAM_INITIALIZER.AR_RHO,));
    __SET_VAR(data__->AR_NOISE.,STD,,__GET_VAR(data__->PARAM_INITIALIZER.AR_STD,));
    ARNOISEGENERATOR_body__(&data__->AR_NOISE);
    __SET_VAR(data__->PERIODIC_NOISE.,TIME_T,,__GET_VAR(data__->TIME_T,));
    __SET_VAR(data__->PERIODIC_NOISE.,FREQ,,__GET_VAR(data__->PARAM_INITIALIZER.PERIODIC_FREQ,));
    __SET_VAR(data__->PERIODIC_NOISE.,AMPLITUDE,,__GET_VAR(data__->PARAM_INITIALIZER.PERIODIC_AMP,));
    PERIODICNOISEGENERATOR_body__(&data__->PERIODIC_NOISE);
    __SET_VAR(data__->,OUT,,(((__GET_VAR(data__->OUT,) + __GET_VAR(data__->WHITE_NOISE.OUT,)) + __GET_VAR(data__->BROWN_NOISE.OUT,)) + __GET_VAR(data__->AR_NOISE.OUT,)));
    __SET_VAR(data__->,TIME_T,,(__GET_VAR(data__->TIME_T,) + __GET_VAR(data__->DELTA_T,)));
  } else {
    __SET_VAR(data__->,OUT,,0.0);
    __SET_VAR(data__->,TIME_T,,0.0);
    __SET_VAR(data__->,TERMINATED,,__BOOL_LITERAL(FALSE));
  };

  goto __end;

__end:
  return;
} // SIGNALGENERATOR_body__() 





// FUNCTION
REAL FUNCTIONX0(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL FUNCTIONX0 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX0;
  }
  OUT = 1.0;
  FUNCTIONX0 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX0;
}


void PROCESSVARIATIONGENERATORX0_init__(PROCESSVARIATIONGENERATORX0 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,1.0,retain)
  __INIT_VAR(data__->STD,0.1,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX09_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX0_body__(PROCESSVARIATIONGENERATORX0 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX09_OUT,,FUNCTIONX0(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX09_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX09_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX0_body__() 





// FUNCTION
REAL FUNCTIONX1(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL V_MAX = 0;
  REAL TAU = 0;
  REAL FUNCTIONX1 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX1;
  }
  V_MAX = 100.0;
  TAU = 1.0;
  OUT = (V_MAX * (1.0 - EXP__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)( -(TIME_T) / TAU))));
  FUNCTIONX1 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX1;
}


void PROCESSVARIATIONGENERATORX1_init__(PROCESSVARIATIONGENERATORX1 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,5.0,retain)
  __INIT_VAR(data__->STD,0.5,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX19_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX1_body__(PROCESSVARIATIONGENERATORX1 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX19_OUT,,FUNCTIONX1(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX19_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX19_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX1_body__() 





// FUNCTION
REAL FUNCTIONX2(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL F_SET = 0;
  REAL T_RISE = 0;
  REAL FUNCTIONX2 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX2;
  }
  F_SET = 150.0;
  T_RISE = 2.0;
  if ((TIME_T < T_RISE)) {
    OUT = (F_SET * (TIME_T / T_RISE));
  } else {
    OUT = F_SET;
  };
  FUNCTIONX2 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX2;
}


void PROCESSVARIATIONGENERATORX2_init__(PROCESSVARIATIONGENERATORX2 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,5.0,retain)
  __INIT_VAR(data__->STD,0.5,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX29_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX2_body__(PROCESSVARIATIONGENERATORX2 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX29_OUT,,FUNCTIONX2(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX29_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX29_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX2_body__() 





// FUNCTION
REAL FUNCTIONX3(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL V_MAX = 0;
  REAL FUNCTIONX3 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX3;
  }
  V_MAX = 8.0;
  OUT = V_MAX;
  FUNCTIONX3 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX3;
}


void PROCESSVARIATIONGENERATORX3_init__(PROCESSVARIATIONGENERATORX3 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,8.0,retain)
  __INIT_VAR(data__->STD,0.8,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX39_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX3_body__(PROCESSVARIATIONGENERATORX3 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX39_OUT,,FUNCTIONX3(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX39_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX39_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX3_body__() 





// FUNCTION
REAL FUNCTIONX4(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL F_SET = 0;
  REAL FREQ = 0;
  REAL PI = 0;
  REAL FUNCTIONX4 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX4;
  }
  F_SET = 150.0;
  FREQ = 0.2;
  PI = 3.14159265;
  OUT = (F_SET * (1.0 + (0.05 * SIN__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)(((2.0 * PI) * 0.2) * TIME_T)))));
  FUNCTIONX4 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX4;
}


void PROCESSVARIATIONGENERATORX4_init__(PROCESSVARIATIONGENERATORX4 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,8.0,retain)
  __INIT_VAR(data__->STD,0.8,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX49_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX4_body__(PROCESSVARIATIONGENERATORX4 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX49_OUT,,FUNCTIONX4(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX49_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX49_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX4_body__() 





// FUNCTION
REAL FUNCTIONX5(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL Q_MAX = 0;
  REAL TAU = 0;
  REAL FUNCTIONX5 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX5;
  }
  Q_MAX = 2.5;
  TAU = 0.5;
  OUT = (Q_MAX * (1.0 - EXP__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)( -(TIME_T) / TAU))));
  FUNCTIONX5 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX5;
}


void PROCESSVARIATIONGENERATORX5_init__(PROCESSVARIATIONGENERATORX5 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,8.0,retain)
  __INIT_VAR(data__->STD,0.8,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX59_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX5_body__(PROCESSVARIATIONGENERATORX5 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX59_OUT,,FUNCTIONX5(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX59_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX59_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX5_body__() 





// FUNCTION
REAL FUNCTIONX6(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL T_OPEN = 0;
  REAL FUNCTIONX6 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX6;
  }
  T_OPEN = 0.8;
  if ((TIME_T < T_OPEN)) {
    OUT = (100.0 * (TIME_T / T_OPEN));
  } else {
    OUT = 100.0;
  };
  FUNCTIONX6 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX6;
}


void PROCESSVARIATIONGENERATORX6_init__(PROCESSVARIATIONGENERATORX6 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,8.0,retain)
  __INIT_VAR(data__->STD,0.8,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX69_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX6_body__(PROCESSVARIATIONGENERATORX6 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX69_OUT,,FUNCTIONX6(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX69_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX69_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX6_body__() 





// FUNCTION
REAL FUNCTIONX7(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL V_MAX = 0;
  REAL TAU = 0;
  REAL FUNCTIONX7 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX7;
  }
  V_MAX = 8.0;
  TAU = 1.5;
  OUT = (V_MAX * (1.0 - EXP__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)( -(TIME_T) / TAU))));
  FUNCTIONX7 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX7;
}


void PROCESSVARIATIONGENERATORX7_init__(PROCESSVARIATIONGENERATORX7 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,3.0,retain)
  __INIT_VAR(data__->STD,0.3,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX79_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX7_body__(PROCESSVARIATIONGENERATORX7 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX79_OUT,,FUNCTIONX7(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX79_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX79_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX7_body__() 





// FUNCTION
REAL FUNCTIONX8(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL F_SET = 0;
  REAL FUNCTIONX8 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX8;
  }
  F_SET = 140.0;
  OUT = F_SET;
  FUNCTIONX8 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX8;
}


void PROCESSVARIATIONGENERATORX8_init__(PROCESSVARIATIONGENERATORX8 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,3.0,retain)
  __INIT_VAR(data__->STD,0.3,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX89_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX8_body__(PROCESSVARIATIONGENERATORX8 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX89_OUT,,FUNCTIONX8(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX89_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX89_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX8_body__() 





// FUNCTION
REAL FUNCTIONX9(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL THETA_MAX = 0;
  REAL TAU = 0;
  REAL FUNCTIONX9 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX9;
  }
  THETA_MAX = 120.0;
  TAU = 3.0;
  OUT = (THETA_MAX * (1.0 - EXP__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)( -(TIME_T) / TAU))));
  FUNCTIONX9 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX9;
}


void PROCESSVARIATIONGENERATORX9_init__(PROCESSVARIATIONGENERATORX9 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,12.0,retain)
  __INIT_VAR(data__->STD,1.2,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX99_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX9_body__(PROCESSVARIATIONGENERATORX9 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX99_OUT,,FUNCTIONX9(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX99_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX99_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX9_body__() 





// FUNCTION
REAL FUNCTIONX10(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL T_TRAMP = 0;
  REAL U_MAX = 0;
  REAL FUNCTIONX10 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX10;
  }
  T_TRAMP = 2.0;
  U_MAX = 5.0;
  if ((TIME_T < T_TRAMP)) {
    OUT = (U_MAX * (TIME_T / T_TRAMP));
  } else {
    OUT = U_MAX;
  };
  FUNCTIONX10 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX10;
}


void PROCESSVARIATIONGENERATORX10_init__(PROCESSVARIATIONGENERATORX10 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,12.0,retain)
  __INIT_VAR(data__->STD,1.2,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX109_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX10_body__(PROCESSVARIATIONGENERATORX10 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX109_OUT,,FUNCTIONX10(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX109_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX109_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX10_body__() 





// FUNCTION
REAL FUNCTIONX11(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL F_SET = 0;
  REAL F_FINAL = 0;
  REAL TAU = 0;
  REAL FUNCTIONX11 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX11;
  }
  F_SET = 160.0;
  F_FINAL = 140.0;
  TAU = 4.0;
  OUT = ((F_SET * EXP__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)( -(TIME_T) / TAU))) + (F_FINAL * (1.0 - EXP__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)( -(TIME_T) / TAU)))));
  FUNCTIONX11 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX11;
}


void PROCESSVARIATIONGENERATORX11_init__(PROCESSVARIATIONGENERATORX11 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,11.0,retain)
  __INIT_VAR(data__->STD,0.9,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX119_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX11_body__(PROCESSVARIATIONGENERATORX11 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX119_OUT,,FUNCTIONX11(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX119_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX119_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX11_body__() 





// FUNCTION
REAL FUNCTIONX12(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL U_MAX = 0;
  REAL CYCLE_T = 0;
  REAL PI = 0;
  REAL FUNCTIONX12 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX12;
  }
  U_MAX = 4.0;
  CYCLE_T = 10.0;
  PI = 3.14159265;
  OUT = ((U_MAX * (1.0 - COS__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)((PI * TIME_T) / CYCLE_T)))) / 2.0);
  FUNCTIONX12 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX12;
}


void PROCESSVARIATIONGENERATORX12_init__(PROCESSVARIATIONGENERATORX12 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,11.0,retain)
  __INIT_VAR(data__->STD,0.9,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX129_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX12_body__(PROCESSVARIATIONGENERATORX12 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX129_OUT,,FUNCTIONX12(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX129_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX129_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX12_body__() 





// FUNCTION
REAL FUNCTIONX13(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL V_MAX = 0;
  REAL FUNCTIONX13 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX13;
  }
  V_MAX = 5.0;
  OUT = V_MAX;
  FUNCTIONX13 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX13;
}


void PROCESSVARIATIONGENERATORX13_init__(PROCESSVARIATIONGENERATORX13 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,4.0,retain)
  __INIT_VAR(data__->STD,0.4,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX139_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX13_body__(PROCESSVARIATIONGENERATORX13 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX139_OUT,,FUNCTIONX13(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX139_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX139_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX13_body__() 





// FUNCTION
REAL FUNCTIONX14(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL F_SET = 0;
  REAL A = 0;
  REAL F = 0;
  REAL PI = 0;
  REAL FUNCTIONX14 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX14;
  }
  F_SET = 130.0;
  A = 5.0;
  F = 0.5;
  PI = 3.14159265;
  OUT = (F_SET + (A * SIN__REAL__REAL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)(((2.0 * PI) * F) * TIME_T))));
  FUNCTIONX14 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX14;
}


void PROCESSVARIATIONGENERATORX14_init__(PROCESSVARIATIONGENERATORX14 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,4.0,retain)
  __INIT_VAR(data__->STD,0.4,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX149_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX14_body__(PROCESSVARIATIONGENERATORX14 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX149_OUT,,FUNCTIONX14(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX149_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX149_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX14_body__() 





// FUNCTION
REAL FUNCTIONX15(
  BOOL EN, 
  BOOL *__ENO, 
  REAL TIME_T)
{
  BOOL ENO = __BOOL_LITERAL(TRUE);
  REAL OUT = 0;
  REAL FUNCTIONX15 = 0;

  // Control execution
  if (!EN) {
    if (__ENO != NULL) {
      *__ENO = __BOOL_LITERAL(FALSE);
    }
    return FUNCTIONX15;
  }
  OUT = 1.0;
  FUNCTIONX15 = OUT;

  goto __end;

__end:
  if (__ENO != NULL) {
    *__ENO = ENO;
  }
  return FUNCTIONX15;
}


void PROCESSVARIATIONGENERATORX15_init__(PROCESSVARIATIONGENERATORX15 *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->STATE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MEAN,4.5,retain)
  __INIT_VAR(data__->STD,0.5,retain)
  __INIT_VAR(data__->OUT,0,retain)
  SIGNALGENERATOR_init__(&data__->SIGNALGENERATOR0,retain);
  GAUSSIANRAND_init__(&data__->GAUSSIANRAND0,retain);
  __INIT_VAR(data__->TERM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_FUNCTIONX159_OUT,0,retain)
}

// Code part
void PROCESSVARIATIONGENERATORX15_body__(PROCESSVARIATIONGENERATORX15 *data__) {
  // Control execution
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;
  }
  else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }
  // Initialise TEMP variables

  __SET_VAR(data__->,_TMP_FUNCTIONX159_OUT,,FUNCTIONX15(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (REAL)__GET_VAR(data__->SIGNALGENERATOR0.TIME_T,)));
  __SET_VAR(data__->GAUSSIANRAND0.,MEAN,,__GET_VAR(data__->MEAN,));
  __SET_VAR(data__->GAUSSIANRAND0.,STD,,__GET_VAR(data__->STD,));
  GAUSSIANRAND_body__(&data__->GAUSSIANRAND0);
  __SET_VAR(data__->SIGNALGENERATOR0.,OUT,,__GET_VAR(data__->_TMP_FUNCTIONX159_OUT,));
  __SET_VAR(data__->SIGNALGENERATOR0.,STATE,,__GET_VAR(data__->STATE,));
  __SET_VAR(data__->SIGNALGENERATOR0.,GAUSS,,__GET_VAR(data__->GAUSSIANRAND0.OUT,));
  SIGNALGENERATOR_body__(&data__->SIGNALGENERATOR0);
  __SET_VAR(data__->,_TMP_FUNCTIONX159_OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE));
  __SET_VAR(data__->,STATE,,__GET_VAR(data__->SIGNALGENERATOR0.STATE,));
  __SET_VAR(data__->,OUT,,__GET_VAR(data__->SIGNALGENERATOR0.OUT,));
  __SET_VAR(data__->,TERM,,__GET_VAR(data__->SIGNALGENERATOR0.TERMINATED,));

  goto __end;

__end:
  return;
} // PROCESSVARIATIONGENERATORX15_body__() 





void MAIN_init__(MAIN *data__, BOOL retain) {
  PROCESSVARIATIONGENERATORX0_init__(&data__->PVG0,retain);
  PROCESSVARIATIONGENERATORX1_init__(&data__->PVG1,retain);
  PROCESSVARIATIONGENERATORX2_init__(&data__->PVG2,retain);
  PROCESSVARIATIONGENERATORX3_init__(&data__->PVG3,retain);
  PROCESSVARIATIONGENERATORX4_init__(&data__->PVG4,retain);
  PROCESSVARIATIONGENERATORX5_init__(&data__->PVG5,retain);
  PROCESSVARIATIONGENERATORX6_init__(&data__->PVG6,retain);
  PROCESSVARIATIONGENERATORX7_init__(&data__->PVG7,retain);
  PROCESSVARIATIONGENERATORX8_init__(&data__->PVG8,retain);
  PROCESSVARIATIONGENERATORX9_init__(&data__->PVG9,retain);
  PROCESSVARIATIONGENERATORX10_init__(&data__->PVG10,retain);
  PROCESSVARIATIONGENERATORX11_init__(&data__->PVG11,retain);
  PROCESSVARIATIONGENERATORX12_init__(&data__->PVG12,retain);
  PROCESSVARIATIONGENERATORX13_init__(&data__->PVG13,retain);
  PROCESSVARIATIONGENERATORX14_init__(&data__->PVG14,retain);
  PROCESSVARIATIONGENERATORX15_init__(&data__->PVG15,retain);
  __INIT_VAR(data__->INX0,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->INX1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX9,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX10,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX11,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX12,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX13,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX14,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX15,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX9,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX10,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX11,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX12,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX13,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX14,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX15,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->SIGNALX0,0,retain)
  __INIT_VAR(data__->SIGNALX1,0,retain)
  __INIT_VAR(data__->SIGNALX2,0,retain)
  __INIT_VAR(data__->SIGNALX3,0,retain)
  __INIT_VAR(data__->SIGNALX4,0,retain)
  __INIT_VAR(data__->SIGNALX5,0,retain)
  __INIT_VAR(data__->SIGNALX6,0,retain)
  __INIT_VAR(data__->SIGNALX7,0,retain)
  __INIT_VAR(data__->SIGNALX8,0,retain)
  __INIT_VAR(data__->SIGNALX9,0,retain)
  __INIT_VAR(data__->SIGNALX10,0,retain)
  __INIT_VAR(data__->SIGNALX11,0,retain)
  __INIT_VAR(data__->SIGNALX12,0,retain)
  __INIT_VAR(data__->SIGNALX13,0,retain)
  __INIT_VAR(data__->SIGNALX14,0,retain)
  __INIT_VAR(data__->SIGNALX15,0,retain)
  __INIT_VAR(data__->T0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->START,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVWEB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSWEB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVCOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSCOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->PUMPCOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->VALVECOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVLEVEL,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSLEVEL,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSDRYB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->AIRB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->HEATER,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->AIRA,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVQC,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSQC,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->RESET,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX9,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX10,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX11,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX12,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX13,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX14,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX15,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_AND184_OUT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_AND185_OUT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_AND186_OUT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_AND188_OUT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->_TMP_AND189_OUT,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void MAIN_body__(MAIN *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->PVG0.,STATE,,__GET_VAR(data__->INX0,));
  PROCESSVARIATIONGENERATORX0_body__(&data__->PVG0);
  __SET_VAR(data__->,INX0,,__GET_VAR(data__->PVG0.STATE));
  __SET_VAR(data__->,OUTX0,,__GET_VAR(data__->PVG0.STATE,));
  __SET_VAR(data__->PVG1.,STATE,,__GET_VAR(data__->INX1,));
  PROCESSVARIATIONGENERATORX1_body__(&data__->PVG1);
  __SET_VAR(data__->,INX1,,__GET_VAR(data__->PVG1.STATE));
  __SET_VAR(data__->,OUTX1,,__GET_VAR(data__->PVG1.STATE,));
  __SET_VAR(data__->PVG2.,STATE,,__GET_VAR(data__->INX2,));
  PROCESSVARIATIONGENERATORX2_body__(&data__->PVG2);
  __SET_VAR(data__->,INX2,,__GET_VAR(data__->PVG2.STATE));
  __SET_VAR(data__->,OUTX2,,__GET_VAR(data__->PVG2.STATE,));
  __SET_VAR(data__->PVG3.,STATE,,__GET_VAR(data__->INX3,));
  PROCESSVARIATIONGENERATORX3_body__(&data__->PVG3);
  __SET_VAR(data__->,INX3,,__GET_VAR(data__->PVG3.STATE));
  __SET_VAR(data__->,OUTX3,,__GET_VAR(data__->PVG3.STATE,));
  __SET_VAR(data__->PVG4.,STATE,,__GET_VAR(data__->INX4,));
  PROCESSVARIATIONGENERATORX4_body__(&data__->PVG4);
  __SET_VAR(data__->,INX4,,__GET_VAR(data__->PVG4.STATE));
  __SET_VAR(data__->,OUTX4,,__GET_VAR(data__->PVG4.STATE,));
  __SET_VAR(data__->PVG5.,STATE,,__GET_VAR(data__->INX5,));
  PROCESSVARIATIONGENERATORX5_body__(&data__->PVG5);
  __SET_VAR(data__->,INX5,,__GET_VAR(data__->PVG5.STATE));
  __SET_VAR(data__->,OUTX5,,__GET_VAR(data__->PVG5.STATE,));
  __SET_VAR(data__->PVG6.,STATE,,__GET_VAR(data__->INX6,));
  PROCESSVARIATIONGENERATORX6_body__(&data__->PVG6);
  __SET_VAR(data__->,INX6,,__GET_VAR(data__->PVG6.STATE));
  __SET_VAR(data__->,OUTX6,,__GET_VAR(data__->PVG6.STATE,));
  __SET_VAR(data__->PVG7.,STATE,,__GET_VAR(data__->INX7,));
  PROCESSVARIATIONGENERATORX7_body__(&data__->PVG7);
  __SET_VAR(data__->,INX7,,__GET_VAR(data__->PVG7.STATE));
  __SET_VAR(data__->,OUTX7,,__GET_VAR(data__->PVG7.STATE,));
  __SET_VAR(data__->PVG8.,STATE,,__GET_VAR(data__->INX8,));
  PROCESSVARIATIONGENERATORX8_body__(&data__->PVG8);
  __SET_VAR(data__->,INX8,,__GET_VAR(data__->PVG8.STATE));
  __SET_VAR(data__->,OUTX8,,__GET_VAR(data__->PVG8.STATE,));
  __SET_VAR(data__->PVG9.,STATE,,__GET_VAR(data__->INX9,));
  PROCESSVARIATIONGENERATORX9_body__(&data__->PVG9);
  __SET_VAR(data__->,INX9,,__GET_VAR(data__->PVG9.STATE));
  __SET_VAR(data__->,OUTX9,,__GET_VAR(data__->PVG9.STATE,));
  __SET_VAR(data__->PVG10.,STATE,,__GET_VAR(data__->INX10,));
  PROCESSVARIATIONGENERATORX10_body__(&data__->PVG10);
  __SET_VAR(data__->,INX10,,__GET_VAR(data__->PVG10.STATE));
  __SET_VAR(data__->,OUTX10,,__GET_VAR(data__->PVG10.STATE,));
  __SET_VAR(data__->PVG11.,STATE,,__GET_VAR(data__->INX11,));
  PROCESSVARIATIONGENERATORX11_body__(&data__->PVG11);
  __SET_VAR(data__->,INX11,,__GET_VAR(data__->PVG11.STATE));
  __SET_VAR(data__->,OUTX11,,__GET_VAR(data__->PVG11.STATE,));
  __SET_VAR(data__->PVG12.,STATE,,__GET_VAR(data__->INX12,));
  PROCESSVARIATIONGENERATORX12_body__(&data__->PVG12);
  __SET_VAR(data__->,INX12,,__GET_VAR(data__->PVG12.STATE));
  __SET_VAR(data__->,OUTX12,,__GET_VAR(data__->PVG12.STATE,));
  __SET_VAR(data__->PVG13.,STATE,,__GET_VAR(data__->INX13,));
  PROCESSVARIATIONGENERATORX13_body__(&data__->PVG13);
  __SET_VAR(data__->,INX13,,__GET_VAR(data__->PVG13.STATE));
  __SET_VAR(data__->,OUTX13,,__GET_VAR(data__->PVG13.STATE,));
  __SET_VAR(data__->PVG14.,STATE,,__GET_VAR(data__->INX14,));
  PROCESSVARIATIONGENERATORX14_body__(&data__->PVG14);
  __SET_VAR(data__->,INX14,,__GET_VAR(data__->PVG14.STATE));
  __SET_VAR(data__->,OUTX14,,__GET_VAR(data__->PVG14.STATE,));
  __SET_VAR(data__->PVG15.,STATE,,__GET_VAR(data__->INX15,));
  PROCESSVARIATIONGENERATORX15_body__(&data__->PVG15);
  __SET_VAR(data__->,INX15,,__GET_VAR(data__->PVG15.STATE));
  __SET_VAR(data__->,OUTX15,,__GET_VAR(data__->PVG15.STATE,));
  __SET_VAR(data__->,SIGNALX3,,__GET_VAR(data__->PVG3.OUT,));
  __SET_VAR(data__->,SIGNALX4,,__GET_VAR(data__->PVG4.OUT,));
  __SET_VAR(data__->,SIGNALX5,,__GET_VAR(data__->PVG5.OUT,));
  __SET_VAR(data__->,SIGNALX6,,__GET_VAR(data__->PVG6.OUT,));
  __SET_VAR(data__->,SIGNALX7,,__GET_VAR(data__->PVG7.OUT,));
  __SET_VAR(data__->,SIGNALX8,,__GET_VAR(data__->PVG8.OUT,));
  __SET_VAR(data__->,SIGNALX14,,__GET_VAR(data__->PVG14.OUT,));
  __SET_VAR(data__->,SIGNALX13,,__GET_VAR(data__->PVG13.OUT,));
  __SET_VAR(data__->,SIGNALX12,,__GET_VAR(data__->PVG12.OUT,));
  __SET_VAR(data__->,SIGNALX11,,__GET_VAR(data__->PVG11.OUT,));
  __SET_VAR(data__->,SIGNALX10,,__GET_VAR(data__->PVG10.OUT,));
  __SET_VAR(data__->,SIGNALX9,,__GET_VAR(data__->PVG9.OUT,));
  __SET_VAR(data__->,SIGNALX15,,__GET_VAR(data__->PVG15.OUT,));
  __SET_VAR(data__->,SIGNALX2,,__GET_VAR(data__->PVG2.OUT,));
  __SET_VAR(data__->,SIGNALX1,,__GET_VAR(data__->PVG1.OUT,));
  __SET_VAR(data__->,SIGNALX0,,__GET_VAR(data__->PVG0.OUT,));
  __SET_VAR(data__->,T0,,(__GET_VAR(data__->START,) && __GET_VAR(data__->INX0,)));
  __SET_VAR(data__->,T1,,(((__GET_VAR(data__->TENSWEB,) && __GET_VAR(data__->CONVWEB,)) && __GET_VAR(data__->INX2,)) && __GET_VAR(data__->INX1,)));
  __SET_VAR(data__->,T2,,(((((((__GET_VAR(data__->VALVECOAT,) && __GET_VAR(data__->PUMPCOAT,)) && __GET_VAR(data__->TENSCOAT,)) && __GET_VAR(data__->CONVCOAT,)) && __GET_VAR(data__->INX6,)) && __GET_VAR(data__->INX5,)) && __GET_VAR(data__->INX4,)) && __GET_VAR(data__->INX3,)));
  __SET_VAR(data__->,T3,,(((__GET_VAR(data__->TENSLEVEL,) && __GET_VAR(data__->CONVLEVEL,)) && __GET_VAR(data__->INX8,)) && __GET_VAR(data__->INX7,)));
  __SET_VAR(data__->,T4,,(((((((__GET_VAR(data__->AIRB,) && __GET_VAR(data__->TENSDRYB,)) && __GET_VAR(data__->AIRA,)) && __GET_VAR(data__->HEATER,)) && __GET_VAR(data__->INX12,)) && __GET_VAR(data__->INX11,)) && __GET_VAR(data__->INX10,)) && __GET_VAR(data__->INX9,)));
  __SET_VAR(data__->,T5,,(((__GET_VAR(data__->TENSQC,) && __GET_VAR(data__->CONVQC,)) && __GET_VAR(data__->INX14,)) && __GET_VAR(data__->INX13,)));
  __SET_VAR(data__->,T6,,(__GET_VAR(data__->RESET,) && __GET_VAR(data__->INX15,)));
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,INX1,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,INX2,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,INX0,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX3,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX4,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX5,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX6,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX1,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX2,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX8,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX3,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX4,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX5,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX6,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX7,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX9,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX10,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX11,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX12,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX7,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX8,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX13,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX14,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,INX13,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,INX14,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T6,)) {
    __SET_VAR(data__->,INX0,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T6,)) {
    __SET_VAR(data__->,INX15,,__BOOL_LITERAL(FALSE));
  };
  __SET_VAR(data__->,START,,__GET_VAR(data__->TERMX0,));
  __SET_VAR(data__->,_TMP_AND184_OUT,,AND__BOOL__BOOL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (UINT)2,
    (BOOL)__GET_VAR(data__->TERMX1,),
    (BOOL)__GET_VAR(data__->TERMX2,)));
  __SET_VAR(data__->,CONVWEB,,__GET_VAR(data__->_TMP_AND184_OUT,));
  __SET_VAR(data__->,TENSWEB,,__GET_VAR(data__->_TMP_AND184_OUT,));
  __SET_VAR(data__->,_TMP_AND185_OUT,,AND__BOOL__BOOL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (UINT)4,
    (BOOL)__GET_VAR(data__->TERMX3,),
    (BOOL)__GET_VAR(data__->TERMX4,),
    (BOOL)__GET_VAR(data__->TERMX5,),
    (BOOL)__GET_VAR(data__->TERMX6,)));
  __SET_VAR(data__->,VALVECOAT,,__GET_VAR(data__->_TMP_AND185_OUT,));
  __SET_VAR(data__->,PUMPCOAT,,__GET_VAR(data__->_TMP_AND185_OUT,));
  __SET_VAR(data__->,TENSCOAT,,__GET_VAR(data__->_TMP_AND185_OUT,));
  __SET_VAR(data__->,CONVCOAT,,__GET_VAR(data__->_TMP_AND185_OUT,));
  __SET_VAR(data__->,_TMP_AND186_OUT,,AND__BOOL__BOOL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (UINT)2,
    (BOOL)__GET_VAR(data__->TERMX7,),
    (BOOL)__GET_VAR(data__->TERMX8,)));
  __SET_VAR(data__->,TENSLEVEL,,__GET_VAR(data__->_TMP_AND186_OUT,));
  __SET_VAR(data__->,CONVLEVEL,,__GET_VAR(data__->_TMP_AND186_OUT,));
  __SET_VAR(data__->,_TMP_AND188_OUT,,AND__BOOL__BOOL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (UINT)4,
    (BOOL)__GET_VAR(data__->TERMX9,),
    (BOOL)__GET_VAR(data__->TERMX10,),
    (BOOL)__GET_VAR(data__->TERMX11,),
    (BOOL)__GET_VAR(data__->TERMX12,)));
  __SET_VAR(data__->,AIRB,,__GET_VAR(data__->_TMP_AND188_OUT,));
  __SET_VAR(data__->,TENSDRYB,,__GET_VAR(data__->_TMP_AND188_OUT,));
  __SET_VAR(data__->,AIRA,,__GET_VAR(data__->_TMP_AND188_OUT,));
  __SET_VAR(data__->,HEATER,,__GET_VAR(data__->_TMP_AND188_OUT,));
  __SET_VAR(data__->,_TMP_AND189_OUT,,AND__BOOL__BOOL(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (UINT)2,
    (BOOL)__GET_VAR(data__->TERMX13,),
    (BOOL)__GET_VAR(data__->TERMX14,)));
  __SET_VAR(data__->,TENSQC,,__GET_VAR(data__->_TMP_AND189_OUT,));
  __SET_VAR(data__->,CONVQC,,__GET_VAR(data__->_TMP_AND189_OUT,));
  __SET_VAR(data__->,TERMX0,,__GET_VAR(data__->PVG0.TERM,));
  __SET_VAR(data__->,TERMX1,,__GET_VAR(data__->PVG1.TERM,));
  __SET_VAR(data__->,TERMX2,,__GET_VAR(data__->PVG2.TERM,));
  __SET_VAR(data__->,TERMX3,,__GET_VAR(data__->PVG3.TERM,));
  __SET_VAR(data__->,TERMX4,,__GET_VAR(data__->PVG4.TERM,));
  __SET_VAR(data__->,TERMX5,,__GET_VAR(data__->PVG5.TERM,));
  __SET_VAR(data__->,TERMX6,,__GET_VAR(data__->PVG6.TERM,));
  __SET_VAR(data__->,TERMX7,,__GET_VAR(data__->PVG7.TERM,));
  __SET_VAR(data__->,TERMX8,,__GET_VAR(data__->PVG8.TERM,));
  __SET_VAR(data__->,TERMX9,,__GET_VAR(data__->PVG9.TERM,));
  __SET_VAR(data__->,TERMX10,,__GET_VAR(data__->PVG10.TERM,));
  __SET_VAR(data__->,TERMX11,,__GET_VAR(data__->PVG11.TERM,));
  __SET_VAR(data__->,TERMX12,,__GET_VAR(data__->PVG12.TERM,));
  __SET_VAR(data__->,TERMX13,,__GET_VAR(data__->PVG13.TERM,));
  __SET_VAR(data__->,TERMX14,,__GET_VAR(data__->PVG14.TERM,));
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,INX15,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX9,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX10,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX12,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX11,,__BOOL_LITERAL(FALSE));
  };
  __SET_VAR(data__->,TERMX15,,__GET_VAR(data__->PVG15.TERM,));
  __SET_VAR(data__->,RESET,,__GET_VAR(data__->TERMX15,));

  goto __end;

__end:
  return;
} // MAIN_body__() 





void MAIN2_init__(MAIN2 *data__, BOOL retain) {
  __INIT_VAR(data__->INX0,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->INX1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX9,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX10,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX11,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX12,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX13,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX14,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INX15,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX9,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX10,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX11,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX12,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX13,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX14,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUTX15,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->SIGNALX0,0,retain)
  __INIT_VAR(data__->SIGNALX1,0,retain)
  __INIT_VAR(data__->SIGNALX2,0,retain)
  __INIT_VAR(data__->SIGNALX3,0,retain)
  __INIT_VAR(data__->SIGNALX4,0,retain)
  __INIT_VAR(data__->SIGNALX5,0,retain)
  __INIT_VAR(data__->SIGNALX6,0,retain)
  __INIT_VAR(data__->SIGNALX7,0,retain)
  __INIT_VAR(data__->SIGNALX8,0,retain)
  __INIT_VAR(data__->SIGNALX9,0,retain)
  __INIT_VAR(data__->SIGNALX10,0,retain)
  __INIT_VAR(data__->SIGNALX11,0,retain)
  __INIT_VAR(data__->SIGNALX12,0,retain)
  __INIT_VAR(data__->SIGNALX13,0,retain)
  __INIT_VAR(data__->SIGNALX14,0,retain)
  __INIT_VAR(data__->SIGNALX15,0,retain)
  __INIT_VAR(data__->T0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->START,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVWEB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSWEB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVCOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSCOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->PUMPCOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->VALVECOAT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVLEVEL,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSLEVEL,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSDRYB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->AIRB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->HEATER,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->AIRA,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CONVQC,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TENSQC,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->RESET,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX9,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX10,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX11,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX12,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX13,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX14,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TERMX15,__BOOL_LITERAL(FALSE),retain)
  PROCESSVARIATIONGENERATORX0_init__(&data__->PVG0,retain);
  PROCESSVARIATIONGENERATORX1_init__(&data__->PVG1,retain);
  PROCESSVARIATIONGENERATORX2_init__(&data__->PVG2,retain);
  PROCESSVARIATIONGENERATORX3_init__(&data__->PVG3,retain);
  PROCESSVARIATIONGENERATORX4_init__(&data__->PVG4,retain);
  PROCESSVARIATIONGENERATORX5_init__(&data__->PVG5,retain);
  PROCESSVARIATIONGENERATORX6_init__(&data__->PVG6,retain);
  PROCESSVARIATIONGENERATORX7_init__(&data__->PVG7,retain);
  PROCESSVARIATIONGENERATORX8_init__(&data__->PVG8,retain);
  PROCESSVARIATIONGENERATORX9_init__(&data__->PVG9,retain);
  PROCESSVARIATIONGENERATORX10_init__(&data__->PVG10,retain);
  PROCESSVARIATIONGENERATORX11_init__(&data__->PVG11,retain);
  PROCESSVARIATIONGENERATORX12_init__(&data__->PVG12,retain);
  PROCESSVARIATIONGENERATORX13_init__(&data__->PVG13,retain);
  PROCESSVARIATIONGENERATORX14_init__(&data__->PVG14,retain);
  PROCESSVARIATIONGENERATORX15_init__(&data__->PVG15,retain);
}

// Code part
void MAIN2_body__(MAIN2 *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->PVG0.,STATE,,__GET_VAR(data__->INX0,));
  PROCESSVARIATIONGENERATORX0_body__(&data__->PVG0);
  __SET_VAR(data__->,INX0,,__GET_VAR(data__->PVG0.STATE));
  __SET_VAR(data__->,OUTX0,,__GET_VAR(data__->PVG0.STATE,));
  __SET_VAR(data__->PVG1.,STATE,,__GET_VAR(data__->INX1,));
  PROCESSVARIATIONGENERATORX1_body__(&data__->PVG1);
  __SET_VAR(data__->,INX1,,__GET_VAR(data__->PVG1.STATE));
  __SET_VAR(data__->,OUTX1,,__GET_VAR(data__->PVG1.STATE,));
  __SET_VAR(data__->PVG2.,STATE,,__GET_VAR(data__->INX2,));
  PROCESSVARIATIONGENERATORX2_body__(&data__->PVG2);
  __SET_VAR(data__->,INX2,,__GET_VAR(data__->PVG2.STATE));
  __SET_VAR(data__->,OUTX2,,__GET_VAR(data__->PVG2.STATE,));
  __SET_VAR(data__->PVG3.,STATE,,__GET_VAR(data__->INX3,));
  PROCESSVARIATIONGENERATORX3_body__(&data__->PVG3);
  __SET_VAR(data__->,INX3,,__GET_VAR(data__->PVG3.STATE));
  __SET_VAR(data__->,OUTX3,,__GET_VAR(data__->PVG3.STATE,));
  __SET_VAR(data__->PVG4.,STATE,,__GET_VAR(data__->INX4,));
  PROCESSVARIATIONGENERATORX4_body__(&data__->PVG4);
  __SET_VAR(data__->,INX4,,__GET_VAR(data__->PVG4.STATE));
  __SET_VAR(data__->,OUTX4,,__GET_VAR(data__->PVG4.STATE,));
  __SET_VAR(data__->PVG5.,STATE,,__GET_VAR(data__->INX5,));
  PROCESSVARIATIONGENERATORX5_body__(&data__->PVG5);
  __SET_VAR(data__->,INX5,,__GET_VAR(data__->PVG5.STATE));
  __SET_VAR(data__->,OUTX5,,__GET_VAR(data__->PVG5.STATE,));
  __SET_VAR(data__->PVG6.,STATE,,__GET_VAR(data__->INX6,));
  PROCESSVARIATIONGENERATORX6_body__(&data__->PVG6);
  __SET_VAR(data__->,INX6,,__GET_VAR(data__->PVG6.STATE));
  __SET_VAR(data__->,OUTX6,,__GET_VAR(data__->PVG6.STATE,));
  __SET_VAR(data__->PVG7.,STATE,,__GET_VAR(data__->INX7,));
  PROCESSVARIATIONGENERATORX7_body__(&data__->PVG7);
  __SET_VAR(data__->,INX7,,__GET_VAR(data__->PVG7.STATE));
  __SET_VAR(data__->,OUTX7,,__GET_VAR(data__->PVG7.STATE,));
  __SET_VAR(data__->PVG8.,STATE,,__GET_VAR(data__->INX8,));
  PROCESSVARIATIONGENERATORX8_body__(&data__->PVG8);
  __SET_VAR(data__->,INX8,,__GET_VAR(data__->PVG8.STATE));
  __SET_VAR(data__->,OUTX8,,__GET_VAR(data__->PVG8.STATE,));
  __SET_VAR(data__->PVG9.,STATE,,__GET_VAR(data__->INX9,));
  PROCESSVARIATIONGENERATORX9_body__(&data__->PVG9);
  __SET_VAR(data__->,INX9,,__GET_VAR(data__->PVG9.STATE));
  __SET_VAR(data__->,OUTX9,,__GET_VAR(data__->PVG9.STATE,));
  __SET_VAR(data__->PVG10.,STATE,,__GET_VAR(data__->INX10,));
  PROCESSVARIATIONGENERATORX10_body__(&data__->PVG10);
  __SET_VAR(data__->,INX10,,__GET_VAR(data__->PVG10.STATE));
  __SET_VAR(data__->,OUTX10,,__GET_VAR(data__->PVG10.STATE,));
  __SET_VAR(data__->PVG11.,STATE,,__GET_VAR(data__->INX11,));
  PROCESSVARIATIONGENERATORX11_body__(&data__->PVG11);
  __SET_VAR(data__->,INX11,,__GET_VAR(data__->PVG11.STATE));
  __SET_VAR(data__->,OUTX11,,__GET_VAR(data__->PVG11.STATE,));
  __SET_VAR(data__->PVG12.,STATE,,__GET_VAR(data__->INX12,));
  PROCESSVARIATIONGENERATORX12_body__(&data__->PVG12);
  __SET_VAR(data__->,INX12,,__GET_VAR(data__->PVG12.STATE));
  __SET_VAR(data__->,OUTX12,,__GET_VAR(data__->PVG12.STATE,));
  __SET_VAR(data__->PVG13.,STATE,,__GET_VAR(data__->INX13,));
  PROCESSVARIATIONGENERATORX13_body__(&data__->PVG13);
  __SET_VAR(data__->,INX13,,__GET_VAR(data__->PVG13.STATE));
  __SET_VAR(data__->,OUTX13,,__GET_VAR(data__->PVG13.STATE,));
  __SET_VAR(data__->PVG14.,STATE,,__GET_VAR(data__->INX14,));
  PROCESSVARIATIONGENERATORX14_body__(&data__->PVG14);
  __SET_VAR(data__->,INX14,,__GET_VAR(data__->PVG14.STATE));
  __SET_VAR(data__->,OUTX14,,__GET_VAR(data__->PVG14.STATE,));
  __SET_VAR(data__->PVG15.,STATE,,__GET_VAR(data__->INX15,));
  PROCESSVARIATIONGENERATORX15_body__(&data__->PVG15);
  __SET_VAR(data__->,INX15,,__GET_VAR(data__->PVG15.STATE));
  __SET_VAR(data__->,OUTX15,,__GET_VAR(data__->PVG15.STATE,));
  __SET_VAR(data__->,SIGNALX3,,__GET_VAR(data__->PVG3.OUT,));
  __SET_VAR(data__->,SIGNALX4,,__GET_VAR(data__->PVG4.OUT,));
  __SET_VAR(data__->,SIGNALX5,,__GET_VAR(data__->PVG5.OUT,));
  __SET_VAR(data__->,SIGNALX6,,__GET_VAR(data__->PVG6.OUT,));
  __SET_VAR(data__->,SIGNALX7,,__GET_VAR(data__->PVG7.OUT,));
  __SET_VAR(data__->,SIGNALX8,,__GET_VAR(data__->PVG8.OUT,));
  __SET_VAR(data__->,SIGNALX14,,__GET_VAR(data__->PVG14.OUT,));
  __SET_VAR(data__->,SIGNALX13,,__GET_VAR(data__->PVG13.OUT,));
  __SET_VAR(data__->,SIGNALX12,,__GET_VAR(data__->PVG12.OUT,));
  __SET_VAR(data__->,SIGNALX11,,__GET_VAR(data__->PVG11.OUT,));
  __SET_VAR(data__->,SIGNALX10,,__GET_VAR(data__->PVG10.OUT,));
  __SET_VAR(data__->,SIGNALX9,,__GET_VAR(data__->PVG9.OUT,));
  __SET_VAR(data__->,SIGNALX15,,__GET_VAR(data__->PVG15.OUT,));
  __SET_VAR(data__->,SIGNALX2,,__GET_VAR(data__->PVG2.OUT,));
  __SET_VAR(data__->,SIGNALX1,,__GET_VAR(data__->PVG1.OUT,));
  __SET_VAR(data__->,SIGNALX0,,__GET_VAR(data__->PVG0.OUT,));
  __SET_VAR(data__->,T0,,(__GET_VAR(data__->START,) && __GET_VAR(data__->INX0,)));
  __SET_VAR(data__->,T1,,(((__GET_VAR(data__->TENSWEB,) && __GET_VAR(data__->CONVWEB,)) && __GET_VAR(data__->INX2,)) && __GET_VAR(data__->INX1,)));
  __SET_VAR(data__->,T2,,(((((((__GET_VAR(data__->VALVECOAT,) && __GET_VAR(data__->PUMPCOAT,)) && __GET_VAR(data__->TENSCOAT,)) && __GET_VAR(data__->CONVCOAT,)) && __GET_VAR(data__->INX6,)) && __GET_VAR(data__->INX5,)) && __GET_VAR(data__->INX4,)) && __GET_VAR(data__->INX3,)));
  __SET_VAR(data__->,T3,,(((__GET_VAR(data__->TENSLEVEL,) && __GET_VAR(data__->CONVLEVEL,)) && __GET_VAR(data__->INX8,)) && __GET_VAR(data__->INX7,)));
  __SET_VAR(data__->,T4,,(((((((__GET_VAR(data__->AIRB,) && __GET_VAR(data__->TENSDRYB,)) && __GET_VAR(data__->AIRA,)) && __GET_VAR(data__->HEATER,)) && __GET_VAR(data__->INX12,)) && __GET_VAR(data__->INX11,)) && __GET_VAR(data__->INX10,)) && __GET_VAR(data__->INX9,)));
  __SET_VAR(data__->,T5,,(((__GET_VAR(data__->TENSQC,) && __GET_VAR(data__->CONVQC,)) && __GET_VAR(data__->INX14,)) && __GET_VAR(data__->INX13,)));
  __SET_VAR(data__->,T6,,(__GET_VAR(data__->RESET,) && __GET_VAR(data__->INX15,)));
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,INX1,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,INX2,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,INX0,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX3,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX4,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX5,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX6,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX1,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,INX2,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX8,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX3,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX4,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX5,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX6,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,INX7,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX9,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX10,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX11,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX12,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX7,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,INX8,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX13,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX14,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,INX13,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,INX14,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T6,)) {
    __SET_VAR(data__->,INX0,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T6,)) {
    __SET_VAR(data__->,INX15,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX9,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX10,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX11,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,INX12,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,INX15,,__BOOL_LITERAL(TRUE));
  };
  __SET_VAR(data__->,TERMX15,,__GET_VAR(data__->PVG15.TERM,));
  __SET_VAR(data__->,TERMX7,,__GET_VAR(data__->PVG7.TERM,));
  __SET_VAR(data__->,TERMX0,,__GET_VAR(data__->PVG0.TERM,));
  __SET_VAR(data__->,TERMX1,,__GET_VAR(data__->PVG1.TERM,));
  __SET_VAR(data__->,TERMX2,,__GET_VAR(data__->PVG2.TERM,));
  __SET_VAR(data__->,TERMX3,,__GET_VAR(data__->PVG3.TERM,));
  __SET_VAR(data__->,TERMX4,,__GET_VAR(data__->PVG4.TERM,));
  __SET_VAR(data__->,TERMX5,,__GET_VAR(data__->PVG5.TERM,));
  __SET_VAR(data__->,TERMX12,,__GET_VAR(data__->PVG12.TERM,));
  __SET_VAR(data__->,TERMX13,,__GET_VAR(data__->PVG13.TERM,));
  __SET_VAR(data__->,TERMX11,,__GET_VAR(data__->PVG11.TERM,));
  __SET_VAR(data__->,TERMX14,,__GET_VAR(data__->PVG14.TERM,));
  __SET_VAR(data__->,TERMX10,,__GET_VAR(data__->PVG10.TERM,));
  __SET_VAR(data__->,TERMX9,,__GET_VAR(data__->PVG9.TERM,));
  __SET_VAR(data__->,TERMX8,,__GET_VAR(data__->PVG8.TERM,));
  __SET_VAR(data__->,TERMX6,,__GET_VAR(data__->PVG6.TERM,));

  goto __end;

__end:
  return;
} // MAIN2_body__() 





