// AUTOMATA FINITO DETERMINISTA

#ifndef _AFD_H_
#define	_AFD_H_

#include "alex.h"

int ispermitedchar(int c);

int afd_inicio(ALex *x);
int afd_numero(ALex *x);
int afd_cadena(ALex *x);
int afd_cadena_fin(ALex *x);
int afd_mul(ALex *x);
int afd_mul_comment(ALex *x);
int afd_id(ALex *x);
int afd_asign(ALex *x);
int afd_asign_fin(ALex *x);
int afd_sum(ALex *x);
int afd_neg(ALex *x);
int afd_or(ALex *x);
int afd_or_fin(ALex *x);
int afd_and(ALex *x);
int afd_and_fin(ALex *x);
int afd_ipar(ALex *x);
int afd_dpar(ALex *x);
int afd_rel(ALex *x);
int afd_rel_fin(ALex *x);
int afd_comentario_linea(ALex *x);
int afd_comentario_bloque(ALex *x);
int afd_comentario_bloque_fin(ALex *x);

int afd_e(ALex *x);
int afd_el(ALex *x);
int afd_els(ALex *x);
int afd_else(ALex *x);
int afd_en(ALex *x);
int afd_end(ALex *x);
int afd_i(ALex *x);
int afd_if(ALex *x);
int afd_in(ALex *x);
int afd_int(ALex *x);
int afd_inte(ALex *x);
int afd_integ(ALex *x);
int afd_intege(ALex *x);
int afd_integer(ALex *x);
int afd_w(ALex *x);
int afd_wh(ALex *x);
int afd_whi(ALex *x);
int afd_whil(ALex *x);
int afd_while(ALex *x);
int afd_s(ALex *x);
int afd_st(ALex *x);
int afd_str(ALex *x);
int afd_stri(ALex *x);
int afd_strin(ALex *x);
int afd_string(ALex *x);
int afd_p(ALex *x);
int afd_pr(ALex *x);
int afd_pri(ALex *x);
int afd_prin(ALex *x);
int afd_print(ALex *x);

#endif // _AFD_H_
