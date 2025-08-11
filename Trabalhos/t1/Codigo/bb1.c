#include "bb1.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static void bb_ok(bb cad)
{
  assert(cad.tam >= 0);
  assert(cad.tam == 0 || cad.mem != NULL);
}

bb bb_cria(char *str)
{
  return (bb){.tam = strlen(str), .mem = str};
}

void bb_destroi(bb cad)
{
  bb_ok(cad);
}

int bb_tam(bb cad)
{
  bb_ok(cad);
  return cad.tam;
}
char *bb_mem(bb cad)
{
  bb_ok(cad);
  return cad.mem;
}

char bb_ch(bb cad, int pos)
{
  bb_ok(cad);
  // ... retorna '\0' em alguns casos
  return cad.mem[pos];
}

bb bb_sub(bb cad, int pos, int tam)
{
  bb_ok(cad);
  // ...
  return bb_("?");
}

int bb_busca_ch(bb cad, int pos_ini, bb chs)
{
  bb_ok(cad);
  // ...
  return -1;
}

int bb_busca_nch(bb cad, int pos_ini, bb chs)
{
  bb_ok(cad);
  // ...
  return -1;
}

int bb_busca_rch(bb cad, int pos_ini, bb chs)
{
  bb_ok(cad);
  // ...
  return -1;
}

int bb_busca_rnch(bb cad, int pos_ini, bb chs)
{
  bb_ok(cad);
  // ...
  return -1;
}

int bb_busca_bb(bb cad, int pos_ini, bb buscada)
{
  bb_ok(cad);
  bb_ok(buscada);
  // ...
  return -1;
}

bool bb_igual(bb cad, bb outra_cad)
{
  bb_ok(cad);
  bb_ok(outra_cad);
  // ...
  return false;
}

void bb_print(bb cad)
{
  bb_ok(cad);
  fwrite(cad.mem, 1, cad.tam, stdout);
  // poderia ser também printf("%.*s", cad.tam, cad.mem);
}
