#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tela.h"
#include "gravidade.h"
#include "buttons.h"

#define WIDTH 700.0f //Largura da Tela
#define HEIGHT 700.0f //Altura da Tela
#define bool _Bool
#define N_SCR 5 //Numero

//Convenção nome_var nomeFunction structName struct_enumname enumname_indice  //FIXME Aplique isto

//Receberão as poisções do mouse
float mouse_x, mouse_y;

//Enums -- Contribuem para legibilidade do código
typedef enum {
    menu,
    play,
    save_score,
    level,
    quit,
} game_estado;
typedef enum {
    partida_pause,
    playing,
    partida_gameOver,
    load,
} partida_estados;
typedef enum {
    sleep,
    active,
    dead
} palavra_estado;
//Fim Enums

//Structs
typedef struct {
    obj fisica;
    char palavra[24];
    palavra_estado estado;
} palavra;
typedef struct {
    char nome[10];
    float score;
    float ppm;
    float accuracy;
}resultado_partida;
typedef struct {
    resultado_partida scores[5];
    char *msg_status;
    int level;
} scoreboard;
typedef struct {
    bool clock;
    double init;
    bool reset;
} timestamp;
typedef struct {
    resultado_partida result;
    partida_estados estado;
    palavra *palavras;
    timestamp *timers;
    int grau;
    int nivel;
}partida;
typedef struct {
    bool run;
    game_estado estado;
    scoreboard sc_board[3];
    button *btn_array;
    int btn_array_lenght;
    int mouse_btn_on; //Retorna o id em que está o mouse
    partida partida1;
} Game;


//Fim strutucts

//Funções
int aleat_entre(int m, int M);//Gera valor aleatorio entre min e máximo
void init_game(Game *g);//Define as varivaeis iniciais do jogo
void handlerGame(Game *Gp);
void verTimestamp(timestamp *t, double standby);
void initTimestamp(timestamp * t);
//void drawScore(); TODO Quem sabe remover


bool despedidas();
void bfDraw(Game );
//O jogo conta com os estados, menu, quit, play e save_score


//TESTES
//Função auxiliar desenha uma grade para ajudar a posicionar a interface na tela
void lines();

//FIM TESTES
int main(){


    srand(2);

    //Inicia a tela
    tela_inicio(WIDTH, HEIGHT, "l1_t3_Jvrates");

    //Cria a estrutura do jogo e define seus valores iniciais
    Game jogo;
    init_game(&jogo);


    while (jogo.run) {
        printf("%i -- %i\n", jogo.estado, jogo.partida1.estado);

        //Posições do mouse gravadas somente uma vez por loop
        mouse_x = tela_rato_x();
        mouse_y = tela_rato_y();

        //Desenha circulo central
        tela_circulo(WIDTH/2, HEIGHT/2, 50, 0, branco, azul);

        handlerGame(&jogo);
        bfDraw(jogo);

        jogo.mouse_btn_on = iterable_f(jogo.btn_array, jogo.btn_array_lenght, mouse_on);

        //Desenha o mouse
        tela_circulo(mouse_x, mouse_y, 3, 1, rosa, rosa);
        tela_atualiza();
    }
    return 0;
}


void stringtoarray(char *array, const char *string, int length ){
    for(int i = 0; i < length; i++){
        array[i] = string[i];
    }
    array[length] = '\0';
}

//Preenche a scoreboard, e insere uma mensagemm status para a operação;
//TODO Em tese está funcionando
void file_to_score(scoreboard *sc_board, char *filename){
    //Isto limpa a varivavel que armazena os nomes score_lvl1 e a status msg
    //Isto será usado em drawScore
    for(int i = 0; i < N_SCR; i++){
        sc_board->scores[i].nome[0] = '\0';
    }
    sc_board->msg_status[0] = '\0';

    //Iste trecho preeenche os valores da scoreboard
    FILE *file;
    file = fopen(filename, "r");


    if(file == NULL){
        printf("Debug");
        free(sc_board->msg_status);
        sc_board->msg_status = malloc(21*sizeof (char));
        stringtoarray(sc_board->msg_status, "score file not found", 20);

    } else {
        int verifica, y = 0;

        char nome[5];

        do{
            float ppm, pontos, accuracy;
            verifica = fscanf(file, "%s |%f|%f|%f",nome, &pontos, &ppm, &accuracy);
            if(verifica == 4){
                stringtoarray(sc_board->scores[y].nome, nome, 3);
                sc_board->scores[y].accuracy = accuracy;
                sc_board->scores[y].score = pontos;
                sc_board->scores[y].ppm = ppm;
                y++;//Conta o numero de scores registrados
            }

        }while(!feof(file) && verifica == 4 && y!=5);
        if(verifica != 4){
            free(sc_board->msg_status);
            sc_board->msg_status = malloc(30*sizeof (char));
            stringtoarray(sc_board->msg_status, "Syntax error in score file", 30);
        }
    }

}

bool despedidas(){
    return true;
}//FIXME implement


void initTimestamp(timestamp * t){
    if(!(t->reset)) {

        t->clock = false;
        t->init = relogio();
        t->reset = !(t->reset);
    }
}
void verTimestamp(timestamp *t, double standby){
    if(relogio()-t->init > standby){
        t->clock = true;

    }
}


//AUX //TODO REMOVA
void lines(){
    for(int i=0; i<700; i+=20){
        tela_linha((float)i, 700, (float)i, 0, 1, branco);
        tela_linha(0, (float)i, 700, (float)i, 1, branco);
    }
}

void drawScore(scoreboard sc_board, double x, double y_, char *title){//TODO corrigir nome variavel
    int y = 0;
    tela_texto(x, y_, 10, verde, title);
    for(; y<5; y++){
        if(sc_board.scores[y].nome[0] == '\0'){
            break;
        } else{
            char text[1500];
            sprintf(text, "%i.%3s %2.2f %2.2f %2.2f",y+1, sc_board.scores[y].nome, sc_board.scores[y].score, sc_board.scores[y].accuracy, sc_board.scores[y].ppm);
            tela_texto(x, y_+15+(20.0f*(float)y+1), 10, verde, text);
        }
    }

    if(sc_board.msg_status[0] != '\0'){
        tela_texto(x, y_+15+(20.0f*(float)y+1), 10, verde, sc_board.msg_status);
        y++;
    }
    tela_retangulo(x - 125, y_-10, x+125, y_+15+(20.0f*(float)y+1), 3, verde, transparente);
}

void drawPalavra(palavra pal){
    if(pal.estado == active){
        tela_texto(pal.fisica.posicao.x, pal.fisica.posicao.y-10, 14, branco, pal.palavra);
        tela_circulo(pal.fisica.posicao.x, pal.fisica.posicao.y, 5, 1, verde, verde);
    }
}
void drawPlay(partida p){
    if(p.grau != 0){
        for (int i = 0; i < 7; ++i) {

            //printf("%s\n",p.palavras[0].palavra);
            drawPalavra(p.palavras[i]);
        };
    }
};

//Função que gerencia todo o draw
void bfDraw(Game g) {
    

    iterable_f(g.btn_array, g.btn_array_lenght, draw_button);

    switch (g.estado) {
    case menu:{
        tela_texto(WIDTH/2, HEIGHT/2 - 300, 24, verde, "L1-T3-Jvrates");
        drawScore(g.sc_board[0], HEIGHT/2    , 200, "SCOREBOARD LVL 1");
        drawScore(g.sc_board[1], HEIGHT/2 - 200    , 340, "SCOREBOARD LVL 2");
        drawScore(g.sc_board[2], HEIGHT/2 + 200    , 340, "SCOREBOARD LVL 3");
        //drawScore(g.sc_board[0], HEIGHT/2+130    , 190, "SCOREBOARD");

        //TODO Adicionar Tooltip Music
    }break;
    case play:{

        printf("AA");
        if(g.partida1.estado == playing){printf("%lf\n", g.partida1.palavras[4].fisica.posicao.x);
            drawPlay(g.partida1);}
    }break;
    case save_score:{

    }break;
    case level:{
        tela_texto(WIDTH/2, HEIGHT/2 - 300, 24, verde, "Select level");
        break;
    }
    case quit:{
        despedidas();
    }break;
    }
}

//Configura os botoes
void setBtnLevel(Game *Gp){
    free(Gp->btn_array);
    Gp->btn_array = malloc(4* sizeof(button));
    Gp->btn_array_lenght = 4;
    Gp->btn_array[0] = init_button( WIDTH/2 - 52, 200, 104, 50, "Level 1", 12, 1, verde, verde, preto);
    Gp->btn_array[1] = init_button( WIDTH/2 - 52, 300, 104, 50, "Level 2", 12, 2, amarelo, amarelo, preto);
    Gp->btn_array[2] = init_button( WIDTH/2 - 52, 400, 104, 50, "Level 3", 12, 3, vermelho, vermelho, preto);
    Gp->btn_array[3] = init_button( 48, 600, 104, 50, "Voltar", 12, -1, verde, verde, preto);
}
void setBtnMenu(Game *Gp) {
    free(Gp->btn_array);
    Gp->btn_array = malloc(2 * sizeof(button));
    Gp->btn_array_lenght = 2;
    Gp->btn_array[0] = init_button(WIDTH / 2 - 250, 500, 100, 50, "Start", 12, 3, branco, verde, preto);
    Gp->btn_array[1] = init_button(WIDTH / 2 + 150, 500, 100, 50, "QUIT", 12, 4, branco, verde, branco);
}

//Funções inicialização de estrutura TODO Em tese está funcionando
void init_game(Game *g){
    g->estado = menu;
    g->run = true;

    //Isto preenche o placar
    //Lvl1
    g->sc_board[0].msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board[0]), "score_lvl1");//TODO Feito em tese
    //Lvl2
    g->sc_board[1].msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board[1]), "score_lvl2");//TODO Feito em tese
    //Lvl3
    g->sc_board[2].msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board[2]), "score_lvl3");//TODO Feito em tese

    //Isto define os botoes do menu
    setBtnMenu(g);

}
void initPartida(partida*p){//Zera os valores da variavel partida//TODO revisar
    p->result.nome[0] = '\0';
    p->result.accuracy = 0.0;
    p->result.ppm = 0.0;
    printf("Antes");
    p->timers = malloc(5*sizeof(timestamp));
    printf("Depois");
    p->grau = 1;
}

//TODO em tese funciona Retorna uma palavra aleatoria
void aleatPalavra(char *pal, int nivel){
    int x;
    switch (nivel) {
        case 1: {
            x = aleat_entre(0, 93);
            break;
        }
        case 2: {
            x = aleat_entre(0, 30051);
            break;
        }
        case 3: {
            x = aleat_entre(0, 245365);
            break;
        }
        //Só para parar de aparecer warnings
    }
    printf("X:%i", x);
    FILE *file;
    if(x <= 93) {
        file = fopen("npal-al.txt", "r");
        fseek(file, x*24, SEEK_SET);
        fscanf(file, "%s", pal);
        printf("==>%s", pal);
    } else if (x <= 30051){
        file = fopen("npal-qp.txt", "r");
        fseek(file, SEEK_SET, (x-94)*24);
        fscanf(file, "%s", pal);
    } else{
        file = fopen("npal-zm.txt", "r");
        fseek(file, SEEK_SET, (x-30052)*24);
        fscanf(file, "%s", pal);
    }
    fclose(file);


}

//TODO em tese está funcionando Cria o vetor de palavras;

//Um ponteiro para um ponteiro
void loadPalavras(palavra **pa, int  grau, int nivel){
    int quantidade = 2;
    printf("Pelo amor de deus");
    switch (grau) {
        case 1:{
            quantidade = 7;

            break;
        }
        case 2: {
            quantidade = 10;
            break;
        }
        case 3:{
            quantidade = 20;
            break;
        }
    }

    palavra *pal  = malloc(quantidade*sizeof(palavra));

    for (int i = 0; i < quantidade; ++i) {
        aleatPalavra(pal[i].palavra, nivel);

        //Uma posição aleatoria no raio de 200 do centro
        pal[i].fisica.posicao = aleat_radius(WIDTH, HEIGHT, 200);

        //Estado inicial , ainda não afeta o game
        pal[i].estado = active; //TODO AQUI
        pal[i].fisica.massa = 10.0;//Implementações físicas

        //Velocidade inicial || Aleatoriedade
        pal[i].fisica.velocidade.x = aleat_entre(0,20)/10;
        pal[i].fisica.velocidade.y = aleat_entre(0,20)/10;
        printf("||%p", pal);

    }
    *pa = pal;
};
void handlerPlay(partida *p){

    switch (p->estado) {

        case partida_pause:{
            tela_texto(23,23,23,branco,"AAAAA");
        break;}
        case playing: {
            break;}
        case load: {
                loadPalavras(&(p->palavras), p->grau, p->nivel);
                p->estado = playing;
            break;}
        case partida_gameOver:{
                break;
        }

    }

};

void handlerGame(Game *Gp){//Game pointer, not Ganglank
    switch (Gp->estado) {
        case menu: {
            if (tela_rato_clicado()) {
                Gp->estado = Gp->mouse_btn_on;
            }

            //Define os novos botoes para selecionar o level
            if(Gp->estado == level){
                setBtnLevel(Gp);
            }
            break;
        }
        case play: {

            handlerPlay(&Gp->partida1);
            if(Gp->partida1.result.nome[0] != '\0'){//Servirá como indicador de estar funcionando
                Gp->estado = save_score;
            }

        break;}
        case save_score: {
            break;}
        case level: {

            if(tela_rato_clicado()){

                if(Gp->mouse_btn_on < 0){
                    Gp->estado = menu;
                    setBtnMenu(Gp);
                }
                else if(Gp->mouse_btn_on > 0){
                    Gp->estado = play;

                    free(Gp->btn_array);
                    Gp->btn_array_lenght = 0;
                    Gp->partida1.nivel = Gp->mouse_btn_on;
                    Gp->partida1.estado = load;

                    initPartida(&Gp->partida1);
                }
            }
            break;}
        case quit:{
            Gp->run = false;
            break;
        }
    }
}

