#ifndef _BITS_H_
#define _BITS_H_

#include <stdbool.h>

typedef struct bits *Bits;

Bits b_cria(int largura, int altura);

Bits b_cria_arq(char *nome);

void b_destroi(Bits b);

void b_limpa(Bits b);

void b_desenha(Bits b, int x, int y, Bits desenho);

bool b_colide(Bits b, int x, int y, Bits ob);

void b_imprime_ascii(Bits b);
void b_imprime_uni_2x3(Bits b);

#endif // _BITS_H_
