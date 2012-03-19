#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "token.h"
#include "code.h"

#define NUM_VARIABLES 16
#define TAMANYO_PILA  128
#define TAMANYO_CTRL  16

typedef enum {
  VOID, INTEGER, STRING, BOOLEAN
} TipoValor;

typedef enum {
  DECLARACION, ASIGNACION, IMPRIMIR,
  IF, ELSE, WHILE, GOTO, END,
  VARIABLE, CONSTANTE,
  SUMAR, RESTAR, NEGAR,
  MULTIPLICAR, DIVIDIR,
  IGUAL, DISTINTO,
  MENOR, MENORIGUAL,
  MAYOR, MAYORIGUAL,
  NOT, OR, AND, NUM_SENTENCIAS
} Sentencia;

typedef struct {
  char *nombre;
  TipoValor tipo;
  void *valor;
} Variable;

typedef struct Nodo Nodo;

struct Nodo {
  int linea, iFila, iColumna;
  Sentencia sentencia;
  TipoValor tipo;
  void *valor;
  char *variable;
  Nodo *next;
};

Nodo *(*runPtr[NUM_SENTENCIAS])(Code *x, Nodo *n);

typedef struct {
  TipoValor tipo;
  void *valor;
  char *variable;
  int  liberar;
} Pila;

struct Code {
  // Datos de compilacion
  Token *token;
  int iFila, iColumna;

  // Datos de compilacion y ejecucion
  Variable **vars;
  Nodo *primero, *ultimo;
  int numNodos;

  // Pila para implementar los saltos en condiciones y bucles
  Nodo *control[TAMANYO_CTRL];
  int cimaCtrl;

  // Pila de ejecucion
  Pila pila[TAMANYO_PILA];
  int cimaPila;
};

Code *code_new()
{
  Code *code = (Code *)malloc(sizeof(Code));
  code->token = (Token *)malloc(sizeof(Token));
  code->vars = (Variable **)malloc(sizeof(Variable *) * NUM_VARIABLES);
  code->primero = code->ultimo = NULL;
  code->numNodos = 0;
  code->cimaCtrl = 0;
  code->cimaPila = 0;

  int i;

  for (i = 0; i < TAMANYO_CTRL; i++)
    code->control[i] = NULL;

  for (i = 0; i < NUM_VARIABLES; i++)
    code->vars[i] = NULL;

  return code;
}

void code_free(Code *x)
{
  Nodo *nodo = x->primero;

  while (nodo) {
    if (nodo->sentencia == CONSTANTE && nodo->tipo == STRING)
      free(nodo->valor);

    free(nodo);
    nodo = nodo->next;
  }

  int i;

  for (i = 0; i < NUM_VARIABLES; i++) {
    Variable *v = x->vars[i];

    if (v) {
      free(v->nombre);
      free(v->valor);
      free(v);
    }
  }

  free(x->token);
  free(x);
}

void code_setToken(Code *x, Token *token, int iFila, int iColumna)
{
  token_copy(x->token, token);
  x->iFila    = iFila;
  x->iColumna = iColumna;
}

void code_addNodo(Code *x, Nodo *nodo)
{
  nodo->linea    = x->numNodos;
  nodo->iFila    = x->iFila;
  nodo->iColumna = x->iColumna;
  x->numNodos++;

  if (x->primero == NULL) {
    assert(x->ultimo == NULL);
    x->primero = x->ultimo = nodo;
  }
  else {
    assert(x->ultimo->next == NULL);
    x->ultimo->next = nodo;
    x->ultimo = nodo;
  }
}

Nodo *code_newNodo(Sentencia sentencia, TipoValor tipo,
                   void *valor, char *variable)
{
  Nodo *nodo = (Nodo *)malloc(sizeof(Nodo));
  nodo->sentencia = sentencia;
  nodo->tipo      = tipo;
  nodo->valor     = valor;
  nodo->variable  = variable;
  nodo->next      = NULL;
  return nodo;
}

void code_error(Code *x, char *mensaje)
{
  printf("Error (%d:%d): %s, message: %s\n",
         x->iFila, x->iColumna,
         x->token->nombre, mensaje);
  exit(0);
}

int code_buscarVariable(Code *x)
{
  int i;

  for (i = 0; i < NUM_VARIABLES; i++) {
    Variable *v = x->vars[i];

    if (v == NULL)
      return i; //............................................RETURN

    if (strcmp(v->nombre, x->token->nombre) == 0)
      return i; //............................................RETURN
  }

  return -1;
}

void code_declVariable(Code *x, TipoValor tipo)
{
  int i = code_buscarVariable(x);

  if (i == -1)
    code_error(x, "Too many variables defined. ");

  if (x->vars[i] != NULL)
    code_error(x, "Already existing variable. ");

  Variable *v = x->vars[i] = (Variable *)malloc(sizeof(Variable));
  v->nombre = strdup(x->token->nombre);
  v->tipo   = tipo;
  v->valor  = malloc(sizeof(void *));

  if (tipo == STRING)
    *((char **)v->valor) = strdup("");
  else
    *((int *)v->valor) = 0;

  Nodo *nodo = code_newNodo(DECLARACION, tipo, &(v->valor), v->nombre);
  code_addNodo(x, nodo);
}

void code_declInteger(Code *x)
{
  code_declVariable(x, INTEGER);
}

void code_declString(Code *x)
{
  code_declVariable(x, STRING);
}

void code_asignacion(Code *x)
{
  Nodo *nodo = code_newNodo(ASIGNACION, VOID, NULL, NULL);
  code_addNodo(x, nodo);
}

void code_print(Code *x)
{
  Nodo *nodo = code_newNodo(IMPRIMIR, VOID, NULL, NULL);
  code_addNodo(x, nodo);
}

void code_pushCtrl(Code *x, Nodo *n)
{
  if (x->cimaCtrl == TAMANYO_CTRL) {
    printf("The stack is full. \n");
    exit(1);
  }

  x->control[x->cimaCtrl] = n;
  x->cimaCtrl++;
}

Nodo *code_popCtrl(Code *x)
{
  if (x->cimaCtrl == 0) {
    printf("The stack is empty. \n");
    exit(1);
  }

  x->cimaCtrl--;
  return x->control[x->cimaCtrl];
}

void code_if(Code *x)
{
  Nodo *nodo = code_newNodo(IF, VOID, NULL, NULL);
  code_addNodo(x, nodo);
  code_pushCtrl(x, nodo);
}

void code_else(Code *x)
{
  Nodo *gotoEnd = code_newNodo(GOTO, VOID, NULL, NULL);
  code_addNodo(x, gotoEnd);

  Nodo *nodoElse = code_newNodo(ELSE, VOID, NULL, NULL);
  code_addNodo(x, nodoElse);

  // GOTO al else: cuando no se cumpla la condicion.
  code_popCtrl(x)->valor = nodoElse;

  code_pushCtrl(x, gotoEnd);
}

void code_while(Code *x)
{
  Nodo *nodo = code_newNodo(WHILE, VOID, NULL, NULL);
  code_addNodo(x, nodo);
  code_pushCtrl(x, nodo);
}

void code_endIf(Code *x)
{
  int bucle = x->cimaCtrl >= 2 &&
              x->control[x->cimaCtrl - 2]->sentencia == WHILE;

  Nodo *gotoInicio = NULL;

  if (bucle) {
    gotoInicio = code_newNodo(GOTO, VOID, NULL, NULL);
    code_addNodo(x, gotoInicio);
  }

  Nodo *fin = code_newNodo(END, VOID, NULL, NULL);
  code_addNodo(x, fin);

  // GOTO al final del bloque: cuando no se cumpla la condicion.
  code_popCtrl(x)->valor = fin;

  if (bucle) {
    // GOTO al inicio del bucle
    gotoInicio->valor = code_popCtrl(x);
  }
}

void code_variable(Code *x)
{
  int i = code_buscarVariable(x);
  Variable *v;

  if (i == -1 || !(v = x->vars[i]))
    code_error(x, "Variable doesn't exists. ");

  Nodo *nodo = code_newNodo(VARIABLE, v->tipo, v->valor, v->nombre);
  code_addNodo(x, nodo);
}

void code_integer(Code *x)
{
  if (strlen(x->token->nombre) > 10)
    code_error(x, "Number too big. ");

  char *tail;
  long n = strtol(x->token->nombre, &tail, 0);

  if (n < INT_MIN || n > INT_MAX || tail[0] != '\0')
    code_error(x, "Number too big. ");

  Nodo *nodo = code_newNodo(CONSTANTE, INTEGER, (void *)n, NULL);
  code_addNodo(x, nodo);
}

void code_string(Code *x)
{
  char *valor = strdup(x->token->nombre);
  Nodo *nodo = code_newNodo(CONSTANTE, STRING, valor, NULL);
  code_addNodo(x, nodo);
}

void code_operador(Code *x, char *operador)
{
  Sentencia sentencia;

  if (strcmp(operador, "+") == 0)
    sentencia = SUMAR;
  else if (strcmp(operador, "-") == 0)
    sentencia = RESTAR;
  else if (strcmp(operador, "-1") == 0)
    sentencia = NEGAR;
  else if (strcmp(operador, "*") == 0)
    sentencia = MULTIPLICAR;
  else if (strcmp(operador, "/") == 0)
    sentencia = DIVIDIR;
  else if (strcmp(operador, "=") == 0)
    sentencia = IGUAL;
  else if (strcmp(operador, "!=") == 0)
    sentencia = DISTINTO;
  else if (strcmp(operador, "<") == 0)
    sentencia = MENOR;
  else if (strcmp(operador, "<=") == 0)
    sentencia = MENORIGUAL;
  else if (strcmp(operador, ">") == 0)
    sentencia = MAYOR;
  else if (strcmp(operador, ">=") == 0)
    sentencia = MAYORIGUAL;
  else if (strcmp(operador, "!") == 0)
    sentencia = NOT;
  else if (strcmp(operador, "||") == 0)
    sentencia = OR;
  else if (strcmp(operador, "&&") == 0)
    sentencia = AND;
  else
    assert(0);

  Nodo *nodo = code_newNodo(sentencia, VOID, NULL, NULL);
  code_addNodo(x, nodo);
}

void code_get(Code *x)
{
  Nodo *n = x->primero;

  while (n != NULL) {
    printf("%5d:  ", n->linea);

    switch (n->tipo) {
      case VOID:
        printf("     ");
        break;

      case INTEGER:
        printf("int  ");
        break;

      case STRING:
        printf("str  ");
        break;

      case BOOLEAN:
        printf("bool ");
        break;

      default:
        assert(0);
    }

    switch (n->sentencia) {
      case DECLARACION:
        printf("decl  ");
        break;

      case ASIGNACION:
        printf(":=    ");
        break;

      case IMPRIMIR:
        printf("print ");
        break;

      case IF:
        printf("if    ");
        break;

      case ELSE:
        printf("else  ");
        break;

      case WHILE:
        printf("while ");
        break;

      case GOTO:
        printf("goto  ");
        break;

      case END:
        printf("end   ");
        break;

      case VARIABLE:
        printf("var   ");
        break;

      case CONSTANTE:
        printf("const ");
        break;

      case SUMAR:
        printf("+     ");
        break;

      case RESTAR:
        printf("-     ");
        break;

      case NEGAR:
        printf("-1    ");
        break;

      case MULTIPLICAR:
        printf("*     ");
        break;

      case DIVIDIR:
        printf("/     ");
        break;

      case IGUAL:
        printf("=     ");
        break;

      case DISTINTO:
        printf("!=    ");
        break;

      case MENOR:
        printf("<     ");
        break;

      case MENORIGUAL:
        printf("<=    ");
        break;

      case MAYOR:
        printf(">     ");
        break;

      case MAYORIGUAL:
        printf("<=    ");
        break;

      case NOT:
        printf("!     ");
        break;

      case OR:
        printf("||    ");
        break;

      case AND:
        printf("&&    ");
        break;

      default:
        assert(0);
    }

    if (n->sentencia == DECLARACION || n->variable != NULL) {
      printf("%s", (char *)n->variable);
    }
    else if (n->valor != NULL || n->tipo == INTEGER) {
      if (n->sentencia == GOTO || n->sentencia == IF)
        printf("%d", ((Nodo *)n->valor)->linea);
      else if (n->tipo == STRING)
        printf("\"%s\"", (char *)n->valor);
      else if (n->tipo == INTEGER)
        printf("%ld", (long)n->valor);
    }

    printf("\n");
    n = n->next;
  }
}

void code_push(Code *x, TipoValor tipo, void *valor,
               char *variable, int liberar)
{
  if (x->cimaPila == TAMANYO_PILA) {
    printf("The stack is full. \n");
    exit(1);
  }

  Pila *p = &x->pila[x->cimaPila];
  p->tipo     = tipo;
  p->valor    = valor;
  p->variable = variable;
  p->liberar  = liberar;
  x->cimaPila++;
}

void code_pop(Code *x, TipoValor *tipo, void **valor,
              int variable, int *liberar)
{
  if (x->cimaPila == 0) {
    printf("The stack is empty. \n");
    exit(1);
  }

  x->cimaPila--;
  Pila *p  = &x->pila[x->cimaPila];
  *tipo    = p->tipo;
  *liberar = p->tipo == STRING && !p->variable && p->liberar;
  *valor   = p->variable && !variable ? *(void **)(p->valor) : p->valor;
}

void code_comprobarTipos(Nodo *n, TipoValor t1, TipoValor t2)
{
  if (t1 != t2) {
    printf("Incompatible types in %d, %d. \n", n->iFila, n->iColumna);
    exit(1);
  }
}

char *code_toString(TipoValor t, void *v, char *s)
{
  if (t == STRING) {
    return (char *)v;
  }
  else if (t == INTEGER) {
    sprintf(s, "%ld", (long)v);
    return s;
  }
  else if (t == BOOLEAN) {
    sprintf(s, "%s", (v ? "true" : "false"));
    return s;
  }
  else {
    assert(0);
  }
}

Nodo *code_runNext(Code *x, Nodo *n)
{
  return n->next;
}

Nodo *code_runAsignacion(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f1, f2;
  code_pop(x, &t2, &v2, 0, &f2);
  code_pop(x, &t1, &v1, 1, &f1);
  code_comprobarTipos(n, t1, t2);

  if (t1 == STRING) {
    // Siempre liberamos los valores anteriores de las cadenas.
    if (*(void **)v1 != NULL)
      free(*(void **)v1);

    // Si v2 se tiene que liberar, no lo duplicamos.
    *(void **)v1 = f2 ? v2 : strdup((char *)v2);
  }
  else {
    *(void **)v1 = v2;
  }

  return n->next;
}

Nodo *code_runImprimir(Code *x, Nodo *n)
{
  TipoValor t;
  void *v;
  int f;
  code_pop(x, &t, &v, 0, &f);

  if (t == INTEGER) {
    printf("%ld\n", (long)v);
  }
  else if (t == STRING) {
    printf("%s\n", (char *)v);

    if (f)
      free(v);
  }
  else if (t == BOOLEAN) {
    printf(v ? "true" : "false");
  }
  else {
    assert(0);
  }

  return n->next;
}

Nodo *code_runIf(Code *x, Nodo *n)
{
  TipoValor t;
  void *v;
  int f;
  code_pop(x, &t, &v, 0, &f);
  code_comprobarTipos(n, t, BOOLEAN);
  return v ? n->next : (Nodo *)n->valor;
}

Nodo *code_runGoto(Code *x, Nodo *n)
{
  return (Nodo *)(n->valor);
}

Nodo *code_runVariable(Code *x, Nodo *n)
{
  code_push(x, n->tipo, n->valor, n->variable, 0);
  return n->next;
}

Nodo *code_runConstante(Code *x, Nodo *n)
{
  code_push(x, n->tipo, n->valor, NULL, 0);
  return n->next;
}

Nodo *code_runSumar(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f1, f2;
  code_pop(x, &t2, &v2, 0, &f2);
  code_pop(x, &t1, &v1, 0, &f1);

  if (t1 == STRING || t2 == STRING) {
    char s[20], *s1, *s2, *sT;
    s1 = code_toString(t1, v1, s);
    s2 = code_toString(t2, v2, s);
    sT = (char *)malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));

    strcpy(sT, s1);
    strcat(sT, s2);
    code_push(x, STRING, (void *)(sT), NULL, 1);

    if (f1)
      free(v1);

    if (f2)
      free(v2);
  }
  else {
    code_comprobarTipos(n, t1, INTEGER);
    code_comprobarTipos(n, t2, INTEGER);
    code_push(x, INTEGER, (void *)((long)v1 + (long)v2), NULL, 0);
  }

  return n->next;
}

Nodo *code_runRestar(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, INTEGER, (void *)((long)v1 - (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runNegar(Code *x, Nodo *n)
{
  TipoValor t;
  void *v;
  int f;
  code_pop(x, &t, &v, 0, &f);
  code_comprobarTipos(n, t, INTEGER);
  code_push(x, INTEGER, (void *)(-(long)v), NULL, 0);
  return n->next;
}

Nodo *code_runMultiplicar(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, INTEGER, (void *)((long)v1 * (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runDividir(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, INTEGER, (void *)((long)v1 / (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runIgual(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, BOOLEAN, (void *)(long)(v1 == v2), NULL, 0);
  return n->next;
}

Nodo *code_runDistinto(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, BOOLEAN, (void *)(long)(v1 != v2), NULL, 0);
  return n->next;
}

Nodo *code_runMenor(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, BOOLEAN, (void *)(long)((long)v1 < (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runMenorIgual(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, BOOLEAN, (void *)(long)((long)v1 <= (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runMayor(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, t1, INTEGER);
  code_comprobarTipos(n, t2, INTEGER);
  code_push(x, BOOLEAN, (void *)(long)((long)v1 > (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runMayorIgual(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, INTEGER, t1);
  code_comprobarTipos(n, INTEGER, t2);
  code_push(x, BOOLEAN, (void *)(long)((long)v1 >= (long)v2), NULL, 0);
  return n->next;
}

Nodo *code_runNot(Code *x, Nodo *n)
{
  TipoValor t;
  void *v;
  int f;
  code_pop(x, &t, &v, 0, &f);
  code_comprobarTipos(n, BOOLEAN, t);
  code_push(x, BOOLEAN, (void *)(long)(!v), NULL, 0);
  return n->next;
}

Nodo *code_runOr(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, BOOLEAN, t1);
  code_comprobarTipos(n, BOOLEAN, t2);
  code_push(x, BOOLEAN, (void *)(long)(v1 || v2), NULL, 0);
  return n->next;
}

Nodo *code_runAnd(Code *x, Nodo *n)
{
  TipoValor t1, t2;
  void *v1, *v2;
  int f;
  code_pop(x, &t2, &v2, 0, &f);
  code_pop(x, &t1, &v1, 0, &f);
  code_comprobarTipos(n, BOOLEAN, t1);
  code_comprobarTipos(n, BOOLEAN, t2);
  code_push(x, BOOLEAN, (void *)(long)(v1 && v2), NULL, 0);
  return n->next;
}

void inicializarRunPtr()
{
  runPtr[DECLARACION] = code_runNext;
  runPtr[ASIGNACION ] = code_runAsignacion;
  runPtr[IMPRIMIR   ] = code_runImprimir;
  runPtr[IF         ] = code_runIf;
  runPtr[ELSE       ] = code_runNext;
  runPtr[WHILE      ] = code_runNext;
  runPtr[GOTO       ] = code_runGoto;
  runPtr[END        ] = code_runNext;
  runPtr[VARIABLE   ] = code_runVariable;
  runPtr[CONSTANTE  ] = code_runConstante;
  runPtr[SUMAR      ] = code_runSumar;
  runPtr[RESTAR     ] = code_runRestar;
  runPtr[NEGAR      ] = code_runNegar;
  runPtr[MULTIPLICAR] = code_runMultiplicar;
  runPtr[DIVIDIR    ] = code_runDividir;
  runPtr[IGUAL      ] = code_runIgual;
  runPtr[DISTINTO   ] = code_runDistinto;
  runPtr[MENOR      ] = code_runMenor;
  runPtr[MENORIGUAL ] = code_runMenorIgual;
  runPtr[MAYOR      ] = code_runMayor;
  runPtr[MAYORIGUAL ] = code_runMayorIgual;
  runPtr[NOT        ] = code_runNot;
  runPtr[OR         ] = code_runOr;
  runPtr[AND        ] = code_runAnd;
}

void code_run(Code *x)
{
  inicializarRunPtr();
  Nodo *n = x->primero;

  while (n != NULL) {
    assert(n->sentencia < NUM_SENTENCIAS);
    n = (runPtr[n->sentencia])(x, n);
  }
}
