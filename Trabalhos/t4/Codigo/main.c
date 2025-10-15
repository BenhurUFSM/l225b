#include "rede.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
  srand(time(0));
  //srand(0);
  Rede rede = NULL;
  if (argc > 1) {
    rede = rede_cria(argv[1]);
  }
  int pontos = joga_com_tela(rede);
  printf("pontos: %d\n", pontos);
  rede_destroi(rede);
}

