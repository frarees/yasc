#include <stdlib.h>
#include <string.h>
#include "adr.h"

/*
 PRECEDENCIA:
 *  /
 +  -
 =  !=
 && ||
 */

void adr_programa(ASin *x)
{
  adr_declaracion(x);
  adr_bloque(x);
  asin_come(x, TOKEN_EOF);
}

void adr_declaracion(ASin *x)
{
  if (x->token->tipo == TOKEN_STRING) {
    asin_come(x, TOKEN_STRING);
    asin_come(x, TOKEN_ID);
    code_declString(x->code);
    adr_declaracion(x);
  }
  else if (x->token->tipo == TOKEN_INTEGER) {
    asin_come(x, TOKEN_INTEGER);
    asin_come(x, TOKEN_ID);
    code_declInteger(x->code);
    adr_declaracion(x);
  }

  /*
  if (x->token->tipo == TOKEN_STRING) {
  	asin_come(x, TOKEN_STRING);
  }
  else if (x->token->tipo == TOKEN_INTEGER) {
  	asin_come(x, TOKEN_INTEGER);
    }

  asin_come(x, TOKEN_ID);
  code_declString(x->code);
  adr_declaracion(x);
  */
}

void adr_bloque(ASin *x)
{
  if (x->token->tipo == TOKEN_ID ||
      x->token->tipo == TOKEN_PRINT ||
      x->token->tipo == TOKEN_IF ||
      x->token->tipo == TOKEN_WHILE) {

    if (x->token->tipo == TOKEN_ID) {
      adr_asignacion(x);
    }
    else if (x->token->tipo == TOKEN_PRINT) {
      adr_impresion(x);
    }
    else if (x->token->tipo == TOKEN_IF) {
      adr_condicion(x);
    }
    else if (x->token->tipo == TOKEN_WHILE) {
      adr_iteracion(x);
    }

    adr_bloque(x);
  }
}

void adr_asignacion(ASin *x)
{
  asin_come(x, TOKEN_ID);
  code_variable(x->code);
  asin_come(x, TOKEN_ASIGN);
  adr_expresion(x);
  code_asignacion(x->code);
}

void adr_impresion(ASin *x)
{
  asin_come(x, TOKEN_PRINT);
  asin_come(x, TOKEN_IPAR);
  adr_expresion(x);
  asin_come(x, TOKEN_DPAR);
  code_print(x->code);
}

void adr_condicion(ASin *x)
{
  asin_come(x, TOKEN_IF);
  asin_come(x, TOKEN_IPAR);
  adr_expresion(x);
  code_if(x->code);
  asin_come(x, TOKEN_DPAR);
  adr_bloque(x);
  adr_else(x);
  asin_come(x, TOKEN_END);
  code_endIf(x->code);
}

void adr_else(ASin *x)
{
  if (x->token->tipo == TOKEN_ELSE) {
    asin_come(x, TOKEN_ELSE);
    code_else(x->code);
    adr_bloque(x);
  }
}

void adr_iteracion(ASin *x)
{
  asin_come(x, TOKEN_WHILE);
  code_while(x->code);
  asin_come(x, TOKEN_IPAR);
  adr_expresion(x);
  asin_come(x, TOKEN_DPAR);
  code_if(x->code);
  adr_bloque(x);
  asin_come(x, TOKEN_END);
  code_endIf(x->code);
}

void adr_expresion(ASin *x)
{
  adr_vor(x);
  adr_vor1(x);
}

void adr_vor(ASin *x)
{
  adr_vand(x);
  adr_vand1(x);
}

void adr_vor1(ASin *x)
{
  if (x->token->tipo == TOKEN_OR) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_OR);
    adr_vor(x);
    code_operador(x->code, op);
    free(op);
    adr_vor1(x);
  }
}

void adr_vand(ASin *x)
{
  if (x->token->tipo == TOKEN_NEG) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_NEG);
    adr_vrel(x);
    code_operador(x->code, op);
    free(op);
    adr_vrel1(x);
  }
  else {
    adr_vrel(x);
    adr_vrel1(x);
  }
}

void adr_vand1(ASin *x)
{
  if (x->token->tipo == TOKEN_AND) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_AND);
    adr_vand(x);
    code_operador(x->code, op);
    free(op);
    adr_vand1(x);
  }
}

void adr_vrel(ASin *x)
{
  if (x->token->tipo == TOKEN_SUM) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_SUM);
    adr_vsum(x);

    if (strcmp(op, "-") == 0)
      code_operador(x->code, "-1");

    free(op);
    adr_vsum1(x);
  }
  else {
    adr_vsum(x);
    adr_vsum1(x);
  }
}

void adr_vrel1(ASin *x)
{
  if (x->token->tipo == TOKEN_REL) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_REL);
    adr_vrel(x);
    code_operador(x->code, op);
    free(op);
  }
}

void adr_vsum(ASin *x)
{
  adr_vmul(x);
  adr_vmul1(x);
}

void adr_vsum1(ASin *x)
{
  if (x->token->tipo == TOKEN_SUM) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_SUM);
    adr_vsum(x);
    code_operador(x->code, op);
    free(op);
    adr_vsum1(x);
  }
}

void adr_vmul(ASin *x)
{
  if (x->token->tipo == TOKEN_IPAR) {
    asin_come(x, TOKEN_IPAR);
    adr_expresion(x);
    asin_come(x, TOKEN_DPAR);
  }
  else {
    adr_valor(x);
  }
}

void adr_vmul1(ASin *x)
{
  if (x->token->tipo == TOKEN_MUL) {
    char *op = strdup(x->token->nombre);
    asin_come(x, TOKEN_MUL);
    adr_vmul(x);
    code_operador(x->code, op);
    free(op);
    adr_vmul1(x);
  }
}

void adr_valor(ASin *x)
{
  if (x->token->tipo == TOKEN_ID) {
    asin_come(x, TOKEN_ID);
    code_variable(x->code);
  }
  else if (x->token->tipo == TOKEN_NUMERO) {
    asin_come(x, TOKEN_NUMERO);
    code_integer(x->code);
  }
  else if (x->token->tipo == TOKEN_CADENA) {
    asin_come(x, TOKEN_CADENA);
    code_string(x->code);
  }
}