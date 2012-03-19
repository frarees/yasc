#include "token.h"
#include <string.h>

Token *token_copy(Token *destino, Token *origen)
{
  destino->tipo = origen->tipo;
  strcpy(destino->nombre, origen->nombre);
  return destino;
}
