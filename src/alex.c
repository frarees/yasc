#include <stdlib.h>
#include "afd.h"

ALex *alex_open(char *fichero)
{
  FILE *file = fopen(fichero, "r");

  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", fichero);
    exit(0);
  }

  ALex *x = malloc(sizeof(ALex));
  x->file = file;
  x->iFila = 1;
  x->iColumna = 0;
  return x;
}

void alex_close(ALex *x)
{
  fclose(x->file);
  free(x);
}

int alex_estadoInicio(ALex *x)
{
  x->estado = afd_inicio;
  x->iNombre = 0;
  x->token.nombre[0] = '\0';
  return 0;
}

int alex_estado(ALex *x, int addCaracter, int(*estado)(ALex *))
{
  if (addCaracter) {
    if (x->iNombre >= TOKEN_MAX_LENGTH) {
      int columna = x->iColumna - x->iNombre;
      fprintf(stderr, "Token too long at %d:%d : %s...\n",
              x->iFila, columna, x->token.nombre);
      exit(0);
    }

    x->token.nombre[x->iNombre] = x->c;
    x->iNombre++;
    x->token.nombre[x->iNombre] = '\0';
  }

  x->estado = estado;
  return 0;
}

int alex_error(ALex *x)
{
  fprintf(stderr, "Character not allowed at %d:%d : %c\n",
          x->iFila, x->iColumna, x->c);
  exit(0); // comentar si queremos seguir comprobando...
  return 0;
}

int alex_token(ALex *x, Tipo tipo)
{
  if (x->c == '\n')
    x->iFila--;
  else
    x->iColumna--;

  ungetc(x->c, x->file);
  x->token.tipo = tipo;
  return 1;
}

void leerCaracter(ALex *x)
{
  x->c = fgetc(x->file);
  x->iColumna++;

  if (x->c == '\n') {
    x->iFila++;
    x->iColumna = 0;
  }
}

int alex_read(ALex *x, Token *t)
{
  alex_estadoInicio(x);

  for (;;) {
    leerCaracter(x);

    if (x->estado(x)) {
      token_copy(t, &x->token);
      return 1;
    }
    else if (x->c == EOF) {
      t->tipo = TOKEN_EOF;
      return 0;
    }
  }
}
