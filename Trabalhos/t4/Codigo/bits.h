#ifndef _BITS_H_
#define _BITS_H_

// bits
//
// TAD que implementa um bitmap, uma matriz de bits
// Essas matrizes são usadas para implementar os desenhos do jogo do dinossauro
// e o desenho da tela desse jogo

#include <stdbool.h>

typedef struct bits *Bits;

// cria uma matriz com tantos bits de altura e largura
Bits b_cria(int largura, int altura);

// cria uma matriz a partir de um arquivo
// a primeira linha do arquivo contém o tamanho da matriz
// as linhas seguintes contêm as linhas da matriz, uma em cada linha do
//   arquivo, com espaço para representar bit 0 e X para bit 1
// se a linha do arquivo for curta, os demais bits da linha da matriz
//   serão 0; se for longa, não vai dar certo
Bits b_cria_arq(char *nome);

// libera a memória ocupada pelo bitmap
void b_destroi(Bits b);

// retorna o número de linhas da matriz
int b_altura(Bits b);

// retorna o número de bits em cada linha da matriz
int b_largura(Bits b);

// coloca todos os bits da matriz em 0
void b_limpa(Bits b);

// coloca em 1 os bits de b que corresponderem ao bits em desenho que estiverem
//   em 1, considerando que o canto superior esquerdo (o bit em 0,0) de desenho
//   seja colocado na posicao x,y de b
void b_desenha(Bits b, int x, int y, Bits desenho);

// retorna true se algum bit estiver em 1 tanto em b quanto em ob, considerando
//   que o canto superior esquerdo (o bit em 0,0) de ob seja colocado na
//   posicao x,y de b
bool b_colide(Bits b, int x, int y, Bits ob);

// retorna true se a posição de algum bit de o for a mesma de algum bit de b,
//   considerando que o canto superior esquerdo (o bit em 0,0) de o seja
//   colocado na posicao x,y de b
bool b_intersecciona(Bits b, int x, int y, Bits o);

// imprime a matriz de bits em b usando 'X' para bits 1 e ' ' para bits 0
void b_imprime_ascii(Bits b);

// imprime a matriz de bits em b usando caracteres unicode que contêm blocos
//   dispostos em uma matriz 2x3
void b_imprime_uni_2x3(Bits b);

#endif // _BITS_H_
