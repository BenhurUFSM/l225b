#ifndef _BB_H_
#define _BB_H_

#include <stdbool.h>

// Cadeia de caracteres (bb)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte I: strings constantes


// bb é o tipo de dados para nossas strings
typedef struct bb bb;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct bb {
  int tam;   // número de caracteres na cadeia
  char *mem; // ponteiro para o primeiro caractere da cadeia
};

// As cadeias são em geral acessadas por cópia (a estrutura que representa
//   a cadeia é passada entre funções) e não por ponteiro.
//
// Nesta parte, as cadeias são constantes, não é possível alterá-las uma vez
//   criadas.
// Uma cadeia não é dona da memória que contém a string, só referencia uma
//   memória que é controlada fora.
//
// Nas funções abaixo, o argumento `pos` refere-se à posição de um
//   caractere em uma cadeia. Esse argumento deve ser interpretado
//   da seguinte forma:
//   - se ele for 0 representa a posição do primeiro caractere da cadeia;
//     se for 1 a do segundo etc
//   - se ele for -1, representa a posição do último caractere da cadeia;
//     se for -2 a do penúltimo etc


// cria e retorna uma bb que referencia a string C em str
bb bb_cria(char *str);

// macro para criar uma bb que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define bb_(s) (bb){.tam=sizeof(s)-1, .mem=s}

// destrói a bb cad.
// essa cadeia não deve ser utilizada após essa chamada
// essa função não faz nada por enquanto, só existe para criar bons costumes
void bb_destroi(bb cad);

// retorna o tamanho (número de bytes) em cad
int bb_tam(bb cad);

// retorna um ponteiro para o primeiro caractere de cad
// esse ponteiro não pode não apontar para uma string compatível com
//   strings padrão C
char *bb_mem(bb cad);

// retorna o valor do byte na posição pos de cad
// retorna 0 se pos for fora dos limites de cad
char bb_ch(bb cad, int pos);

// retorna um bb que é a substring que inicia na posição pos
//   de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o bb retornado não é alterável, e só aponta para a memória de cad
// o bb retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "barco":
//   pos=2, tam=3 sub="rco"
//   pos=2, tam=4 sub="rco" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="ba" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado par o final da string, tam para 0)
bb bb_sub(bb cad, int pos, int tam);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes da primeira, a busca deve iniciar
//   na primeira
int bb_busca_ch(bb cad, int pos, bb chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes da primeira, a busca deve iniciar
//   na primeira
int bb_busca_nch(bb cad, int pos, bb chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int bb_busca_rch(bb cad, int pos, bb chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int bb_busca_rnch(bb cad, int pos, bb chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes da primeira, a busca deve iniciar
//   na primeira
int bb_busca_bb(bb cad, int pos, bb buscada);

// retorna true se cad e outra_cad forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool bb_igual(bb cad, bb outra_cad);

// imprime a cadeia em cad na saída padrão
void bb_imprime(bb cad);

#endif // _BB_H_
