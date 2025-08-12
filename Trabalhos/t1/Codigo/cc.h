#ifndef _CC_H_
#define _CC_H_

#include <stdbool.h>

// Cadeia de caracteres (cc)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte III: strings constantes e alteráveis (alocadas dinamicamente)


// cc é o tipo de dados para nossas strings
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
//   a estrutura cc controla a memória em mem, uma memória que foi alocada
//     pela string, e que deve ser liberada na destruição
//   essa memória não deve ser alterada fora da cc enquando essa cc ou alguma
//     outra criada a partir dela existir (e nesse caso não deve ser alterada
//     nem por uma chamada de alteração de cc) -- essa responsabilidade é de
//     quem usa essas strings.
//   pode ser passada por referência para uma função que altera a cadeia
//   tem no campo cap o tamanho do região de memória alocada
//   o caractere que segue o último da cadeia deve ser '\0', para manter
//     compatibilidade com strings C. com isso, a maior string possível tem
//     tamanho um a menos que o tamanho do região de memória (como strings C normais)
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


// cria e retorna uma cadeia que referencia a string C em cad
cc cc_cria(char *cad);

// cria e retorna uma cadeia alterável que contém uma cópia de cad
// o tamanho da memória alocada deve seguir as regras das operações de alteração
cc cc_copia(cc cad);

// macro para criar uma cadeia que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define cc_(s) (cc){ .tam = sizeof(s) - 1, .cap = 0, .mem = s }

// destrói a cadeia cad.
// essa cadeia não deve ser utilizada após essa chamada
// essa função deve liberar a memória em cadeias alteráveis
void cc_destroi(cc cad);

// retorna o tamanho (número de bytes) de cad
int cc_tam(cc cad);

// retorna um ponteiro para o primeiro caractere de cad
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *cc_mem(cc cad);

// retorna o valor do byte na posição pos de cad
// retorna 0 se pos for fora dos limites de cad
char cc_ch(cc cad, int pos);

// retorna uma cadeia que é a substring que inicia na posição pos
//   de cad e tem tam bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o cc retornado não é alterável, e só aponta para a memória de cad
// o cc retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "barco":
//   pos=2,  tam=3 sub="rco"
//   pos=2,  tam=4 sub="rco" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="ba" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado para o final da string, tam para 0)
cc cc_sub(cc cad, int pos, int tam);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int cc_busca_c(cc cad, int pos, cc chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int cc_busca_nc(cc cad, int pos, cc chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int cc_busca_rc(cc cad, int pos, cc chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int cc_busca_rnc(cc cad, int pos, cc chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int cc_busca_s(cc cad, int pos, cc buscada);

// retorna true se cad e cadb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool cc_igual(cc cad, cc cadb);

// imprime a cadeia em cad na saída padrão
void cc_imprime(cc cad);

// funções novas/alteradas em cc, operações que alteram a string
// essas funções devem realocar a memória da string se necessário, de forma
//   que a quantidade de memória seja sempre superior ao tamanho da string
//   (e sempre seja colocado o '\0' após o final), e nunca inferior a 8,
//   e nem inferior a 1/3 do tamanho da string. o aumento/diminuição do 
//   tamanho da memória deve ser feito sempre com a razão 2 (os tamanhos
//   válidos são 8, 16, 32, 64 etc)

// adiciona ao final da cadeia apontada por pcad o conteúdo da cadeia cadb
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void cc_cat(cc *pcad, cc cadb);

// adiciona o caractere c ao final da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void cc_catc(cc *pcad, char c);

// insere a cadeia cadb na posição pos da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
// pos pode ser negativo, e se estiver fora da cadeia *pcad, deve ser interpretado
//   como se fosse 0 (se estiver antes do início) ou como tam (se após o final)
void cc_insere(cc *pcad, int pos, cc cadb);

// remove a substring de tamanho tam iniciando em pos em *pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// pos e tam devem ser interpretados como em cc_sub
void cc_remove(cc *pcad, int pos, int tam);

// completa o final da string em *pcad com caracteres c, de forma que ela passe
//   a ter tam caracteres
// não faz nada se a cadeia apontada por pcad não for alterável ou já possuir tam
//   caracteres ou mais
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void cc_preenche(cc *pcad, int tam, char c);

// substitui, na cadeia apontada por pcad (que deve ser alterável),
//   a substring que inicia em pos e tem tam bytes pela cadeia em cadb.
// caso pos esteja fora da cadeia pcad, insere caracteres iguais a preenche
//   no início ou no final de pcad, caso necessário, para que a posição 
//   seja válida
// exemplos, suponha que *pcad seja "abacaxi", preenche seja '%':
//   pos=5, tam=2, cadb="te", *pcad deve ser transformado em "abacate"
//   7,0,"." -> "abacaxi."
//   9,0,"." -> "abacaxi%%."
//   9,0,"" -> "abacaxi%%"
//   9,10,"." -> "abacaxi%%."
//   -3,0,"123" -> "abac123axi"
//   0,1,"123" -> "123bacaxi"
//   -9,1,"123" -> "123%abacaxi"
//   3,200,"" -> "ab"
void cc_subst(cc *pcad, int pos, int tam, cc cadb, char preenche);


// retorna uma cópia compatível com string C da string em cad, em uma nova
// memória alocada com malloc de um byte a mais que o tamanho da string.
// é responsabilidade de quem chama esta função liberar essa memória.
char *cc_strc(cc cad);

// cria uma cadeia alterável com o conteúdo do arquivo chamado nome
// retorna uma cadeia vazia (não alterável) em caso de erro
cc cc_le_arquivo(cc nome);

// grava o conteúdo de cad em um arquivo chamado nome
void cc_grava_arquivo(cc cad, cc nome);


#endif // _CC_H_
