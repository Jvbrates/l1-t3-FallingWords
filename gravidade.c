// Programa de exemplo de uso de tela.h e tela.c
//
// Necessita os arquivos exemplo3.c (este), tela.h, tela.c e DejaVuSans.ttf
// Necessita ter instalado a biblioteca allegro5
//
// para compilar este programa manualmente em linux:
// gcc -Wall -o exemplo3 exemplo3.c tela.c -lallegro_font -lallegro_color -lallegro_ttf -lallegro_primitives -lallegro
//

// Este exemplo é um programa de colorir círculos na tela.
// Existem dois conjuntos de círculos desenhados na tela:
// - um fixo no topo da tela, que serve para escolher cores
// - um móvel, que podem ser coloridos
// Clicando em um dos círculos de cores se seleciona a cor corrente
// Clicando em um dos círculos móveis se altera a cor do círculo

#include "tela.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// definição dos tipos de dados
// -----------------------------
#include "gravidade.h"


void grav_test(obj *o1, obj * o2);
void atualiza_obj(obj *o);
void print_obj(obj o, int x, int y);
// A função principal
/*
int main(void)
{

  // inicializa a tela gráfica
  tela_inicio(700, 700, "exemplo");
  obj o1 = {
    {
        9, -4
    },
    {
      0, 0
    },
    {
      50, 250
    },
    5

};

  obj o2 = {
    {
        -3, 1
    },
    {
      0, 0
    },
    {
      450, 50
    },
    5000

};

  obj o3 = {
    {
        0, 0
    },
    {
      0, 0
    },
    {
      450, 350
    },
    1000

};
  int c = 0;
  while (true) {
    o1.aceleracao.x = 0;
    o1.aceleracao.y = 0;
    o2.aceleracao.x = 0;
    o2.aceleracao.y = 0;
    o3.aceleracao.x = 0;
    o3.aceleracao.y = 0;
    grav_test(&o1, &o2);
    grav_test(&o1, &o3);
    grav_test(&o2, &o1);
    grav_test(&o2, &o3);
    //grav_test(&o3, &o1);
    //grav_test(&o3, &o2);
    
    atualiza_obj(&o1);
    atualiza_obj(&o2);
    // /atualiza_obj(&o3);
    
    tela_circulo(350, 350, 300, 0, amarelo, amarelo);
    tela_circulo(o1.posicao.x, o1.posicao.y, 5, 0, verde, verde);
    tela_circulo(o2.posicao.x, o2.posicao.y, 5, 0, verde, vermelho);
    tela_circulo(o3.posicao.x, o3.posicao.y, 5, 0, branco, branco);
    print_obj(o1, 20, 20);
    tela_atualiza();
  }

  // encerra a tela gráfica
  tela_fim();

  return 0;
}

*/

// funções auxiliares
// -------------------
//
void atualiza_obj(obj *o){
    if(o->velocidade.x + o->aceleracao.x < 10 && o->velocidade.x + o->aceleracao.x > -10){o->velocidade.x += o->aceleracao.x;}
    if(o->velocidade.y + o->aceleracao.y < 10  && o->velocidade.y + o->aceleracao.y > -10){o->velocidade.y += o->aceleracao.y;}
    
    //o->velocidade.y += o->aceleracao.y;
    //o->velocidade.y += o->aceleracao.y;
    
    o->posicao.x += o->velocidade.x;
    o->posicao.y += o->velocidade.y;
}

float aleat_entre(int m, int M)
{

    return (rand()%(M-m+1)+m); //TODO Modifique para usar float .2f
}


void draw_vector(int o_x, int o_y, vetor v){
    tela_linha(o_x, o_y, o_x+v.x, o_y+v.y, 2, verde);
    tela_circulo(o_x+v.x-5, o_y+v.y, 5, 2, transparente, branco);
}

double dist(vetor v1,vetor v2){
  double x = v1.x - v2.x;
  double y = v1.y - v2.y;
  

  return sqrt(pow(x,2)+pow(y,2));
}

void print_obj(obj o, int x, int y){
  char c[80];
  sprintf(c, "pos x:%.f", o.posicao.x);
  tela_texto_dir(x, y, 18, verde, c);
  sprintf(c, "pos y:%2.f", o.posicao.y);
  tela_texto_dir(x, y+20, 18, verde, c);
  

  sprintf(c, "ac x:%.9f", o.aceleracao.x);
  tela_texto_dir(x, y+40, 18, verde, c);
  sprintf(c, "ac y:%.9f", o.aceleracao.y);
  tela_texto_dir(x, y+60, 18, verde, c);

  sprintf(c, "vel x:%.9f", o.velocidade.x);
  tela_texto_dir(x, y+80, 18, verde, c);
  sprintf(c, "vel y:%.9f", o.velocidade.y);
  tela_texto_dir(x, y+100, 18, verde, c);
}

void grav_test(obj *o1, obj * o2){
  double constG = 6.6;

  double cos = ((o2->posicao.x-o1->posicao.x)/dist(o1->posicao, o2->posicao));
  double sen = ((o2->posicao.y-o1->posicao.y)/dist(o1->posicao, o2->posicao));

  double pm = o1->massa*o2->massa;
  double pd = pow(dist(o1->posicao, o2->posicao),2);
  printf("%lf|%lf-", o2->posicao.x-o1->posicao.x, cos);
  o1->aceleracao.x += (cos*constG*pm/pd)/o1->massa;
  o1->aceleracao.y += (sen*constG*pm/pd)/o1->massa;
};

vetor aleat_radius(int width, int height, float radius){ //Retorna um vetor com posições aleatorias dentro de um raio a partir do centro

    vetor obj = {aleat_entre(0, width), aleat_entre(0, height)};

    vetor centro = {height/2, width/2};

    double d = dist(obj, centro);

    double q = radius/d;
    printf("Q: %.2f\n", q);

    return (vetor){centro.x+(obj.x-width/2)*q,
                   centro.y+(obj.y-height/2)*q};


}