#include <ctype.h>
#include <string.h>
#include "afd.h"

int ispermitedchar(int c)
{
  return isalpha(c) || isdigit(c) || c == '_';
}

int afd_inicio(ALex *x)
{
  if (x->c == 'e')
    return alex_estado(x, 1, afd_e);
  else if (x->c == 'i')
    return alex_estado(x, 1, afd_i);
  else if (x->c == 's')
    return alex_estado(x, 1, afd_s);
  else if (x->c == 'w')
    return alex_estado(x, 1, afd_w);
  else if (x->c == 'p')
    return alex_estado(x, 1, afd_p);
  else if (x->c == '"')
    return alex_estado(x, 0, afd_cadena);
  else if (x->c == '*')
    return alex_estado(x, 1, afd_mul);
  else if (x->c == '/')
    return alex_estado(x, 1, afd_mul_comment);
  else if (x->c == ':')
    return alex_estado(x, 1, afd_asign);
  else if (x->c == '+' || x->c == '-')
    return alex_estado(x, 1, afd_sum);
  else if (x->c == '!')
    return alex_estado(x, 1, afd_neg);
  else if (x->c == '|')
    return alex_estado(x, 1, afd_or);
  else if (x->c == '&')
    return alex_estado(x, 1, afd_and);
  else if (x->c == '(')
    return alex_estado(x, 1, afd_ipar);
  else if (x->c == ')')
    return alex_estado(x, 1, afd_dpar);
  else if (x->c == '>' || x->c == '<')
    return alex_estado(x, 1, afd_rel);
  else if (x->c == '=')
    return alex_estado(x, 1, afd_rel_fin);
  else if (isalpha(x->c))
    return alex_estado(x, 1, afd_id);
  else if (isdigit(x->c))
    return alex_estado(x, 1, afd_numero);
  else if (isspace(x->c))
    return alex_estado(x, 0, afd_inicio);
  else if (x->c == EOF)
    return 0;
  else
    return alex_error(x);
}

int afd_comentario_linea(ALex *x)
{
  if (x->c == '\n')
    return alex_estadoInicio(x);
  else
    return alex_estado(x, 1, afd_comentario_linea);

}

int afd_comentario_bloque(ALex *x)
{
  if (x->c == '*')
    return alex_estado(x, 1, afd_comentario_bloque_fin);
  else
    return alex_estado(x, 0, afd_comentario_bloque);
}

int afd_comentario_bloque_fin(ALex *x)
{
  if (x->c == '/')
    return alex_estadoInicio(x);
  else
    return alex_estado(x, 0, afd_comentario_bloque);
}

int afd_rel(ALex *x)
{
  if (x->c == '=')
    return alex_estado(x, 1, afd_rel_fin);
  else
    return alex_token(x, TOKEN_REL);
}

int afd_rel_fin(ALex *x)
{
  return alex_token(x, TOKEN_REL);
}

int afd_dpar(ALex *x)
{
  return alex_token(x, TOKEN_DPAR);
}

int afd_ipar(ALex *x)
{
  return alex_token(x, TOKEN_IPAR);
}

int afd_asign(ALex *x)
{
  if (x->c == '=')
    return alex_estado(x, 1, afd_asign_fin);
  else
    return alex_error(x);
}

int afd_asign_fin(ALex *x)
{
  return alex_token(x, TOKEN_ASIGN);
}

int afd_or(ALex *x)
{
  if (x->c == '|')
    return alex_estado(x, 1, afd_or_fin);
  else
    return alex_error(x);
}

int afd_or_fin(ALex *x)
{
  return alex_token(x, TOKEN_OR);
}

int afd_and(ALex *x)
{
  if (x->c == '&')
    return alex_estado(x, 1, afd_and_fin);
  else
    return alex_error(x);
}

int afd_and_fin(ALex *x)
{
  return alex_token(x, TOKEN_AND);
}

int afd_id(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_mul_comment(ALex *x)
{
  if (x->c == '*')
    return alex_estado(x, 1, afd_comentario_bloque);
  else if (x->c == '/')
    return alex_estado(x, 1, afd_comentario_linea);
  else
    return alex_token(x, TOKEN_MUL);
}

int afd_mul(ALex *x)
{
  return alex_token(x, TOKEN_MUL);
}

int afd_sum(ALex *x)
{
  return alex_token(x, TOKEN_SUM);
}

int afd_neg(ALex *x)
{
  if (x->c == '=')
    return alex_estado(x, 1, afd_rel_fin);
  else
    return alex_token(x, TOKEN_NEG);
}

int afd_numero(ALex *x)
{
  if (isdigit(x->c))
    return alex_estado(x, 1, afd_numero);
  else
    return alex_token(x, TOKEN_NUMERO);
}

int afd_e(ALex *x)
{
  if (x->c == 'n')
    return alex_estado(x, 1, afd_en);
  else if (x->c == 'l')
    return alex_estado(x, 1, afd_el);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_en(ALex *x)
{
  if (x->c == 'd')
    return alex_estado(x, 1, afd_end);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_end(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_END);
}

int afd_el(ALex *x)
{
  if (x->c == 's')
    return alex_estado(x, 1, afd_els);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_els(ALex *x)
{
  if (x->c == 'e')
    return alex_estado(x, 1, afd_else);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_else(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ELSE);
}

int afd_i(ALex *x)
{
  if (x->c == 'f')
    return alex_estado(x, 1, afd_if);
  else if (x->c == 'n')
    return alex_estado(x, 1, afd_in);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_if(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_IF);
}

int afd_in(ALex *x)
{
  if (x->c == 't')
    return alex_estado(x, 1, afd_int);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_int(ALex *x)
{
  if (x->c == 'e')
    return alex_estado(x, 1, afd_inte);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_inte(ALex *x)
{
  if (x->c == 'g')
    return alex_estado(x, 1, afd_integ);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_integ(ALex *x)
{
  if (x->c == 'e')
    return alex_estado(x, 1, afd_intege);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_intege(ALex *x)
{
  if (x->c == 'r')
    return alex_estado(x, 1, afd_integer);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_integer(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_INTEGER);
}

int afd_w(ALex *x)
{
  if (x->c == 'h')
    return alex_estado(x, 1, afd_wh);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_wh(ALex *x)
{
  if (x->c == 'i')
    return alex_estado(x, 1, afd_whi);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_whi(ALex *x)
{
  if (x->c == 'l')
    return alex_estado(x, 1, afd_whil);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_whil(ALex *x)
{
  if (x->c == 'e')
    return alex_estado(x, 1, afd_while);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_while(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_WHILE);
}

int afd_s(ALex *x)
{
  if (x->c == 't')
    return alex_estado(x, 1, afd_st);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_st(ALex *x)
{
  if (x->c == 'r')
    return alex_estado(x, 1, afd_str);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_str(ALex *x)
{
  if (x->c == 'i')
    return alex_estado(x, 1, afd_stri);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_stri(ALex *x)
{
  if (x->c == 'n')
    return alex_estado(x, 1, afd_strin);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_strin(ALex *x)
{
  if (x->c == 'g')
    return alex_estado(x, 1, afd_string);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_string(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_STRING);
}

int afd_p(ALex *x)
{
  if (x->c == 'r')
    return alex_estado(x, 1, afd_pr);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_pr(ALex *x)
{
  if (x->c == 'i')
    return alex_estado(x, 1, afd_pri);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_pri(ALex *x)
{
  if (x->c == 'n')
    return alex_estado(x, 1, afd_prin);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_prin(ALex *x)
{
  if (x->c == 't')
    return alex_estado(x, 1, afd_print);
  else if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_ID);
}

int afd_print(ALex *x)
{
  if (ispermitedchar(x->c))
    return alex_estado(x, 1, afd_id);
  else
    return alex_token(x, TOKEN_PRINT);
}

int afd_cadena(ALex *x)
{
  if (x->c == '"')
    return alex_estado(x, 0, afd_cadena_fin);

  if (x->c == '\n') {
    return alex_estado(x, 0, afd_cadena);
  }
  else
    return alex_estado(x, 1, afd_cadena);
}

int afd_cadena_fin(ALex *x)
{
  return alex_token(x, TOKEN_CADENA);
}

