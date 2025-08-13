#include "cc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

static void cc_ok(cc cad)
{
  assert(cad.tam >= 0);
  assert(cad.cap >= 0);
  assert(cad.mem != NULL);
  if (cad.cap > 0) {
    assert(cad.cap > cad.tam);
    assert(cad.cap >= MIN_ALLOC);
    assert(cad.cap == (cad.cap & -cad.cap)); // é potência de 2?
  }

}

cc cc_cria(char *str)
{
  return (cc){.tam = strlen(str), .cap = 0, .mem = str};
}

void cc_destroi(cc cad)
{
  cc_ok(cad);
  if (cad.cap > 0) free(cad.mem);
}

int cc_tam(cc cad)
{
  cc_ok(cad);
  return cad.tam;
}
char *cc_mem(cc cad)
{
  cc_ok(cad);
  return cad.mem;
}

char cc_ch(cc cad, int pos)
{
  cc_ok(cad);
  // ... retorna '\0' em alguns casos
  return cad.mem[pos];
}

cc cc_sub(cc cad, int pos, int tam)
{
  cc_ok(cad);
  // ...
  return cc_("?");
}

char *cc_strc(cc cad)
{
  cc_ok(cad);
  // ...
  return NULL;
}

cc cc_copia(cc cad)
{
  cc_ok(cad);
  // ...
  return cc_("?");
}

int cc_busca_c(cc cad, int pos_ini, cc chs)
{
  cc_ok(cad);
  // ...
  return -1;
}

int cc_busca_nc(cc cad, int pos_ini, cc chs)
{
  cc_ok(cad);
  // ...
  return -1;
}

int cc_busca_rc(cc cad, int pos_ini, cc chs)
{
  cc_ok(cad);
  // ...
  return -1;
}

int cc_busca_rnc(cc cad, int pos_ini, cc chs)
{
  cc_ok(cad);
  // ...
  return -1;
}

int cc_busca_s(cc cad, int pos_ini, cc buscada)
{
  cc_ok(cad);
  cc_ok(buscada);
  // ...
  return -1;
}

bool cc_igual(cc cad, cc outra_cad)
{
  cc_ok(cad);
  cc_ok(outra_cad);
  // ...
  return false;
}


void cc_subst(cc *pcad, int pos, int tam, cc cad, char preenche)
{
  cc_ok(*pcad);
  cc_ok(cad);
  // ...
}

void cc_imprime(cc cad)
{
  cc_ok(cad);
  fwrite(cad.mem, 1, cad.tam, stdout);
}

cc cc_le_arquivo(cc nome)
{
  cc_ok(nome);
  char *nomec = cc_strc(nome);
  FILE *arq = fopen(nomec, "r");
  free(nomec);
  if (arq == NULL) return cc_("");
  // ...
  fclose(arq);
  return cc_("");
}

void cc_grava_arquivo(cc cad, cc nome)
{
  // ...
}
