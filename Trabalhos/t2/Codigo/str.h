#ifndef _STR_H_
#define _STR_H_

#include <stdbool.h>

// Cadeia de caracteres (str)
//
// TAD que implementa strings, como um tipo mais encapsulado e bem comportado
//   que strings padrão em C.
//
// Parte IV: strings unicode
//

// declarações {{{1
// Em unicode, cada caractere tem um código (chamado de codepoint) entre 0 e
//   0x10FFFF (exceto entre 0xD800 e 0xDFFF).
// Uma string (deste TAD) é um sequência de tais caracteres, codificados em
//   UTF8, em que cada código é codificado em UTF8 usando entre 1 e 4 bytes.
// Uma string pode conter quaisquer códigos válidos, **inclusive 0**.

// str é o tipo de dados para nossas strings
typedef struct str str;

// tipo para representar um byte da string
// em C23 esse tipo seria char8_t
typedef unsigned char byte;

// tipo para representar um codepoint (código de um caractere unicode)
typedef unsigned int unichar;
#define UNI_INV (unichar)-1   // representa um caractere inválido

// A estrutura é considerada aberta, mas só deve ser acessada diretamente
//   por quem conhece e aceita as consequências.
struct str {
  unsigned int tamc; // número de caracteres na string
  unsigned int tamb; // número de bytes na string
  unsigned int cap;  // número de bytes em mem, ou 0 se a string não é alterável
  byte *mem;         // ponteiro para o primeiro byte do primeiro caractere
};

// As strings são em geral passadas para e retornadas de funções acessadas por
//   cópia e não por referência (a estrutura que representa a string é passada
//   entre funções, e não um ponteiro para ela). Quando uma string é passada
//   por referência, é porque a função que a recebe tem o direito de alterá-la.
//
// Uma string pode ser alterável ou não.
// - alterável:
//   a estrutura str controla a memória em mem, uma memória que foi alocada
//     pela string, e que deve ser liberada na destruição
//   pode ser passada por referência para uma função que altera a string,
//     mas não quando existir outra string que dependa dela (uma substring
//     por exemplo) -- a responsabilidade de não violar esta regra é de quem
//     usa essas strings.
//   tem no campo cap o tamanho do região de memória alocada
//   o caractere que segue o último da cadeia deve ser '\0', para manter
//     compatibilidade com strings C. Com isso, a maior string possível tem
//     tamanho (em bytes) um a menos que o tamanho do região de memória
//     (como strings C normais)
// - não alterável:
//   o campo cap é 0
//   a memória não pertence à string, e não deve ser alterada nem liberada
//   a memória pode pertencer a outra string, ou ser uma constante C ou qualquer
//     memória passada para a função de criação (cujo tamanho é desconhecido)
//   uma string não alterável não deve ser passada por referência para funções
//     que implementam operações que alteram a string
//
// Nas funções abaixo, o argumento `pos` refere-se à posição de um
//   caractere (e não de um byte) em uma string. Esse argumento deve ser
//   interpretado da seguinte forma:
//   - se ele for 0 representa a posição do primeiro caractere da cadeia;
//     se for 1 a do segundo etc
//   - se ele for -1, representa a posição do último caractere da cadeia;
//     se for -2 a do penúltimo etc


// operações de criação e destruição {{{1

// cria e retorna uma string que referencia a string C em strC
// a string C deve estar codificada em UTF8, e termimar em \0
// o conteúdo da string C não deve ser alterado enquanto a str
//   retornada estiver em uso
// retorna uma string vazia se strC não contiver UTF8 válido
str s_cria(char *strC);

// macro para criar uma cadeia que contém a string C constante em s (não uma cópia)
// só pode ser usada com strings constantes (entre aspas)
// se essa string contém caracteres não ASCII, ela deve estar codificada em UTF8,
//   mas isso depende do ambiente onde esse programa foi processado.
// em C23, isso pode ser garantido prefixando a string com u8. por exemplo,
//    str oi = s_(u8"Olá!");
int u8_nchars_na_strC(char *strc);
#define s_(s)                      \
  (str) {                          \
    .tamc = u8_nchars_na_strC(s),  \
    .tamb = sizeof(s) - 1,         \
    .cap = 0,                      \
    .mem = (byte *)s               \
  }

// destrói a cadeia cad.
// essa cadeia não deve ser utilizada após essa chamada
// essa função deve liberar a memória em cadeias alteráveis
// essa função não faz nada com cadeias não alteráveis (não precisa destruir
//   cadeias não alteráveis)
void s_destroi(str cad);


// operações de acesso {{{1

// retorna o tamanho (número de caracteres) de cad
int s_tam(str cad);

// retorna um ponteiro para o primeiro caractere de cad
// esse ponteiro pode não apontar para uma string compatível com strings
//   padrão C (pode não possuir um \0 no final)
char *s_mem(str cad);

// retorna o valor do caractere na posição pos de cad
// retorna UNI_INV se pos for fora dos limites de cad
unichar s_ch(str cad, int pos);

// retorna uma cadeia que é a substring que inicia no caractere na posição pos
//   de cad e tem tam caracteres
// o ponteiro e tamanho da substring devem ser limitados ao conteúdo de cad
// o str retornado não é alterável, e só aponta para a memória de cad
// o str retornado não pode ser usado se cad for alterada depois
//   desta chamada
// por exemplo: se cad contém "bárcó":
//   pos=2,  tam=3 sub="rcó"
//   pos=2,  tam=4 sub="rcó" (tam é ajustado para 3)
//   pos=-2, tam=1 sub="c"
//   pos=-6, tam=3 sub="bá" (pos é ajustado para 0, tam para 2)
//   pos=10, tam=5 sub="" (pos é ajustado para o final da string, tam para 0)
str s_sub(str cad, int pos, int tam);

// retorna una cadeia que é uma substring de cad, sem eventuais caracteres
//   pertencentes a sobras no início e no final de cad
// a cadeia retornada é não alterável e referencia a cadeia original
// apara("teste 1", " .") -> "teste 1"
// apara("  teste 2. ", " .") -> "teste 2"
str s_apara(str cad, str sobras);


// operações de busca {{{1

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int s_busca_c(str cad, int pos, str chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
int s_busca_nc(str cad, int pos, str chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere pertencente a chs
// retorna -1 se não encontrar
int s_busca_rc(str cad, int pos, str chs);

// retorna a última posição em cad, não posterior a pos, onde tem
//   algum caractere não pertencente a chs
// retorna -1 se não encontrar
int s_busca_rnc(str cad, int pos, str chs);

// retorna a primeira posição em cad, não antes de pos, onde tem
//   a substring buscada
// retorna -1 se não encontrar
// se pos referenciar uma posição antes do primeiro caractere de cad, a busca
//   deve iniciar no primeiro caractere
// a string vazia é encontrada em todo lugar (se buscada for vazia, retorna o
//   valor corrigido de pos)
int s_busca_s(str cad, int pos, str buscada);


// operações de alteração {{{1

// o argumento pcad nessas funções deve apontar para uma string alterável
// essas funções devem realocar a memória da string se necessário, de forma
//   que a quantidade de memória seja sempre superior ao número de bytes na
//   string (e sempre seja colocado o '\0' após o final), e nunca inferior
//   a 8, e nem inferior a 1/3 do tamanho da string. o aumento/diminuição do 
//   tamanho da memória deve ser feito sempre com a razão 2 (os tamanhos
//   válidos são 8, 16, 32, 64 etc)

// cria e retorna uma string alterável que contém uma cópia de cad
// o tamanho da memória alocada deve seguir as regras das operações
//   de alteração
str s_copia(str cad);

// adiciona ao final da cadeia apontada por pcad o conteúdo da cadeia cadb
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void s_cat(str *pcad, str cadb);

// insere a cadeia cadb na posição pos da cadeia apontada por pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
// pos pode ser negativo, e se estiver fora da cadeia *pcad, deve ser interpretado
//   como se fosse 0 (se estiver antes do início) ou como tam (se após o final)
void s_insere(str *pcad, int pos, str cadb);

// remove a substring de tamanho tam iniciando em pos em *pcad
// não faz nada se a cadeia apontada por pcad não for alterável
// pos e tam devem ser interpretados como em s_sub
void s_remove(str *pcad, int pos, int tam);

// completa o final da string em *pcad com cópias de enchimento, de forma que
//   a string em *pcad passe a ter tam caracteres
// não faz nada se a cadeia apontada por pcad não for alterável ou já possuir tam
//   caracteres ou mais
// coloca um caractere '\0' na posição seguinte ao final da cadeia em *pcad
void s_preenche(str *pcad, int tam, str enchimento);

// substitui, na string apontada por pcad (que deve ser alterável),
//   a substring que inicia em pos e tem tam bytes pela cadeia em cadb.
// caso pos esteja fora da cadeia pcad, insere cópias de enchimento
//   no início ou no final de pcad, caso necessário, para que a posição 
//   seja válida
// exemplos, suponha que *pcad seja "abácaxi", enchimento seja "%-":
//   pos=5, tam=2, cadb="te", *pcad deve ser transformado em "abácate"
//   7,0,"." -> "abácaxi."
//   9,0,"." -> "abácaxi%-."
//   9,0,"" -> "abácaxi%-"
//   10,50,"." -> "abácaxi%-%."
//   -3,0,"123" -> "abác123axi"
//   0,1,"123" -> "123bácaxi"
//   -9,1,"123" -> "123-abácaxi"
//   2,200,"" -> "ab"
void s_subst(str *pcad, int pos, int tam, str cadb, str enchimento);


// operações de acesso a arquivo {{{1

// cria uma cadeia alterável com o conteúdo do arquivo chamado nome
// retorna uma cadeia vazia (não alterável) em caso de erro
str s_le_arquivo(str nome);

// grava o conteúdo de cad em um arquivo chamado nome
void s_grava_arquivo(str cad, str nome);


// outras operações {{{1

// retorna true se cad e cadb forem iguais, falso caso sejam diferentes
// se não forem do mesmo tamanho, são diferentes
bool s_igual(str cad, str cadb);

// imprime a cadeia em cad na saída padrão
void s_imprime(str cad);

// retorna uma cópia compatível com string C da string em cad, em uma nova
//   memória alocada com malloc de um byte a mais que o número de bytes da
//   string.
// é responsabilidade de quem chama esta função liberar essa memória.
char *s_strc(str cad);

#include "lstr.h"
// retorna uma lista de substrings de cad, delimitadas em cad por algum dos
//   caracteres em separadores. Os separadores não são colocados nas substrings.
// as substrings referenciam cad, que não deve ser alterada enquanto essas
//   substrings estiverem sendo usadas.
// separa "abacaxi,banana;maçã", ",;" -> ["abacaxi", "banana", "maçã"]
// separa "abacaxi,banana;maçã", "," -> ["abacaxi", "banana;maçã"]
Lstr s_separa(str cad, str separadores);

#endif // _STR_H_
// vim: foldmethod=marker shiftwidth=2

