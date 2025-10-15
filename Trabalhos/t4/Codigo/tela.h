// tela.h
// ------
// implementa uma abstração do terminal de texto
// trata caracteres lidos do teclado e controla a impressão na tela
//

#ifndef TELA_H
#define TELA_H

#include <stdbool.h>

// tipo de dados usado para representar uma tecla
// pode ser um código unicode ou valores especiais para representar teclas
//   que não representam caracteres
typedef enum {
  t_primeiro_unicode = 0,
  t_ctrl_a,
  t_ctrl_b,
  t_ctrl_c,
  t_ctrl_d,
  t_ctrl_e,
  t_ctrl_f,
  t_ctrl_g,
  t_ctrl_h,
  t_ctrl_i,
  t_ctrl_j,
  t_ctrl_k,
  t_ctrl_l,
  t_ctrl_m,
  t_ctrl_n,
  t_ctrl_o,
  t_ctrl_p,
  t_ctrl_q,
  t_ctrl_r,
  t_ctrl_s,
  t_ctrl_t,
  t_ctrl_u,
  t_ctrl_v,
  t_ctrl_w,
  t_ctrl_x,
  t_ctrl_y,
  t_ctrl_z,
  t_esc,
  t_ultimo_unicode = 0x10FFFF,
  // codificações especiais, para representar códigos que não são caracteres
  t_none,          // não tem caractere na entrada
  t_resize,        // a janela mudou de tamanho (não é um caractere digitado)
  // codificação de teclas que não representam caracteres
  t_enter,
  t_back,
  t_del,
  t_pgup,
  t_pgdn,
  t_home,
  t_end,
  t_insert,
  t_left,
  t_right,
  t_up,
  t_down,
} tecla;

// tipos de cursor
typedef enum { invisivel, bloco, barra, sublinha } tipo_cursor;

// inicializa a tela e o teclado
// a tela será mostrada fps vezes por segundo
void tela_cria(int fps);

// volta a tela ao normal
void tela_destroi(void);

// mostra o que foi desenhado na tela
void tela_mostra();

// limpa a tela
void tela_limpa(void);

// limpa do cursor ao final da linha
void tela_limpa_fim_da_linha(void);

// posiciona o cursor (0,0 é o canto superior esquerdo)
void tela_lincol(int lin, int col);

// retorna a altura da tela (número de linhas)
int tela_nlin(void);

// retorna a largura da tela (número de colunas)
int tela_ncol(void);

// cor normal para as próximas impressões
void tela_cor_normal(void);

// seleciona a cor das letras nas próximas impressões
void tela_cor_letra(int vermelho, int verde, int azul);

// seleciona a cor do fundo nas próximas impressões
void tela_cor_fundo(int vermelho, int verde, int azul);

// seleciona o tipo de cursor
void tela_seleciona_cursor(tipo_cursor tipo);

// retorna a próxima tecla da entrada, que pode ser um caractere unicode
//   ou um caractere especial de controle ou 't_none' se não houver
//   caractere disponível na entrada
tecla tela_le_tecla(void);

// retorna o número de segundos desde algum momento no passado
double tela_relogio(void);
#endif // TELA_H
