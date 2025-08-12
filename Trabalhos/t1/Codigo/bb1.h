#ifndef _BB1_H_
#define _BB1_H_

#include <stdbool.h>

// Cadeia de caracteres (bb1)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte I: strings constantes


// bb1 é o tipo de dados para nossas strings
typedef struct bb1 bb1;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct bb1 {
  int tam;   // número de caracteres na cadeia
  char *mem; // ponteiro para o primeiro caractere da cadeia
};

// As cadeias são em geral acessadas por cópia (a estrutura que representa
//   a cadeia é passada entre funções) e não por ponteiro.
//
// Como regra geral, não se sabe o que tem antes ou depois dos caracteres
//   que estão na cadeia, nem se são acessáveis
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


// cria e retorna uma cadeia que referencia a string C em str
bb1 bb1_cria(char *str);

// macro para criar uma cadeia que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define bb1_(s) (bb1){ .tam = sizeof(s) - 1, .mem = s }

// destrói a cadeia cad.
// essa cadeia não deve ser utilizada após essa chamada
// essa função não faz nada por enquanto, só existe para criar bons costumes
void bb1_destroi(bb1 cad);

// retorna o tamanho (número de bytes) de cad
int bb1_tam(bb1 cad);

// retorna um ponteiro para o primeiro caractere de cad
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *bb1_mem(bb1 cad);

// retorna o valor do byte na posição pos de cad
// retorna 0 se pos for fora dos limites de cad
char bb1_ch(bb1 cad, int pos);

// retorna uma cadeia que é a substring que inicia na posição pos
//   de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o bb1 retornado não é alterável, e só aponta para a memória de cad
// o bb1 retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "barco":
//   pos=2,  tam=3 sub="rco"
//   pos=2,  tam=4 sub="rco" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="ba" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado para o final da string, tam para 0)
bb1 bb1_sub(bb1 cad, int pos, int tam);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb1_busca_c(bb1 cad, int pos, bb1 chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb1_busca_nc(bb1 cad, int pos, bb1 chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int bb1_busca_rc(bb1 cad, int pos, bb1 chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int bb1_busca_rnc(bb1 cad, int pos, bb1 chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb1_busca_s(bb1 cad, int pos, bb1 buscada);

// retorna true se cad e cadb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool bb1_igual(bb1 cad, bb1 cadb);

// imprime a cadeia em cad na saída padrão
void bb1_imprime(bb1 cad);

#endif // _BB1_H_
