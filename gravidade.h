#ifndef _gravity_
#define _gravity_

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


#endif