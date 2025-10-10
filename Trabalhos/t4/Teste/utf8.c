#include "utf8.h"

#include <stdbool.h>
#include <stdlib.h>

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


int u8_bytes_no_unichar_que_comeca_com(byte b1)
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

bool u8_unichar_valido(unichar uni)
{
  if (uni > 0x10FFFF) return false;
  if (uni >= 0xD800 && uni <= 0xDFFF) return false;
  return true;
}

int u8_unichar_nos_bytes(byte *s, int maxn, unichar *puni)
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
  if (!u8_unichar_valido(uni)) return -1;

  if (puni != NULL) *puni = uni;
  return nbytes;
}

int u8_conta_unichar_nos_bytes(byte *ptr, int nbytes)
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

byte *u8_avanca_unichar(byte *ptr, int n)
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

int u8_converte_pra_utf8(unichar uni, byte *buf)
{
  if (!u8_unichar_valido(uni)) uni = 0xFFFD;
  if (uni <= 127) {
    buf[0] = uni;
    return 1;
  } else if (uni < 0x800) {
    buf[0] = 0b11000000 | ((uni >>  6) & 0b00011111);
    buf[1] = 0b10000000 | ( uni        & 0b00111111);
    return 2;
  } else if (uni < 0x10000) {
    buf[0] = 0b11100000 | ((uni >> 12) & 0b00001111);
    buf[1] = 0b10000000 | ((uni >>  6) & 0b00111111);
    buf[2] = 0b10000000 | ( uni        & 0b00111111);
    return 3;
  } else {
    buf[0] = 0b11110000 | ((uni >> 18) & 0b00000111);
    buf[1] = 0b10000000 | ((uni >> 12) & 0b00111111);
    buf[2] = 0b10000000 | ((uni >>  6) & 0b00111111);
    buf[3] = 0b10000000 | ( uni        & 0b00111111);
    return 4;
  }
}
