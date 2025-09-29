#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "str.h"
#include "lstr.h"
#include "fila-prio.h"

int aleat_entre(int ini, int fim) {
  return rand() % (fim - ini + 1) + ini;
}

int chuta_pontos()
{
  return aleat_entre(1, 10) * aleat_entre(1,10) * aleat_entre(1, 10);
}

typedef struct {
  str palavra;
  int pontos;
} palpon_t;

int compara_palpon(void *p1, void *p2)
{
  palpon_t *pp1 = p1;
  palpon_t *pp2 = p2;
  return pp2->pontos - pp1->pontos;
}

double agora()
{
  return time(0);
}

typedef struct {
  str conteudo_do_arquivo;
  Lstr palavras;
  FilaPrio fila_de_palavras;
  palpon_t palavra_escolhida;
  str palavra_sendo_digitada;
  int pontos;
  bool terminou;
  double inicio;
} jogo_t;

jogo_t *j_cria(str nome_do_arquivo)
{
  jogo_t *self = malloc(sizeof(*self));
  assert(self != NULL);
  self->conteudo_do_arquivo = s_le_arquivo(nome_do_arquivo);
  self->palavras = s_separa(self->conteudo_do_arquivo, s_("\n"));
  self->fila_de_palavras = fp_cria(sizeof(palpon_t), compara_palpon);
  self->pontos = 0;
  self->terminou = false;
  self->inicio = agora();
  return self;
}

void j_destroi(jogo_t *self)
{
  s_destroi(self->conteudo_do_arquivo);
  ls_destroi(self->palavras);
  fp_destroi(self->fila_de_palavras);
  free(self);
}

void j_escolhe_palavra(jogo_t *self)
{
  if (fp_tam(self->fila_de_palavras) == 0) {
    self->terminou = true;
  } else {
    fp_remove(self->fila_de_palavras, &self->palavra_escolhida);
  }
}

void j_verifica_tempo(jogo_t *self)
{
  if (agora() - self->inicio > 30) {
    self->terminou = true;
  }
}

void j_sorteia_palavra(jogo_t *self)
{
  int p = aleat_entre(0, ls_tam(self->palavras) - 1);
  palpon_t sorteada;
  ls_posiciona(self->palavras, p);
  sorteada.palavra = ls_item(self->palavras);
  sorteada.pontos = chuta_pontos();
  fp_insere(self->fila_de_palavras, &sorteada);
}

int main()
{
  srand(time(0));
  jogo_t *jogo = j_cria(s_("palavras"));

  for (int i = 0; i < 10; i++) {
    j_sorteia_palavra(jogo);
  }
  j_escolhe_palavra(jogo);
  while (!jogo->terminou) {
    s_imprime(jogo->palavra_escolhida.palavra);
    printf(" %d  pontos=%d  tempo=%.1f\n", jogo->palavra_escolhida.pontos, jogo->pontos, agora() - jogo->inicio);
    char digitado[100];
    scanf("%s", digitado);
    jogo->palavra_sendo_digitada = s_cria(digitado);
    if (s_igual(jogo->palavra_sendo_digitada, jogo->palavra_escolhida.palavra)) {
      jogo->pontos += jogo->palavra_escolhida.pontos;
    }
    j_escolhe_palavra(jogo);
    j_verifica_tempo(jogo);
  }

  printf("\n\nfim do jogo.\ntotal de pontos:%d\n", jogo->pontos);

  j_destroi(jogo);
}
