//////////////////////////////////////////////////////////////////////////////
// Product:  Board Support Package (BSP) for the Calculator example
// Date of the Last Update:  Oct 30, 2007
//
// Copyright (C) 2002-2007 Miro Samek. All rights reserved.
//
// Contact information:
// e-mail: miro@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#ifndef bsp_h
#define bsp_h

#define KEY_PLUS    '+'
#define KEY_MINUS   '-'
#define KEY_MULT    '*'
#define KEY_DIVIDE  '/'

void BSP_clear (void);
void BSP_negate(void);
void BSP_insert(int keyId);
double BSP_get_value(void);
int  BSP_eval(double operand1, int oper, double operand2);
void BSP_exit(void);

void BSP_display(void);
void BSP_message(char const *state);

#endif                                                                // bsp_h
