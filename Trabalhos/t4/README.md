# t4 - dino

Faça uma implementação de grafos, implementando em `grafo.c` a interface definida em `grafo.h`.
Implemente também os algoritmos de detecção de ciclos e ordem topográfica que estão descritos.
Implemente também lista e fila, de acordo com os respectivos `.h`.

Os demais arquivos fornecidos implementam um jogo do dinossauro, fortemente baseado do que é implementado no navegador chrome.
Esse jogo pode ser jogado manualmente ou por uma rede neural, que é implementada em `rede.c` e `rede.h`, usando o grafo que você deve implementar.
Uma rede é fornecida, no arquivo `rede-exemplo`.

Tem também um programa para treinar uma rede para jogar esse jogo.

O arquivo `compila` tem os comandos de compilação para gerar esses dois programas, usando o compilador `gcc`.

O programa `main` joga o jogo. Se for executado com `./main`, joga manualmente, comandado pelas teclas `espaço` ou `seta para cima` e `seta para baixo`. Tem ainda `q` para terminar o programa quando ele estiver parado.

Se o programa for executado com `./main rede-exemplo`, vai executar controlado pela rede nesse arquivo.

O programa `treina` faz o treinamento de redes, partindo de uma geração de redes vazias alteradas aleatoriamente. Essas redes são usadas para executar o jogo (sem tela), e as que obtiverem melhor pontuação são usadas para produzir uma nova geração de redes.
A melhor de cada geração é gravada em um arquivo no diretório `G` (que deve ser criado).

## RAP

1. Como funciona `fila_proximo`?

   Se a fila `f` contiver A, B, C:

   Se for chamado `fila_inicia_percurso(f, 0)`, e depois for chamado `fila_proximo(f, p)` várias vezes:
   - a primeira chamada vai colocar A em *p e retornar true;
   - a segunda chamada vai colocar B em *p e retornar true;
   - a terceira chamada vai colocar C em *p e retornar true;
   - a quarta, quinta etc chamadas vão retornar false.

   Se for chamado fila_inicia_percurso(f, -2), e depois for chamado fila_proximo(f, p) várias vezes:
   - a primeira chamada vai colocar B em *p e retornar true;
   - a segunda chamada vai colocar A em *p e retornar true;
   - a terceira, quarta, quinta etc chamadas vão retornar false.
