#ifndef _S2_H_
#define _S2_H_

#include <stdbool.h>

// Cadeia de caracteres (s2)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte II: strings constantes e alteráveis (contidas em vetores)


// s2 é o tipo de dados para nossas strings
typedef struct s2 s2;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct s2 {
  int len;   // número de caracteres na cadeia
  int cap;   // tamanho de mem se alterável, 0 se não alterável
  char *mem; // ponteiro para o primeiro caractere da cadeia
};

// As cadeias são em geral acessadas por cópia (a estrutura que representa
//   a cadeia é passada entre funções) e não por ponteiro.
//
// Uma cadeia pode ser alterável ou não.
// - alterável:
//   a estrutura s2 controla a memória em mem, um vetor que foi passado para
//     ela na criação
//   esse vetor não deve ser alterado fora da s2 enquando essa s2 ou alguma
//     outra criada a partir dela existir (e nesse caso não deve ser alterada
//     nem por uma chamada de alteração de s2) -- essa responsabilidade é de
//     quem usa essas strings.
//   pode ser passada por referência para uma função que altera a cadeia
//   tem no campo cap o tamanho do vetor
//   o caractere que segue o último da cadeia deve ser '\0', para manter
//     compatibilidade com strings C. com isso, a maior string possível tem
//     tamanho um a menos que o tamanho do vetor (como strings C normais)
// - não alterável:
//   o campo cap é 0
//   a memória não pertence à string, e não deve ser alterada nem liberada
//   a memória pode pertencer a outra string, ou ser uma constante C ou qualquer
//     memória passada para a função de criação (cujo tamanho é desconhecido)
//   uma string não alterável não deve ser passada por referência para funções
//     que implementam operações que alteram a string
//
// Nas funções abaixo, o argumento `pos` refere-se à posição de um
//   caractere em uma cadeia. Esse argumento deve ser interpretado
//   da seguinte forma:
//   - se ele for 0 representa a posição do primeiro caractere da cadeia;
//     se for 1 a do segundo etc
//   - se ele for -1, representa a posição do último caractere da cadeia;
//     se for -2 a do penúltimo etc


// cria e retorna uma s2 que referencia a string C em str
s2 s2_create(char *str);

// cria e retorna uma s2 que referencia a string C que está no vetor vec
s2 s2_create_vec(int n, char vec[n]);

// macro para criar uma s2 que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define s2_(s) (s2){ .len = sizeof(s) - 1, .cap = 0, .mem = s }

// destrói a s2 str.
// essa cadeia não deve ser utilizada após essa chamada
// essa função não faz nada por enquanto, só existe para criar bons costumes
void s2_destroy(s2 str);

// retorna o tamanho (número de bytes) em str
int s2_len(s2 str);

// retorna um ponteiro para o primeiro caractere de str
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *s2_mem(s2 str);

// retorna o valor do byte na posição pos de str
// retorna 0 se pos for fora dos limites de str
char s2_ch(s2 str, int pos);

// retorna um s2 que é a substring que inicia na posição pos
//   de str e tem len bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de str
// o s2 retornado não é alterável, e só aponta para a memória de str
// o s2 retornado não pode ser usado se str for alterada depois
//   desta chamada
// por exemplo: se str contém "barco":
//   pos=2,  len=3 sub="rco"
//   pos=2,  len=4 sub="rco" (len é ajustado para 3)
//   pos=-2, len=1 sub="c"
//   pos=-6, len=3 sub="ba" (pos é ajustado para 0, len para 2)
//   pos=10, len=5 sub="" (pos é ajustado para o final da string, tam para 0)
s2 s2_sub(s2 str, int pos, int len);

// retorna a primeira posição em str, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int s2_search_c(s2 str, int pos, s2 chs);

// retorna a primeira posição em str, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int s2_search_nc(s2 str, int pos, s2 chs);

// retorna a última posição em str, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int s2_search_rc(s2 str, int pos, s2 chs);

// retorna a última posição em str, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int s2_search_rnc(s2 str, int pos, s2 chs);

// retorna a primeira posição em str, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int s2_search_s(s2 str, int pos, s2 buscada);

// retorna true se str e strb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool s2_equal(s2 str, s2 strb);

// imprime a cadeia em str na saída padrão
void s2_print(s2 str);

// funções novas em s2, operações que alteram a string
// essas funções devem tomar cuidado para não extrapolar a área do vetor

// adiciona ao final da cadeia apontada por pstr o conteúdo da cadeia strb
// não faz nada se a cadeia apontada por pstr não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
// não adiciona mais caracteres do que couberem no vetor de *pstr (com o \0)
//   os que não couberem são ignorados
void s2_cat(s2 *pstr, s2 strb);

// adiciona o caractere c ao final da cadeia apontada por pstr
// não faz nada se a cadeia apontada por pstr não for alterável
// não faz nada se não houver espaço para o caractere no vetor
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
void s2_catc(s2 *pstr, char c);

// insere a cadeia strb na posição pos da cadeia apontada por pstr
// não faz nada se a cadeia apontada por pstr não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
// não adiciona mais caracteres do que couberem no vetor de *pstr (com o \0)
//   os que não couberem são ignorados
// pos pode ser negativo, e se estiver fora da cadeia *pstr, deve ser interpretado
//   como se fosse 0 (se estiver antes do início) ou como len (se após o final)
void s2_insert(s2 *pstr, int pos, s2 strb);

// remove a substring de tamanho len iniciando em pos em *pstr
// não faz nada se a cadeia apontada por pstr não for alterável
// pos e len devem ser interpretados como em s2_sub
void s2_remove(s2 *pstr, int pos, int len);

// completa o final da string em *pstr com caracteres c, de forma que ela passe
//   a ter len caracteres
// não faz nada se a cadeia apontada por pstr não for alterável ou já possuir len
//   caracteres ou mais
// limita o número de caracteres à capacidade do vetor que contém a string
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
void s2_fill(s2 *pstr, int len, char c);


#endif // _S2_H_
