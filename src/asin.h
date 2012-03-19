// ANALIZADOR SINTACTICO

#ifndef _ASIN_H_
#define _ASIN_H_

#include "alex.h"
#include "code.h"

typedef struct ASin {
  ALex  *lex;
  Token *token;
  Code  *code;
} ASin;

ASin *asin_open(char *fichero);
void  asin_close(ASin *x);
void  asin_come(ASin *x, Tipo t);
void  asin_error(ASin *x);

#endif // _ASIN_H_
