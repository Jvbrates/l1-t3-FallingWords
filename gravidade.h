//João Vitor Belmonte Rates
#ifndef _gravidade_
#define _gravidade_

typedef struct{
    double x;
    double y;
} vetor;

typedef struct {
    vetor velocidade;
    vetor aceleracao;
    vetor posicao;
    double massa;
} obj;


vetor aleat_radius(int width, int height, float radius);
void grav_test(obj *o1, obj o2);
void atualiza_obj(obj *o, int vmax);
double dist(vetor v1,vetor v2);
int aleat_entre(int m, int M);
#endif