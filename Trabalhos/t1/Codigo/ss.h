#ifndef _S_H_
#define _S_H_

#include <stdbool.h>

// Cadeia de caracteres (ss)
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte III: strings constantes e alteráveis (alocadas dinamicamente)


// ss é o tipo de dados para nossas strings
typedef struct ss ss;

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct ss {
  int len;   // número de caracteres na cadeia
  int cap;   // tamanho de mem se alterável, 0 se não alterável
  char *mem; // ponteiro para o primeiro caractere da cadeia
};

// As cadeias são em geral acessadas por cópia (a estrutura que representa
//   a cadeia é passada entre funções) e não por ponteiro.
//
// Uma cadeia pode ser alterável ou não.
// - alterável:
//   a estrutura ss controla a memória em mem, uma memória que foi alocada
//     pela string, e que deve ser liberada na destruição
//   essa memória não deve ser alterada fora da ss enquando essa ss ou alguma
//     outra criada a partir dela existir (e nesse caso não deve ser alterada
//     nem por uma chamada de alteração de ss) -- essa responsabilidade é de
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


// cria e retorna uma ss que referencia a string C em str
ss ss_create(char *str);

// cria e retorna uma ss alterável que contém uma cópia de str
// o tamanho da memória alocada deve seguir as regras das operações de alteração
ss ss_copia(ss str);

// macro para criar uma ss que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
#define ss_(s) (ss){ .len = sizeof(s) - 1, .cap = 0, .mem = s }

// destrói a ss str.
// essa cadeia não deve ser utilizada após essa chamada
// essa função não faz nada por enquanto, só existe para criar bons costumes
void ss_destroy(ss str);

// retorna o tamanho (número de bytes) em str
int ss_len(ss str);

// retorna um ponteiro para o primeiro caractere de str
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *ss_mem(ss str);

// retorna o valor do byte na posição pos de str
// retorna 0 se pos for fora dos limites de str
char ss_ch(ss str, int pos);

// retorna um ss que é a substring que inicia na posição pos
//   de str e tem len bytes
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de str
// o ss retornado não é alterável, e só aponta para a memória de str
// o ss retornado não pode ser usado se str for alterada depois
//   desta chamada
// por exemplo: se str contém "barco":
//   pos=2,  len=3 sub="rco"
//   pos=2,  len=4 sub="rco" (len é ajustado para 3)
//   pos=-2, len=1 sub="c"
//   pos=-6, len=3 sub="ba" (pos é ajustado para 0, len para 2)
//   pos=10, len=5 sub="" (pos é ajustado para o final da string, tam para 0)
ss ss_sub(ss str, int pos, int len);

// retorna a primeira posição em str, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int ss_search_c(ss str, int pos, ss chs);

// retorna a primeira posição em str, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int ss_search_nc(ss str, int pos, ss chs);

// retorna a última posição em str, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int ss_search_rc(ss str, int pos, ss chs);

// retorna a última posição em str, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int ss_search_rnc(ss str, int pos, ss chs);

// retorna a primeira posição em str, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de str, a busca
//   deve iniciar no primeiro caractere
int ss_search_s(ss str, int pos, ss buscada);

// retorna true se str e strb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool ss_equal(ss str, ss strb);

// imprime a cadeia em str na saída padrão
void ss_print(ss str);

// funções novas/alteradas em ss, operações que alteram a string
// essas funções devem realocar a memória da string se necessário, de forma
//   que a quantidade de memória seja sempre superior ao tamanho da string
//   (e sempre seja colocado o '\0' após o final), e nunca inferior a 8,
//   e nem inferior a 1/3 do tamanho da string. o aumento/diminuição do 
//   tamanho da memória deve ser feito sempre com a razão 2 (os tamanhos
//   válidos são 8, 16, 32, 64 etc)

// adiciona ao final da cadeia apontada por pstr o conteúdo da cadeia strb
// não faz nada se a cadeia apontada por pstr não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
void ss_cat(ss *pstr, ss strb);

// adiciona o caractere c ao final da cadeia apontada por pstr
// não faz nada se a cadeia apontada por pstr não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
void ss_catc(ss *pstr, char c);

// insere a cadeia strb na posição pos da cadeia apontada por pstr
// não faz nada se a cadeia apontada por pstr não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
// pos pode ser negativo, e se estiver fora da cadeia *pstr, deve ser interpretado
//   como se fosse 0 (se estiver antes do início) ou como len (se após o final)
void ss_insert(ss *pstr, int pos, ss strb);

// remove a substring de tamanho len iniciando em pos em *pstr
// não faz nada se a cadeia apontada por pstr não for alterável
// pos e len devem ser interpretados como em ss_sub
void ss_remove(ss *pstr, int pos, int len);

// completa o final da string em *pstr com caracteres c, de forma que ela passe
//   a ter len caracteres
// não faz nada se a cadeia apontada por pstr não for alterável ou já possuir len
//   caracteres ou mais
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pstr
void ss_fill(ss *pstr, int len, char c);

// substitui, na cadeia apontada por pstr (que deve ser alterável),
//   a substring que inicia em pos e tem len bytes pela cadeia em strb.
// caso pos esteja fora da cadeia pstr, insere caracteres iguais a fill
//   no início ou no final de pstr, caso necessário, para que a posição 
//   seja válida
// exemplos, suponha que *pstr seja "abacaxi", fill seja '%':
//   pos=5, len=2, strb="te", *pstr deve ser transformado em "abacate"
//   7,0,"." -> "abacaxi."
//   9,0,"." -> "abacaxi%%."
//   9,0,"" -> "abacaxi%%"
//   9,10,"." -> "abacaxi%%."
//   -3,0,"123" -> "abac123axi"
//   0,1,"123" -> "123bacaxi"
//   -9,1,"123" -> "123%abacaxi"
//   3,200,"" -> "ab"
void ss_subst(ss *pstr, int pos, int len, ss strb, char fill);


// retorna uma cópia compatível com string C da string em str, em uma nova
// memória alocada com malloc de um byte a mais que o tamanho da string.
// é responsabilidade de quem chama esta função liberar essa memória.
char *ss_strc(ss str);

// cria uma cadeia alterável com o conteúdo do arquivo chamado name
// retorna uma cadeia vazia (não alterável) em caso de erro
ss ss_read_file(ss name);

// grava o conteúdo de str em um arquivo chamado name
void ss_write_file(ss str, ss name);


#endif // _S_H_
