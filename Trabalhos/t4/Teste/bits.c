#include "bits.h"
#include "utf8.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <stdio.h>

struct bits {
  int largura;
  int altura;
  int bytes_por_linha;
  byte *bytes;
  byte *byte_atual;
  int bits_no_byte_atual;
};

#define BITS_POR_BYTE (sizeof(byte)*CHAR_BIT)

// quantos bytes precisa para tantos bits?
static int nbytes_para_nbits(int nbits)
{
  int nbytes = nbits / BITS_POR_BYTE;
  if (nbits % BITS_POR_BYTE != 0) nbytes++;
  return nbytes;
}

Bits b_cria(int largura, int altura)
{
  if (largura < 1 || altura < 1) return NULL;
  Bits bits = malloc(sizeof(struct bits));
  if (bits == NULL) return NULL;
  int larg_b = largura / 8;
  if (largura % 8 != 0) larg_b++;
  bits->bytes = calloc(altura, larg_b);
  if (bits->bytes == NULL) {
    free(bits);
    return NULL;
  }
  bits->largura = largura;
  bits->altura = altura;
  bits->bytes_por_linha = larg_b;
  return bits;
}

Bits b_cria_arq(char *nome)
{
  FILE *f = fopen(nome, "r");
  Bits b = NULL;
  if (f == NULL) return NULL;
  int larg, alt;
  char ch;
  if (fscanf(f, "%d%d%c", &alt, &larg, &ch) != 3) goto err;
  if (ch != '\n') goto err;
  b = b_cria(larg, alt);
  if (b == NULL) goto err;
  for (int l = 0; l < alt; l++) {
    byte bit = (byte)1 << (BITS_POR_BYTE - 1);
    byte *ptr = b->bytes + l * b->bytes_por_linha;
    for (int c = 0; /*c < larg*/; c++) {
      if (bit == 0) {
        ptr++;
        bit = (byte)1 << (BITS_POR_BYTE - 1);
      }
      if (fscanf(f, "%c", &ch) != 1) break;
      if (ch == '\n') break;
      if (c < larg && ch != ' ') *ptr |= bit;
      bit >>= 1;
    }
  }
  fclose(f);
  return b;
err:
  if (f != NULL) fclose(f);
  if (b != NULL) b_destroi(b);
  return NULL;
}

void b_destroi(Bits b)
{
  if (b == NULL) return;
  free(b->bytes);
  free(b);
}

void b_limpa(Bits b)
{
  memset(b->bytes, 0, b->altura * b->bytes_por_linha);
}

// retorna um byte que só tem os nbits de by, entre os bits_dir mais à direita.
// retorna os bits alinhados à direita.
// se by é abcdefgh, bits_dir é 5 e nbits é 2, retorna 000000de
static inline byte extrai_bits(byte by, int bits_dir, int nbits)
{
  return ((byte)(by << (BITS_POR_BYTE - bits_dir)) >> (BITS_POR_BYTE - nbits));
}

static inline int menor(int a, int b) { return a < b ? a : b; }
static inline int maior(int a, int b) { return a > b ? a : b; }

// posiciona no x-ésimo bit da y-ésima linha
void b_posiciona(Bits b, int x, int y)
{
  b->byte_atual = b->bytes + y * b->bytes_por_linha + x / BITS_POR_BYTE;
  b->bits_no_byte_atual = BITS_POR_BYTE - x % BITS_POR_BYTE;
}

// avança a posição nbits à direita
void b_avanca_bits(Bits b, int nbits)
{
  while (nbits > 0) {
    int sub = menor(nbits, b->bits_no_byte_atual);
    nbits -= sub;
    b->bits_no_byte_atual -= sub;
    if (b->bits_no_byte_atual == 0) {
      b->byte_atual++;
      b->bits_no_byte_atual = BITS_POR_BYTE;
    }
  }
}

// retorna os próximos n bits, e avança esse tanto.
// os bits retornados são alinhados à direita do byte retornado.
// nbits não pode ser maior que BITS_POR_BYTE.
// não pode ser usado além do final da linha corrente.
byte b_proximos_bits(Bits b, int nbits)
{
  byte bits;
  if (b->bits_no_byte_atual >= nbits) {
    bits = extrai_bits(*b->byte_atual, b->bits_no_byte_atual, nbits);
    b->bits_no_byte_atual -= nbits;
    if (b->bits_no_byte_atual == 0) {
      b->byte_atual++;
      b->bits_no_byte_atual = BITS_POR_BYTE;
    }
  } else {
    byte p1 = extrai_bits(*b->byte_atual, b->bits_no_byte_atual, b->bits_no_byte_atual);
    int nbits2 = nbits - b->bits_no_byte_atual;
    b->byte_atual++;
    b->bits_no_byte_atual = BITS_POR_BYTE;
    byte p2 = extrai_bits(*b->byte_atual, b->bits_no_byte_atual, nbits2);
    b->bits_no_byte_atual -= nbits2;
    bits = (p1 << nbits2) | p2;
  }
  return bits;
}

static void ajusta(int *v0, int dv0, int *v1, int *dv1)
{
  if (*v1 < 0) { *v0 -= *v1; *v1 = 0; }
  if (*v0 < 0) { *v1 -= *v0; *v0 = 0; }
  *dv1 -= *v1;
  if (*v0 + *dv1 > dv0) *dv1 = dv0 - *v0;
}

void b_desenha(Bits b, int x, int y, Bits d)
{
  // int primeiro_bit_b = maior(x, 0);
  // int primeiro_bit_d = maior(0, -x);
  // int bits_por_linha = d->largura - primeiro_bit_d;
  // if (primeiro_bit_b + bits_por_linha > b->largura)
  //   bits_por_linha = b->largura - primeiro_bit_b;
  int primeiro_bit_b = x;
  int primeiro_bit_d = 0;
  int bits_por_linha = d->largura;
  ajusta(&primeiro_bit_b, b->largura, &primeiro_bit_d, &bits_por_linha);
  if (bits_por_linha <= 0) return;

  int primeira_linha_b = y;
  int primeira_linha_d = 0;
  int nlinhas = d->altura;
  ajusta(&primeira_linha_b, b->altura, &primeira_linha_d, &nlinhas);
  if (nlinhas <= 0) return;

  for (int l = 0; l < nlinhas; l++) {
    b_posiciona(b, primeiro_bit_b, primeira_linha_b + l);
    b_posiciona(d, primeiro_bit_d, primeira_linha_d + l);
    int nbits;
    for (int bits_que_faltam = bits_por_linha; bits_que_faltam > 0; bits_que_faltam -= nbits) {
      nbits = menor(b->bits_no_byte_atual, bits_que_faltam);

      *b->byte_atual |= b_proximos_bits(d, nbits) << (b->bits_no_byte_atual - nbits);

      b_avanca_bits(b, nbits);
    }
  }
}

bool b_colide(Bits b, int x, int y, Bits d)
{
  int primeiro_bit_b = x;
  int primeiro_bit_d = 0;
  int bits_por_linha = d->largura;
  ajusta(&primeiro_bit_b, b->largura, &primeiro_bit_d, &bits_por_linha);
  if (bits_por_linha <= 0) return false;

  int primeira_linha_b = y;
  int primeira_linha_d = 0;
  int nlinhas = d->altura;
  ajusta(&primeira_linha_b, b->altura, &primeira_linha_d, &nlinhas);
  if (nlinhas <= 0) return false;

  for (int l = 0; l < nlinhas; l++) {
    b_posiciona(b, primeiro_bit_b, primeira_linha_b + l);
    b_posiciona(d, primeiro_bit_d, primeira_linha_d + l);
    int nbits;
    for (int bits_que_faltam = bits_por_linha; bits_que_faltam > 0; bits_que_faltam -= nbits) {
      int nbits = menor(b->bits_no_byte_atual, bits_que_faltam);

      if ((b_proximos_bits(b, nbits) & b_proximos_bits(d, nbits)) != 0)
        return true;
    }
  }

  return false;
}

// unicode tem caracteres para todas as combinações de bits em uma matriz 2x3
// a maior parte está em ordem, exceto o vazio, o cheio e as duas barras verticais
// u1fb00 - 11110000 10011111 10101100 10xxxxxx  00-3b
// bits              unicode
// 000000          - ' '
// 000001 a 010100 - u1fb00 + 000000 a 010011
// 010101          - u258c
// 010110 a 101001 - u1fb00 + 010100 a 100111
// 101010          - u2590
// 101011 a 111110 - u1fb00 + 101000 a 111011
// 111111          - u2588
void b_imprime_uni_2x3(Bits b)
{
  // cada 6 bits são codificados em um unicode, que usa até 4 chars em utf-8,
  //   mais um \n por linha, mais um \0
  char buf[b->altura*b->largura*4/6 + (b->altura+2)/3 + 1];
  char *s = buf;
  for (int l = 0; l < b->altura; l += 3) {
    for (int c = 0; c < b->largura; c += 2) {
      // junta os 6 bits, na ordem esperada
      byte bits = 0;
      b_posiciona(b, c, l);
      bits |= b_proximos_bits(b, 1) | (b_proximos_bits(b, 1) << 1);
      if (l+1 < b->altura) {
        b_posiciona(b, c, l + 1);
        bits |= (b_proximos_bits(b, 1) << 2) | (b_proximos_bits(b, 1) << 3);
        if (l+2 < b->altura) {
          b_posiciona(b, c, l + 2);
          bits |= (b_proximos_bits(b, 1) << 4) | (b_proximos_bits(b, 1) << 5);
        }
      }
      // anexa o código do caractere unicode correspondente
      if      (bits == 0b111111) { s = stpcpy(s, "\u2588"); }
      else if (bits == 0b101010) { s = stpcpy(s, "\u2590"); }
      else if (bits == 0b010101) { s = stpcpy(s, "\u258c"); }
      else if (bits == 0b000000) { s = stpcpy(s, " "); }
      else {
        if      (bits > 0b101010) { bits -= 3; }
        else if (bits > 0b010101) { bits -= 2; }
        else                      { bits -= 1; }
        s = stpcpy(s, "\U0001FB00");
        *(s-1) += bits;
      }
    }
    s = stpcpy(s, "\n");
  }
  fwrite(buf, 1, s - buf, stdout);
}

void b_imprime_ascii(Bits b)
{
  for (int l = 0; l < b->altura; l++) {
    b_posiciona(b, 0, l);
    for (int c = 0; c < b->largura; c++) {
      if (b_proximos_bits(b, 1) == 1) putchar('X'); else putchar('.');
    }
    putchar('\n');
  }
}
