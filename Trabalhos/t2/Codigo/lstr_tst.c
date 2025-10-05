#include "lstr.h"
#include "str.h"
#include <stdarg.h>
#include <stdio.h>

int num_teste = 0;

void inicia_teste(const char *nome)
{
  printf("\n======== '%s' ========\n", nome);
  num_teste = 0;
}

void verifica_str(str a, str b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  printf("'");
  s_imprime(a);
  printf("' ");
  if (s_igual(a, b)) {
    printf("OK\n");
  } else {
    printf("(diferente de '");
    s_imprime(b);
    printf("'): FALHOU\n");
  }
}

void verifica_c(unichar a, unichar b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  printf("'%x' ", a);
  if (a == b) {
    printf("OK\n");
  } else {
    printf("(diferente de '%x'): FALHOU\n", b);
  }
}

void verifica_int(int a, int b)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  printf("'%d' ", a);
  if (a == b) {
    printf("OK\n");
  } else {
    printf("(diferente de '%d'): FALHOU\n", b);
  }
}

void verifica_bool(bool v)
{
  num_teste++;
  printf("teste %d: ", num_teste);
  printf("'%s' ", v ? "true" : "false");
  if (v) {
    printf("OK\n");
  } else {
    printf("(diferente de true): FALHOU\n");
  }
}

void testa_avanca(Lstr l, int n, str itens[n])
{
  inicia_teste(__FUNCTION__);
  ls_inicio(l);
  verifica_bool(!ls_item_valido(l));
  for (int i = 0; i < n; i++) {
    verifica_bool(ls_avanca(l));
    verifica_str(itens[i], ls_item(l));
  }
  verifica_bool(!ls_avanca(l));
  verifica_bool(!ls_item_valido(l));
}

void testa_recua(Lstr l, int n, str itens[n])
{
  inicia_teste(__FUNCTION__);
  ls_final(l);
  verifica_bool(!ls_item_valido(l));
  for (int i = n - 1; i >= 0; i--) {
    verifica_bool(ls_recua(l));
    verifica_str(itens[i], ls_item(l));
  }
  verifica_bool(!ls_recua(l));
  verifica_bool(!ls_item_valido(l));
}

void testa_posiciona(Lstr l, int n, str itens[n])
{
  inicia_teste(__FUNCTION__);
  for (int i = 0; i < n; i++) {
    ls_posiciona(l, i);
    verifica_bool(ls_item_valido(l));
    verifica_str(itens[i], ls_item(l));
  }
}

// função com número variável de argumentos (como o printf).
// funciona assim: a função declara os argumentos conhecidos, e depois "...".
// antes de C23 tinha que ter pelo menos um argumento conhecido.
// declara-se uma variável do tipo va_list (incluir stdarg.h).
// inicializa-se essa variável chamando va_start, com o último argumento
//   conhecido como parâmetro (esse parâmetro não é necessário em C23).
// cada argumento desconhecido pode ser obtido com va_arg, passando o tipo
//   do argumento como parâmetro.
// no final, chamar va_end.
// tem que ter alguma forma de a função saber quantos argumentos tem, e
//   o tipo de cada um (printf encontra isso na string de formato, por
//   exemplo).
void testa_lista(Lstr l, int n, ...)
{
  inicia_teste(__FUNCTION__);
  printf("'");
  ls_imprime(l);
  printf("'\n");
  verifica_int(n, ls_tam(l));
  str itens[n];
  va_list arg_list;
  va_start(arg_list, n);
  for (int i = 0; i < n; i++) {
    itens[i] = va_arg(arg_list, str);
  }
  va_end(arg_list);

  testa_posiciona(l, n, itens);
  testa_avanca(l, n, itens);
  testa_recua(l, n, itens);
}

void testa_insere()
{
  inicia_teste(__FUNCTION__);
  Lstr l = ls_cria();
  testa_lista(l, 0);
  verifica_bool(!ls_item_valido(l));
  ls_insere_depois(l, s_("A"));
  testa_lista(l, 1, s_("A"));
  ls_destroi(l);

  l = ls_cria();
  ls_insere_depois(l, s_("A"));
  ls_insere_depois(l, s_("C"));
  ls_insere_antes(l, s_("B"));
  testa_lista(l, 3, s_("A"), s_("B"), s_("C"));

  ls_inicio(l);
  ls_insere_antes(l, s_("a"));
  testa_lista(l, 4, s_("a"), s_("A"), s_("B"), s_("C"));
  ls_inicio(l);
  ls_insere_depois(l, s_("b"));
  testa_lista(l, 5, s_("b"), s_("a"), s_("A"), s_("B"), s_("C"));
  ls_final(l);
  ls_insere_antes(l, s_("c"));
  testa_lista(l, 6, s_("b"), s_("a"), s_("A"), s_("B"), s_("C"), s_("c"));
  ls_final(l);
  ls_insere_depois(l, s_("d"));
  testa_lista(l, 7, s_("b"), s_("a"), s_("A"), s_("B"), s_("C"), s_("c"), s_("d"));

  ls_destroi(l);
}

void testa_remove()
{
  inicia_teste(__FUNCTION__);
  Lstr l = ls_cria();
  ls_insere_depois(l, s_("A"));
  ls_remove(l);
  testa_lista(l, 0);
  ls_insere_antes(l, s_("B"));
  ls_insere_antes(l, s_("C"));
  ls_insere_depois(l, s_("A"));
  ls_posiciona(l, -1);
  verifica_str(ls_remove(l), s_("B"));
  verifica_bool(!ls_item_valido(l));
  testa_lista(l, 2, s_("C"), s_("A"));
  ls_posiciona(l, 0);
  verifica_str(ls_remove(l), s_("C"));
  verifica_str(ls_remove(l), s_("A"));
  verifica_bool(ls_vazia(l));
  testa_lista(l, 0);

  ls_destroi(l);
}

void testa_arq()
{
  inicia_teste(__FUNCTION__);
  str abacaxi = s_("abacaxi\U0001f34d");
  str abacate = s_("abacate🥑");
  str tomate  = s_("tomate\xf0\x9f\x8d\x85");
  str s = s_copia(s_(""));
  s_cat(&s, abacaxi);
  s_cat(&s, s_("  \n "));
  int p = s_tam(s);
  s_cat(&s, s_(", \n"));
  s_subst(&s, p, 0, abacate, s_(""));
  s_cat(&s, tomate);
  verifica_str(s, s_("abacaxi\U0001f34d  \n abacate🥑, \ntomate\xf0\x9f\x8d\x85"));
  s_grava_arquivo(s, s_("dados"));
  str f = s_le_arquivo(s_("dados"));
  Lstr l = s_separa(f, s_("\n"));
  for (ls_inicio(l); ls_avanca(l); ) {
    ls_insere_antes(l, s_apara(ls_remove(l), s_(", ")));
  }
  testa_lista(l, 3, abacaxi, abacate, tomate);
  ls_destroi(l);
  s_destroi(f);
  s_destroi(s);
}

int main()
{
  testa_insere();
  testa_remove();
  testa_arq();
}
