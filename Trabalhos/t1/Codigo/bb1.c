#include "bb1.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static void bb1_ok(bb1 cad)
{
  assert(cad.tam >= 0);
  assert(cad.tam == 0 || cad.mem != NULL);
}

bb1 bb1_cria(char *str)
{
  return (bb1){.tam = strlen(str), .mem = str};
}

void bb1_destroi(bb1 cad)
{
  bb1_ok(cad);
}

int bb1_tam(bb1 cad)
{
  bb1_ok(cad);
  return cad.tam;
}
char *bb1_mem(bb1 cad)
{
  bb1_ok(cad);
  return cad.mem;
}

char bb1_ch(bb1 cad, int pos)
{
  bb1_ok(cad);
  // ... retorna '\0' em alguns casos
  return cad.mem[pos];
}

bb1 bb1_sub(bb1 cad, int pos, int tam)
{
  bb1_ok(cad);
  // ...
  return bb1_("?");
}

int bb1_busca_c(bb1 cad, int pos, bb1 chs)
{
  bb1_ok(cad);
  // ...
  return -1;
}

int bb1_busca_nc(bb1 cad, int pos, bb1 chs)
{
  bb1_ok(cad);
  // ...
  return -1;
}

int bb1_busca_rc(bb1 cad, int pos, bb1 chs)
{
  bb1_ok(cad);
  // ...
  return -1;
}

int bb1_busca_rnc(bb1 cad, int pos, bb1 chs)
{
  bb1_ok(cad);
  // ...
  return -1;
}

int bb1_busca_s(bb1 cad, int pos, bb1 buscada)
{
  bb1_ok(cad);
  bb1_ok(buscada);
  // ...
  return -1;
}

bool bb1_igual(bb1 cad, bb1 cadb)
{
  bb1_ok(cad);
  bb1_ok(cadb);
  // ...
  return false;
}

void bb1_imprime(bb1 cad)
{
  bb1_ok(cad);
  fwrite(cad.mem, 1, cad.tam, stdout);
  // poderia ser também printf("%.*s", cad.tam, cad.mem);
}
