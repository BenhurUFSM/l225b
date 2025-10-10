#include "bits.h"
#include "tela.h"
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>

int main()
{
  tela_cria();
  double antes = tela_relogio();
  double t_fps = antes;
  double frames = 0;
  int fps = 0;
  tela_seleciona_cursor(invisivel);
  Bits canvas = b_cria(600, 150);
  Bits e[] = { b_cria_arq("im/r4"), b_cria_arq("im/r5") };
  Bits cac = b_cria_arq("im/c2");
  Bits chao1 = b_cria_arq("im/h1");
  Bits chao2 = b_cria_arq("im/h2");
  Bits d[] = {
    b_cria_arq("im/d0"), b_cria_arq("im/d1"), b_cria_arq("im/d2"), b_cria_arq("im/d3"),
    b_cria_arq("im/d4"), b_cria_arq("im/d5"), b_cria_arq("im/d6"), b_cria_arq("im/d7"),
    b_cria_arq("im/d8"), b_cria_arq("im/d9"),
  };
  int t_dino = 0;
  float d_dino = 0;
  float v_dino = 150;
  float vy_dino = 0;
  float a_dino = 9.7;
  float grav = 150;
  float y_dino = 75;
  int i_cac = 350;
  while (d_dino < 3000) {
    double agora = tela_relogio();
    if (agora - t_fps >= 1) {
      fps = frames / (agora - t_fps);
      t_fps += 1;
      frames = 0;
    }
    frames++;
    float delta = agora - antes;
    antes = agora;
    t_dino += delta * 1000;
    if (t_dino >= 400) t_dino -= 400;
    d_dino += v_dino * delta;
    v_dino += a_dino * delta;
    if (v_dino > 500) v_dino = 500;
    int p_cac = i_cac - d_dino;
    if (i_cac - d_dino < -10) i_cac += 600;
    y_dino += vy_dino * delta;
    if (y_dino < 25) vy_dino = 0;
    if (vy_dino > 0) y_dino += vy_dino * delta;
    vy_dino += grav * delta;
    if (y_dino > 75) { y_dino = 75; vy_dino = 0;}
    if (d_dino > 100 && y_dino == 75 && rand()%30000 < delta*10000) vy_dino = -150;
    b_limpa(canvas);
    b_desenha(canvas, 30, y_dino, e[(int)(d_dino/10)%2]);
    b_desenha(canvas, -d_dino, 110, chao1);
    b_desenha(canvas, -d_dino+600, 110, chao2);
    b_desenha(canvas, -d_dino+1200, 110, chao2);
    b_desenha(canvas, -d_dino+1800, 110, chao2);
    b_desenha(canvas, -d_dino+2400, 110, chao1);
    b_desenha(canvas, p_cac, 85, cac);
    int n = fps;
    for (int i = 0; i < 4; i++) {
      b_desenha(canvas, 580 - 12*i, 5, d[n%10]);
      n /= 10;
    }
    //tela_limpa();
    tela_lincol(1, 1);
    tela_cor_fundo(50,50,50);
    tela_cor_letra(255,125,0);
    b_imprime_uni_2x3(canvas);
    //b_imprime_ascii(canvas);
    fflush(stdout);
    //usleep(5000);
    //if (b_colide(e[t_dino/200], p_cac-30, 85-y_dino, cac)) break;
  }
  tela_destroi();
}

