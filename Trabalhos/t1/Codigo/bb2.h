#ifndef _BB2_H_
#define _BB2_H_

#include <stdbool.h>

// Cadeia de caracteres (bb2)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte II: strings constantes e alteráveis (contidas em vetores)


// bb2 é o tipo de dados para nossas strings
typedef struct bb2 bb2;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct bb2 {
  int tam;   // número de caracteres na cadeia
  int cap;   // tamanho de mem se alterável, 0 se não alterável
  char *mem; // ponteiro para o primeiro caractere da cadeia
};

// As cadeias são em geral acessadas por cópia (a estrutura que representa
//   a cadeia é passada entre funções) e não por ponteiro.
//
// Uma cadeia pode ser alterável ou não.
// - alterável:
//   a estrutura bb2 controla a memória em mem, um vetor que foi passado para
//     ela na criação
//   esse vetor não deve ser alterado fora da bb2 enquando essa bb2 ou alguma
//     outra criada a partir dela existir (e nesse caso não deve ser alterada
//     nem por uma chamada de alteração de bb2) -- essa responsabilidade é de
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


// cria e retorna uma cadeia que referencia a string C em str
bb2 bb2_cria(char *str);

// cria e retorna uma cadeia que referencia a string C que está no vetor vec
bb2 bb2_cria_vec(int n, char vec[n]);

// macro para criar uma cadeia que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define bb2_(s) (bb2){ .tam = sizeof(s) - 1, .cap = 0, .mem = s }

// destrói a cadeia cad.
// essa cadeia não deve ser utilizada após essa chamada
// essa função não faz nada por enquanto, só existe para criar bons costumes
void bb2_destroi(bb2 cad);

// retorna o tamanho (número de bytes) de cad
int bb2_tam(bb2 cad);

// retorna um ponteiro para o primeiro caractere de cad
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *bb2_mem(bb2 cad);

// retorna o valor do byte na posição pos de cad
// retorna 0 se pos for fora dos limites de cad
char bb2_ch(bb2 cad, int pos);

// retorna uma cadeia que é a substring que inicia na posição pos
//   de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o bb2 retornado não é alterável, e só aponta para a memória de cad
// o bb2 retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "barco":
//   pos=2,  tam=3 sub="rco"
//   pos=2,  tam=4 sub="rco" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="ba" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado para o final da string, tam para 0)
bb2 bb2_sub(bb2 cad, int pos, int tam);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb2_busca_c(bb2 cad, int pos, bb2 chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb2_busca_nc(bb2 cad, int pos, bb2 chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int bb2_busca_rc(bb2 cad, int pos, bb2 chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int bb2_busca_rnc(bb2 cad, int pos, bb2 chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int bb2_busca_s(bb2 cad, int pos, bb2 buscada);

// retorna true se cad e cadb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool bb2_igual(bb2 cad, bb2 cadb);

// imprime a cadeia em cad na saída padrão
void bb2_imprime(bb2 cad);

// funções novas em bb2, operações que alteram a string
// essas funções devem tomar cuidado para não extrapolar a área do vetor

// adiciona ao final da cadeia apontada por pcad o conteúdo da cadeia cadb
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
// não adiciona mais caracteres do que couberem no vetor de *pcad (com o \0)
//   os que não couberem são ignorados
void bb2_cat(bb2 *pcad, bb2 cadb);

// adiciona o caractere c ao final da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// não faz nada se não houver espaço para o caractere no vetor
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void bb2_catc(bb2 *pcad, char c);

// insere a cadeia cadb na posição pos da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
// não adiciona mais caracteres do que couberem no vetor de *pcad (com o \0)
//   os que não couberem são ignorados
// pos pode ser negativo, e se estiver fora da cadeia *pcad, deve ser interpretado
//   como se fosse 0 (se estiver antes do início) ou como tam (se após o final)
void bb2_insere(bb2 *pcad, int pos, bb2 cadb);

// remove a substring de tamanho tam iniciando em pos em *pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// pos e tam devem ser interpretados como em bb2_sub
void bb2_remove(bb2 *pcad, int pos, int tam);

// completa o final da string em *pcad com caracteres c, de forma que ela passe
//   a ter tam caracteres
// não faz nada se a cadeia apontada por pcad não for alterável ou já possuir tam
//   caracteres ou mais
// limita o número de caracteres à capacidade do vetor que contém a string
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void bb2_preenche(bb2 *pcad, int tam, char c);


#endif // _BB2_H_
