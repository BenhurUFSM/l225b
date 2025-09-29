#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "str.h"
#include "lstr.h"
#include "fila-prio.h"

// que horas são?
double agora()
{
  return time(0);
}

// chuta um número
int aleat_entre(int ini, int fim) {
  return rand() % (fim - ini + 1) + ini;
}

// chuta um número entre 1 e 1000, com mais chances de ser pequeno que grande
int chuta_pontos()
{
  return aleat_entre(1, 10) * aleat_entre(1, 10) * aleat_entre(1, 10);
}

// dado para colocar na fila, com uma palavra e o número de pontos que ela vale
typedef struct {
  str palavra;
  int pontos;
} palpon_t;

// função de comparação para prioridade entre palpons
// considera que quem vale menos pontos tem maior prioridade
int compara_palpon(void *p1, void *p2)
{
  palpon_t *pp1 = p1;
  palpon_t *pp2 = p2;
  return pp2->pontos - pp1->pontos;
}

// estrutura para manter o estado do jogo
typedef struct {
  str conteudo_do_arquivo;
  Lstr palavras;
  FilaPrio fila_de_palavras;
  palpon_t palavra_escolhida;
  str palavra_sendo_digitada;
  int pontos;
  bool terminou;
  double inicio;
} *Jogo;

// aloca e inicializa o estado do jogo (mas não de uma partida)
Jogo j_cria(str nome_do_arquivo)
{
  Jogo self = malloc(sizeof(*self));
  assert(self != NULL);
  self->conteudo_do_arquivo = s_le_arquivo(nome_do_arquivo);
  self->palavras = s_separa(self->conteudo_do_arquivo, s_("\n"));
  self->fila_de_palavras = NULL;
  return self;
}

// libera a memória ocupada pelo estado do jogo
void j_destroi(Jogo self)
{
  s_destroi(self->conteudo_do_arquivo);
  ls_destroi(self->palavras);
  fp_destroi(self->fila_de_palavras);
  free(self);
}

// pega a próxima palavra da fila (ou termina se não tem mais)
void j_escolhe_palavra(Jogo self)
{
  if (fp_tam(self->fila_de_palavras) == 0) {
    self->terminou = true;
  } else {
    fp_remove(self->fila_de_palavras, &self->palavra_escolhida);
  }
}

// vê se o jogo deve ser terminado por tempo
void j_verifica_tempo(Jogo self)
{
  if (agora() - self->inicio > 30) {
    self->terminou = true;
  }
}

// insere mais uma palavra na fila (aleatória)
void j_sorteia_palavra(Jogo self)
{
  int p = aleat_entre(0, ls_tam(self->palavras) - 1);
  palpon_t sorteada;
  ls_posiciona(self->palavras, p);
  sorteada.palavra = ls_item(self->palavras);
  sorteada.pontos = chuta_pontos() * s_tam(sorteada.palavra);
  fp_insere(self->fila_de_palavras, &sorteada);
}

// inicia uma partida
void j_inicia(Jogo self)
{
  fp_destroi(self->fila_de_palavras);
  self->fila_de_palavras = fp_cria(sizeof(palpon_t), compara_palpon);
  self->pontos = 0;
  self->terminou = false;
  self->inicio = agora();
  for (int i = 0; i < 10; i++) {
    j_sorteia_palavra(self);
  }
}

int main()
{
  srand(time(0));
  Jogo jogo = j_cria(s_("palavras"));
  j_inicia(jogo);

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
