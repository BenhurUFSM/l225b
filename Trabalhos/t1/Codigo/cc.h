#ifndef _CC_H_
#define _CC_H_

#include <stdbool.h>

// Cadeia de caracteres (cc)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.

typedef struct cc cc;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct cc {
  int tam;   // número de caracteres na cadeia
  int cap;   // tamanho de mem se alterável, 0 se não alterável
  char *mem; // ponteiro para o primeiro caractere da cadeia
};

// As cadeias são em geral acessadas por cópia (a estrutura que representa
//   a cadeia é passada entre funções) e não por ponteiro.
//
// Uma cadeia pode ser alterável ou não.
// - alterável:
//   a estrutura cc é "dona" da memória em mem, que foi alocada em alguma
//     das funções abaixo
//   a memória deve ser liberada quando a cadeia for destruída
//   pode ser passada por referência para uma função que altera a cadeia
//   tem no campo cap o tamanho da memória alocada
//   cap não pode ser inferior a um tamanho mínimo, e deve ser maior que tam
//   o caractere que segue o último da cadeia deve ser '\0', para manter
//     compatibilidade com strings C
// - não alterável:
//   o campo cap é 0
//   a memória não pertence à cc, e não deve ser alterada nem liberada
//   a memória pode pertencer a outra cc, ou ser uma constante C ou uma
//     variável C ou uma região alocada com malloc fora de cc
//   não se sabe o que tem antes ou depois dos caracteres que estão na cc,
//     nem se são acessáveis
//
// Nas funções abaixo, o argumento `pos` refere-se à posição de um
//   caractere em uma cadeia. Esse argumento deve ser interpretado
//   da seguinte forma:
//   - se ele for 0 representa a posição do primeiro caractere da cadeia;
//     se for 1 a do segundo etc
//   - se ele for -1, representa a posição do último caractere da cadeia;
//     se for -2 a do penúltimo etc
//
// As cadeias recebidas como argumento podem ser alteráveis ou não, e não
//   devem ser alteradas. As recebidas por referência devem ser alteráveis
//   e podem ser alteradas.


// cria e retorna uma cc que contém uma cópia da string C em str
// a cc retornada é alterável
cc cc_cria(char *str);

// macro para criar uma cc que contém a string C constante em s (não uma cópia)
// a cc retornada não é alterável
// só pode ser usada com strings constantes (entre aspas)
#define cc_(s) (cc){.tam=sizeof(s)-1, .cap=0, .mem=s}

// destrói a cc cad.
// se for alterável, libera a memória; se não, não faz nada
void cc_destroi(cc cad);

// retorna o tamanho (número de bytes) em cad
int cc_tam(cc cad);

// retorna um ponteiro para o primeiro caractere de cad
// se cad for alterável, esse ponteiro pode ser usado como string C
//   porque cc alterável sempre deve ter um `\0` além da string
// o conteúdo apontado por esse ponteiro não deve ser alterado
//   (a não ser que tenha **muita** certeza do que está fazendo)
char *cc_mem(cc cad);

// retorna o valor do byte na posição pos de cad
// retorna 0 se pos for fora dos limites de cad
char cc_ch(cc cad, int pos);

// retorna um cc que é a substring que inicia na posição pos
//   de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o cc retornado não é alterável, e só aponta para a memória de cad
// o cc retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "barco":
//   pos=2, tam=3 sub="rco"
//   pos=2, tam=4 sub="rco" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="ba" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado par o final da string, tam para 0)
cc cc_sub(cc cad, int pos, int tam);

// aloca memória, copia o conteúdo de cad para essa memória, coloca um
//   terminador `\0` para ser uma string C e retorna o ponteiro alocado
// essa memória pode ser usada pelo chamador como bem entender; o chamador
//   é responsável por liberar essa memória com `free`
char *cc_strc(cc cad);

// cria uma cc que é uma cópia alterável de cad
cc cc_copia(cc cad);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes da primeira, a busca deve iniciar
//   na primeira
int cc_busca_ch(cc cad, int pos, cc chs);
// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes da primeira, a busca deve iniciar
//   na primeira
int cc_busca_nch(cc cad, int pos, cc chs);
// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int cc_busca_rch(cc cad, int pos, cc chs);
// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int cc_busca_rnch(cc cad, int pos, cc chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes da primeira, a busca deve iniciar
//   na primeira
int cc_busca_cc(cc cad, int pos, cc buscada);

// retorna true se cad e outra_cad forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool cc_igual(cc cad, cc outra_cad);


// substitui, na cadeia apontada por pcad (que deve ser alterável),
//   a substring que inicia em pos e tem tam bytes pela cadeia em cad.
// caso pos esteja fora da cadeia pcad, insere caracteres iguais a preenche
//   no início ou no final de pcad, caso necessário, para que a posição 
//   seja válida
// exemplos, suponha que *pcad seja "abacaxi", preenche seja '%':
//   pos=5, tam=2, cad="te", *pcad deve ser transformado em "abacate"
//   7,0,"." -> "abacaxi."
//   9,0,"." -> "abacaxi%%."
//   9,0,"" -> "abacaxi%%"
//   9,10,"." -> "abacaxi%%."
//   -3,0,"123" -> "abac123axi"
//   0,1,"123" -> "123bacaxi"
//   -9,1,"123" -> "123%abacaxi"
//   3,200,"" -> "ab"
void cc_subst(cc *pcad, int pos, int tam, cc cad, char preenche);

// imprime a cadeia em cad na saída padrão
void cc_print(cc cad);

// cria uma cadeia alterável com o conteúdo do arquivo chamado nome
// retorna uma cadeia vazia em caso de erro
cc cc_le_arq(cc nome);

// grava o conteúdo de cad em um arquivo chamado nome
void cc_grava_arq(cc cad, cc nome);

#endif // _CC_H_
