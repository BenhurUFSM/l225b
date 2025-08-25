#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

// funções auxiliares para codificação UTF8 {{{1

// codificação UTF8
// Um codepoint unicode tem um valor entre 0 e x10FFFF.
// UTF8 codifica um codepoint em 1 a 4 bytes, dependendo do valor do codepoint.
// O primeiro byte inicia com o primeiro bit (o mais significativo) 0 ou com os
// dois primeiros bits 11.
// Os demais bytes (se houver) iniciam com os 2 primeiros bits 10.
// Analisando esses bits, é fácil identificar um byte inicial ou de continuação.
// Cada byte UTF8 contém alguns dos bits que compõem o valor do codepoint,
// representados por x na tabela abaixo
//
// hexa do codepoint    1º byte   2º byte   3º byte   4º byte
// 000000 a 00007F     0xxxxxxx
// 000080 a 0007FF     110xxxxx  10xxxxxx
// 000800 a 00FFFF     1110xxxx  10xxxxxx  10xxxxxx
// 010000 a 10FFFF     11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
//
// Os codepoints entre 0xD800 e 0xDFFF são usados como auxiliares na codificação
// UTF16 e não podem ser usados para representar caracteres.
// Quando um codepoint pode ser representado por mais de um tamanho de sequência
// (o código 101 poderia ser representado por 00000101 ou 11000000 10000101 por
// exemplo), só a representação mais curta é considerada válida.


// retorna o número de bytes usados para codificar um caractere em unicode
//   quando o primeiro byte é codificado é b1
static int u8_bytes_no_unichar_que_comeca_com(byte b1)
{
  if (b1 <= 0b01111111) return 1;
  if (b1 >= 0b11000000 && b1 <= 0b11011111) return 2;
  if (b1 >= 0b11100000 && b1 <= 0b11101111) return 3;
  if (b1 >= 0b11110000 && b1 <= 0b11110111) return 4;
  return -1; // ou aborta?
}

// b é um byte de continuação de uma codificação utf8?
static bool u8_byte_de_continuacao(byte b)
{
  return (b & 0b11000000) == 0b10000000;
}

// adiciona os 6 bits do byte do continuação cont em *uni
// retorna false se cont não for válido
static bool u8_soma_bits_de_continuacao(unichar *uni, byte cont)
{
  if (!u8_byte_de_continuacao(cont)) return false;
  byte bits = cont & 0b00111111;
  *uni = ((*uni) << 6) | bits;
  return true;
}

// calcula o codepoint do caractere que inicia no byte apontado por s
// que tem no máximo maxn bytes
// retorna o número de bytes no caractere ou -1 se erro (código inválido,
//   codificação inválida ou precisa mais que maxn bytes)
// se puni não for NULL e o retorno não for -1, coloca o caractere em *puni
static int u8_unichar_nos_bytes(byte *s, int maxn, unichar *puni)
{
  byte primeiro_byte = s[0];
  int nbytes = u8_bytes_no_unichar_que_comeca_com(primeiro_byte);
  if (nbytes < 1 || nbytes > maxn || nbytes > 4) return -1;

  // monta o código uni à partir dos bytes utf8
  //   máscara binária com os bits válidos para o 1o byte de cada tamanho
  byte mascara[] = { 0b01111111, 0b00011111, 0b00001111, 0b00000111, };
  unichar uni = primeiro_byte & mascara[nbytes - 1];
  int nbytes_adicionados = 1;
  while (nbytes > nbytes_adicionados) {
    if (!u8_soma_bits_de_continuacao(&uni, s[nbytes_adicionados])) return -1;
    nbytes_adicionados++;
  }

  // verifica se o código é válido
  if (nbytes > 1 && uni < 0x80) return -1;
  if (nbytes > 2 && uni < 0x800) return -1;
  if (nbytes > 3 && uni < 0x10000) return -1;
  if (uni > 0x10FFFF) return -1;
  if (uni >= 0xD800 && uni <= 0xDFFF) return -1;

  if (puni != NULL) *puni = uni;
  return nbytes;
}

// conta quantos caracteres unicode estão codificados em utf8 nos
//   nbytes que iniciam em *ptr
// faz a verificação da codificação utf8
// retorna o número de caracteres ou -1 se houver erro na codificação
static int u8_conta_unichar_nos_bytes(byte *ptr, int nbytes)
{
  int num_uni = 0;
  while (nbytes > 0) {
    int nb1 = u8_unichar_nos_bytes(ptr, nbytes, NULL);
    if (nb1 < 1) return -1;
    num_uni++;
    ptr += nb1;
    nbytes -= nb1;
  }
  return num_uni;
}

// retorna o ponteiro para o primeiro byte do caractere que está
//   n caracteres adiante do caractere cujo 1o byte está em *ptr
// quem chama garante que existem pelo menos n caracteres
// retorna NULL em caso de erro na codificação (que não deveria
//   acontecer, esta função é para ser usada só em região contendo
//   utf8 válido)
// não tem suporte a n negativo
static byte *u8_avanca_unichar(byte *ptr, int n)
{
  byte *p = ptr;
  for (int i = 0; i < n; i++) {
    int nb1 = u8_bytes_no_unichar_que_comeca_com(*p);
    if (nb1 < 1) return NULL;
    p += nb1;
  }
  return p;
}

// conta o número de caracteres unicode codificados em UTF8 em uma string C
// função pública, só para a macro s_()
// não faz uma análise mais completa da validade como u8_conta_unichar_nos_bytes
int u8_nchars_na_strC(char *s)
{
  int l = 0;
  while (*s != '\0') {
    if (!u8_byte_de_continuacao((byte)*s)) l++;
    s++;
  }
  return l;
}


// funções auxiliares {{{1

// testa se um número é potência de 2
static bool pot2(int n)
{
    return n == (n & -n);
}

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(str cad)
{
  assert(cad.tamb >= cad.tamc);
  if (cad.tamc > 0) {
    assert(cad.mem != NULL);
  }
  if (cad.cap > 0) {
    assert(cad.mem != NULL);
    assert(cad.cap > cad.tamb);
    assert(cad.mem[cad.tamb] == '\0');
    assert(cad.cap >= MIN_ALLOC);
    assert(pot2(cad.cap));
  }
}

// verifica se a string apontada por pcad é alterável
static bool s_alteravel(str *pcad)
{
  return pcad->cap > 0;
}

// operações de criação e destruição {{{1

str s_cria(char *strC)
{
  str cad;
  cad.mem = (byte *)strC;
  cad.cap = 0;
  cad.tamb = strlen(strC);
  int tamc = u8_conta_unichar_nos_bytes(cad.mem, cad.tamb);
  if (tamc == -1) return (str){0,0,0,NULL};
  cad.tamc = tamc;
  return cad;
}

void s_destroi(str cad)
{
  s_ok(cad);
  if (cad.cap > 0) free(cad.mem);
}


// operações de acesso {{{1

int s_tam(str cad)
{
  s_ok(cad);
  return cad.tamc;
}

char *s_mem(str cad)
{
  s_ok(cad);
  return (char *)cad.mem;
}

// retorna o endereço do byte onde inicia o caractere na posição pos de cad
// versão sem medo -- pos está dentro dos limites válidos para cad
static byte *s_ender_pos_sm(str cad, int pos)
{
  if (pos == cad.tamc) return cad.mem + cad.tamb;
  return u8_avanca_unichar(cad.mem, pos);
}

unichar s_ch(str cad, int pos)
{
  s_ok(cad);
  // ... retorna UNI_INV em alguns casos
  byte *ptr = s_ender_pos_sm(cad, pos);
  int nbytes_pulados = ptr - cad.mem;
  unichar uni = UNI_INV;
  u8_unichar_nos_bytes(ptr, cad.tamb - nbytes_pulados, &uni);
  return uni;
}

str s_sub(str cad, int pos, int tam)
{
  s_ok(cad);
  // ...
  return s_("?");
}


// operações de busca {{{1

int s_busca_c(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  // ...
  return -1;
}

int s_busca_nc(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  // ...
  return -1;
}

int s_busca_rc(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  // ...
  return -1;
}

int s_busca_rnc(str cad, int pos, str chs)
{
  s_ok(cad);
  s_ok(chs);
  // ...
  return -1;
}

int s_busca_s(str cad, int pos, str buscada)
{
  s_ok(cad);
  s_ok(buscada);
  // ...
  return -1;
}


// operações de alteração {{{1

str s_copia(str cad)
{
  s_ok(cad);
  // ...
  return s_("?");
}

void s_cat(str *pcad, str cadb)
{
  s_ok(*pcad);
  s_ok(cadb);
  if (!s_alteravel(pcad)) return;
  // ...
}

void s_insere(str *pcad, int pos, str cadb)
{
  s_ok(*pcad);
  s_ok(cadb);
  if (!s_alteravel(pcad)) return;
  // ...
}

void s_remove(str *pcad, int pos, int tam)
{
  s_ok(*pcad);
  if (!s_alteravel(pcad)) return;
  // ...
}

void s_preenche(str *pcad, int tam, str enchimento)
{
  s_ok(*pcad);
  s_ok(enchimento);
  if (!s_alteravel(pcad)) return;
  // ...
}

void s_subst(str *pcad, int pos, int tam, str cadb, str enchimento)
{
  s_ok(*pcad);
  s_ok(cadb);
  s_ok(enchimento);
  if (!s_alteravel(pcad)) return;
  // ...
}


// operações de acesso a arquivo {{{1

str s_le_arquivo(str nome)
{
  s_ok(nome);

  // abre o arquivo
  FILE *arq;
  char *nomec = s_strc(nome);
  arq = fopen(nomec, "r");
  free(nomec);
  if (arq == NULL) return s_("");
  // ...
  fclose(arq);
  return s_("");
}


void s_grava_arquivo(str cad, str nome)
{
  s_ok(cad);
  s_ok(nome);
  // ...
}


// outras operações {{{1

bool s_igual(str cad, str cadb)
{
  s_ok(cad);
  s_ok(cadb);
  // ...
  return false;
}

void s_imprime(str cad)
{
  s_ok(cad);
  fwrite(cad.mem, 1, cad.tamb, stdout);
}

char *s_strc(str cad)
{
  // ...
  return NULL;
}

// vim: foldmethod=marker shiftwidth=2
