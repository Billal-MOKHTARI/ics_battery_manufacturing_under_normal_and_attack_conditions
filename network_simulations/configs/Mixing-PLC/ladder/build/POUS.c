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





void MIXING_init__(MIXING *data__, BOOL retain) {
  __INIT_VAR(data__->M,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->A,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->B,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->D1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->D2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->E1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->E2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->F,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->G,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->L,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->EA,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->EB,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ED,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->EE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->EF,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->EG,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->M1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->M2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X0,__BOOL_LITERAL(TRUE),retain)
  __INIT_VAR(data__->X1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->X8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T0,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T1,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T2,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T3,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T4,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T5,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T6,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T7,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->T8,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->PV,3,retain)
  __INIT_VAR(data__->CV,0,retain)
  __INIT_VAR(data__->TIME_1,0,retain)
  __INIT_VAR(data__->TIME_2,0,retain)
  __INIT_VAR(data__->TIMER_GAUGE_1,5,retain)
  __INIT_VAR(data__->TIMER_GAUGE_2,5,retain)
  __INIT_VAR(data__->CU,__BOOL_LITERAL(TRUE),retain)
  TP_init__(&data__->TP1,retain);
  TP_init__(&data__->TP2,retain);
  CTU_init__(&data__->CTU0,retain);
  TON_init__(&data__->TON0,retain);
  __INIT_VAR(data__->_TMP_INT_TO_TIME191_OUT,__time_to_timespec(1, 0, 0, 0, 0, 0),retain)
  __INIT_VAR(data__->_TMP_INT_TO_TIME42_OUT,__time_to_timespec(1, 0, 0, 0, 0, 0),retain)
  R_TRIG_init__(&data__->R_TRIG1,retain);
  __INIT_VAR(data__->_TMP_INT_TO_TIME43_OUT,__time_to_timespec(1, 0, 0, 0, 0, 0),retain)
  __INIT_VAR(data__->_TMP_TIME_TO_INT188_OUT,0,retain)
  __INIT_VAR(data__->_TMP_TIME_TO_INT40_OUT,0,retain)
}

// Code part
void MIXING_body__(MIXING *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->,T0,,(__GET_VAR(data__->M,) && __GET_VAR(data__->X0,)));
  __SET_VAR(data__->,T1,,(__GET_VAR(data__->D1,) && __GET_VAR(data__->X1,)));
  __SET_VAR(data__->,T2,,(__GET_VAR(data__->A,) && __GET_VAR(data__->X2,)));
  __SET_VAR(data__->,T3,,(__GET_VAR(data__->E1,) && __GET_VAR(data__->X4,)));
  __SET_VAR(data__->,T4,,(__GET_VAR(data__->B,) && __GET_VAR(data__->X5,)));
  __SET_VAR(data__->,T5,,(((__GET_VAR(data__->E2,) && __GET_VAR(data__->D2,)) && __GET_VAR(data__->X6,)) && __GET_VAR(data__->X3,)));
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,X0,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,X1,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T0,)) {
    __SET_VAR(data__->,X4,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,X1,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T1,)) {
    __SET_VAR(data__->,X2,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,X2,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T2,)) {
    __SET_VAR(data__->,X3,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,X4,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T3,)) {
    __SET_VAR(data__->,X5,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,X5,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T4,)) {
    __SET_VAR(data__->,X6,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,X3,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,X6,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T6,)) {
    __SET_VAR(data__->,X7,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T6,)) {
    __SET_VAR(data__->,X8,,__BOOL_LITERAL(TRUE));
  };
  __SET_VAR(data__->,EA,,__GET_VAR(data__->X1,));
  __SET_VAR(data__->,ED,,__GET_VAR(data__->X3,));
  __SET_VAR(data__->,EB,,__GET_VAR(data__->X4,));
  __SET_VAR(data__->,EE,,__GET_VAR(data__->X6,));
  __SET_VAR(data__->,EF,,__GET_VAR(data__->X7,));
  __SET_VAR(data__->,_TMP_INT_TO_TIME191_OUT,,INT_TO_TIME(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (INT)__GET_VAR(data__->TIMER_GAUGE_2,)));
  __SET_VAR(data__->TP2.,IN,,__GET_VAR(data__->X8,));
  __SET_VAR(data__->TP2.,PT,,__GET_VAR(data__->_TMP_INT_TO_TIME191_OUT,));
  TP_body__(&data__->TP2);
  __SET_VAR(data__->,M2,,__GET_VAR(data__->TP2.Q,));
  __SET_VAR(data__->,_TMP_INT_TO_TIME42_OUT,,INT_TO_TIME(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (INT)__GET_VAR(data__->TIMER_GAUGE_1,)));
  __SET_VAR(data__->TP1.,IN,,__GET_VAR(data__->X7,));
  __SET_VAR(data__->TP1.,PT,,__GET_VAR(data__->_TMP_INT_TO_TIME42_OUT,));
  TP_body__(&data__->TP1);
  __SET_VAR(data__->,M1,,__GET_VAR(data__->TP1.Q,));
  __SET_VAR(data__->,EG,,__GET_VAR(data__->X8,));
  __SET_VAR(data__->R_TRIG1.,CLK,,(__GET_VAR(data__->X8,) && __GET_VAR(data__->CU,)));
  R_TRIG_body__(&data__->R_TRIG1);
  __SET_VAR(data__->CTU0.,CU,,__GET_VAR(data__->R_TRIG1.Q,));
  __SET_VAR(data__->CTU0.,R,,__GET_VAR(data__->T8,));
  __SET_VAR(data__->CTU0.,PV,,__GET_VAR(data__->PV,));
  CTU_body__(&data__->CTU0);
  __SET_VAR(data__->,_TMP_INT_TO_TIME43_OUT,,INT_TO_TIME(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (INT)__GET_VAR(data__->TIMER_GAUGE_2,)));
  __SET_VAR(data__->TON0.,IN,,__GET_VAR(data__->CTU0.Q,));
  __SET_VAR(data__->TON0.,PT,,__GET_VAR(data__->_TMP_INT_TO_TIME43_OUT,));
  TON_body__(&data__->TON0);
  __SET_VAR(data__->,T8,,__GET_VAR(data__->TON0.Q,));
  __SET_VAR(data__->,CV,,__GET_VAR(data__->CTU0.CV,));
  __SET_VAR(data__->,T6,,((__GET_VAR(data__->L,) && __GET_VAR(data__->F,)) && __GET_VAR(data__->X7,)));
  if (__GET_VAR(data__->T5,)) {
    __SET_VAR(data__->,X7,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T7,)) {
    __SET_VAR(data__->,X8,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T7,)) {
    __SET_VAR(data__->,X7,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->T8,)) {
    __SET_VAR(data__->,X8,,__BOOL_LITERAL(FALSE));
  };
  if (__GET_VAR(data__->T8,)) {
    __SET_VAR(data__->,X0,,__BOOL_LITERAL(TRUE));
  };
  __SET_VAR(data__->,T7,,(__GET_VAR(data__->G,) && __GET_VAR(data__->X8,)));
  __SET_VAR(data__->,_TMP_TIME_TO_INT188_OUT,,TIME_TO_INT(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (TIME)__GET_VAR(data__->TP1.ET,)));
  __SET_VAR(data__->,TIME_1,,__GET_VAR(data__->_TMP_TIME_TO_INT188_OUT,));
  __SET_VAR(data__->,_TMP_TIME_TO_INT40_OUT,,TIME_TO_INT(
    (BOOL)__BOOL_LITERAL(TRUE),
    NULL,
    (TIME)__GET_VAR(data__->TP2.ET,)));
  __SET_VAR(data__->,TIME_2,,__GET_VAR(data__->_TMP_TIME_TO_INT40_OUT,));

  goto __end;

__end:
  return;
} // MIXING_body__() 





