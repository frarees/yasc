#include <stdlib.h>
#include "asin.h"
#include "code.h"

ASin *asin_open(char *fichero)
{
  ASin *x = malloc(sizeof(ASin));
  x->lex = alex_open(fichero);
  x->code = code_new();
  x->token = (Token *)malloc(sizeof(Token));
  alex_read(x->lex, x->token);
  return x;
}

void asin_close(ASin *x)
{
  alex_close(x->lex);
  code_free(x->code);
  free(x->token);
  free(x);
}

void asin_come(ASin *x, Tipo t)
{
  if (x->token->tipo != t)
    asin_error(x);

  code_setToken(x->code, x->token, x->lex->iFila, x->lex->iColumna);
  alex_read(x->lex, x->token);
}

void asin_error(ASin *x)
{
  fprintf(stderr, "Error (%d:%d): token %d, value: %s\n",
          x->lex->iFila, x->lex->iColumna,
          x->token->tipo, x->token->nombre);
  exit(0);
}
