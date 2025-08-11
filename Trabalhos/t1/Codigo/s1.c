#include "s1.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static void s1_ok(s1 str)
{
  assert(str.len >= 0);
  assert(str.len == 0 || str.mem != NULL);
}

s1 s1_create(char *str)
{
  return (s1){.len = strlen(str), .mem = str};
}

void s1_destroy(s1 str)
{
  s1_ok(str);
}

int s1_len(s1 str)
{
  s1_ok(str);
  return str.len;
}
char *s1_mem(s1 str)
{
  s1_ok(str);
  return str.mem;
}

char s1_ch(s1 str, int pos)
{
  s1_ok(str);
  // ... retorna '\0' em alguns casos
  return str.mem[pos];
}

s1 s1_sub(s1 str, int pos, int len)
{
  s1_ok(str);
  // ...
  return s1_("?");
}

int s1_search_c(s1 str, int pos, s1 chs)
{
  s1_ok(str);
  // ...
  return -1;
}

int s1_search_nc(s1 str, int pos, s1 chs)
{
  s1_ok(str);
  // ...
  return -1;
}

int s1_search_rc(s1 str, int pos, s1 chs)
{
  s1_ok(str);
  // ...
  return -1;
}

int s1_search_rnc(s1 str, int pos, s1 chs)
{
  s1_ok(str);
  // ...
  return -1;
}

int s1_search_s(s1 str, int pos, s1 buscada)
{
  s1_ok(str);
  s1_ok(buscada);
  // ...
  return -1;
}

bool s1_equal(s1 str, s1 strb)
{
  s1_ok(str);
  s1_ok(strb);
  // ...
  return false;
}

void s1_print(s1 str)
{
  s1_ok(str);
  fwrite(str.mem, 1, str.len, stdout);
  // poderia ser também printf("%.*s", cad.len, cad.mem);
}
