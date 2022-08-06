#include <stdio.h>
#include <stdlib.h>


typedef struct item {
    int valor;
    struct item * next;
} li;

int aleat_entre(int m, int M)
{

    return (rand()%(M-m+1)+m); //TODO Modifique para usar float .2f
}
typedef struct {
    struct item * primeiro;
}lista;




void add(lista *listaitem){
    li *nitem = malloc(sizeof (li));
    nitem->valor = aleat_entre(0,100);
    nitem->next = listaitem->primeiro;
    listaitem->primeiro = nitem;
}

void rec_p(li *liv){
    printf("|%i|", liv->valor);
    if(liv->next != NULL){
        rec_p(liv->next);
    }
};
int main(){

    lista l;
    l.primeiro = malloc(sizeof (li));
    l.primeiro->next = NULL;
    add(&l);
    add(&l);
    add(&l);
    add(&l);
    rec_p(l.primeiro);
    printf("Hello world");
    return 0;
}
