#ifndef _CODIGO_H_
#define	_CODIGO_H_

typedef struct Code Code;

Code *code_new();
void  code_free(Code *x);

void code_setToken(Code *x, Token *token, int iFila, int iColumna);

void code_declInteger(Code *x);
void code_declString(Code *x);
void code_asignacion(Code *x);
void code_print(Code *x);
void code_if(Code *x);
void code_else(Code *x);
void code_while(Code *x);
void code_endIf(Code *x);
void code_variable(Code *x);
void code_integer(Code *x);
void code_string(Code *x);
void code_operador(Code *x, char *operador);

void code_get(Code *x); // muestra el codigo ejecutable
void code_run(Code *x); // ejecuta el programa

#endif	// _CODIGO_H_
