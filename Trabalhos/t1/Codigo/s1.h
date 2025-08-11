#ifndef _S1_H_
#define _S1_H_

#include <stdbool.h>

// Cadeia de caracteres (s1)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte I: strings constantes


// s1 é o tipo de dados para nossas strings
typedef struct s1 s1;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct s1 {
  int len;   // número de caracteres na cadeia
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


// cria e retorna uma s1 que referencia a string C em str
s1 s1_create(char *str);

// macro para criar uma s1 que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define s1_(s) (s1){ .len = sizeof(s) - 1, .mem = s }

// destrói a s1 str.
// essa cadeia não deve ser utilizada após essa chamada
// essa função não faz nada por enquanto, só existe para criar bons costumes
void s1_destroy(s1 str);

// retorna o tamanho (número de bytes) em str
int s1_len(s1 str);

// retorna um ponteiro para o primeiro caractere de str
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *s1_mem(s1 str);

// retorna o valor do byte na posição pos de str
// retorna 0 se pos for fora dos limites de str
char s1_ch(s1 str, int pos);

// retorna um s1 que é a substring que inicia na posição pos
//   de str e tem len bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de str
// o s1 retornado não é alterável, e só aponta para a memória de str
// o s1 retornado não pode ser usado se str for alterada depois
//   desta chamada
// por exemplo: se str contém "barco":
//   pos=2,  len=3 sub="rco"
//   pos=2,  len=4 sub="rco" (len é ajustado para 3)
//   pos=-2, len=1 sub="c"
//   pos=-6, len=3 sub="ba" (pos é ajustado para 0, len para 2)
//   pos=10, len=5 sub="" (pos é ajustado para o final da string, tam para 0)
s1 s1_sub(s1 str, int pos, int len);

// retorna a primeira posição em str, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int s1_search_c(s1 str, int pos, s1 chs);

// retorna a primeira posição em str, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int s1_search_nc(s1 str, int pos, s1 chs);

// retorna a última posição em str, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int s1_search_rc(s1 str, int pos, s1 chs);

// retorna a última posição em str, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int s1_search_rnc(s1 str, int pos, s1 chs);

// retorna a primeira posição em str, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int s1_search_s(s1 str, int pos, s1 buscada);

// retorna true se str e strb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool s1_equal(s1 str, s1 strb);

// imprime a cadeia em str na saída padrão
void s1_print(s1 str);

#endif // _S1_H_
