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
#endif