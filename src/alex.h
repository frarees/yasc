// ANALIZADOR LEXICO

#ifndef _ALEX_H_
#define _ALEX_H_

#include <stdio.h>
#include "token.h"

typedef struct ALex {
  FILE *file;
  char  c;
  int   iNombre, iFila, iColumna;
  Token token;
  int (*estado)(struct ALex *x);
} ALex;

ALex *alex_open(char *fichero);
void  alex_close(ALex *x);
int   alex_read(ALex *x, Token *t);

int alex_error(ALex *x);
int alex_token(ALex *x, Tipo tipo);
int alex_estado(ALex *x, int addCaracter, int(*estado)(ALex *));
int alex_estadoInicio(ALex *x);

#endif // _ALEX_H_
