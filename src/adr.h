// ANALIZADOR DESCENDENTE RECURSIVO

#ifndef _ADR_H_
#define	_ADR_H_

#include "asin.h"

void adr_programa(ASin *x);
void adr_declaracion(ASin *x);
void adr_bloque(ASin *x);
void adr_asignacion(ASin *x);
void adr_impresion(ASin *x);
void adr_condicion(ASin *x);
void adr_else(ASin *x);
void adr_iteracion(ASin *x);
void adr_expresion(ASin *x);
void adr_vor(ASin *x);
void adr_vor1(ASin *x);
void adr_vand(ASin *x);
void adr_vand1(ASin *x);
void adr_vrel(ASin *x);
void adr_vrel1(ASin *x);
void adr_vsum(ASin *x);
void adr_vsum1(ASin *x);
void adr_vmul(ASin *x);
void adr_vmul1(ASin *x);
void adr_valor(ASin *x);

#endif // _ADR_H_

