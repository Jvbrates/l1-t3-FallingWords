#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tela.h"
#include "gravidade.h"
#include "buttons.h"

#define WIDTH 700.0f //Largura da Tela
#define HEIGHT 700.0f //Altura da Tela

#define N_SCR 5 //Numero 
// TODO DRAW INPUT HANDLER
//Convenção nome_var nomeFunction structName struct_enumname enumname_indice  //FIXME Aplique isto

//Receberão as poisções do mouse
float mouse_x, mouse_y;

//Enums -- Contribuem para legibilidade do código
typedef enum {
    menu,
    play,
    save_score,
    quit
} game_estado;
typedef enum {
    input_start = 1,
    input_quit = 2
} input_menu;
typedef enum {
    partida_pause,
    partida_level1,
    partida_level2,
    partida_level3,
    partida_gameOver
} partida_estados;
//Fim Enums

//Estruturas
typedef struct {
    obj fisica;
    char palavra[24];
    int lifes;
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
} scoreboard;
typedef struct {
    bool run;
    int palavra_adr;
    int lifes;
    game_estado estado;
    scoreboard sc_board;
    button *btn_array;
    int btn_array_lenght;
    //TODO implement PPM accuracy 
} Game;
typedef struct {
    bool run;
    partida_estados estado;
    obj *palavras;
}partida;


//Funções
int aleat_entre(int m, int M);//Gera valor aleatorio entre min e máximo
void init_game(Game *g);//Define as varivaeis iniciais do jogo
void file_to_score(scoreboard *sc_board);
void draw_score();


//Draw
void draw_menu(button *);
bool despedidas();
void bfDraw(Game Gp);
//O jogo conta com os estaodos, menu, quit, playing e save_score

//Função auxiliar desenha uma grade para ajudar a posicionar a interface na tela
void lines();

int main(){
    srand(2);
    
    //Inicia a tela
    tela_inicio(WIDTH, HEIGHT, "Meu sistema não tem titlebar");
    
    //Cria a estrutura do jogo e define seus valores iniciais
    Game jogo;
    init_game(&jogo);

    //Variavel que armazenará a partida
    //partida partida_var;

    //Laço principal
    while (jogo.run) {

        //Posições do mouse gravadas somente uma vez por loop
        mouse_x = tela_rato_x();
        mouse_y = tela_rato_y();

        if(tela_rato_apertado()){

            lines();//Função auxiliar
        }

        bfDraw(jogo);

        //Desenha circulo central //TODO Referencia
        tela_circulo(WIDTH/2, HEIGHT/2, 50, 0, branco, azul);
        
        //Router estados do jogo
        switch (jogo.estado) {
            case menu: {
                

                /*input_menu input = menu_management();

                //Redirect menu input for states
                switch (input) {
                    case input_start:{

                    }break;
                    case input_quit:{
                        jogo.estado = quit;
                    }break;
                }*/

            } break;
            case play: {
                printf("fas");
            }break;
            case quit:{
                if(despedidas()){
                    tela_fim();
                    return 0;
                }
            }break;
            case save_score:{
            }break;
        
        }

        tela_circulo(mouse_x, mouse_y, 3, 1, rosa, rosa);
        tela_atualiza();
    }
    return 0;
}

int aleat_entre(int m, int M)
{
  return rand()%(M-m+1)+m;
}

//Funções inicialização de estrutura TODO Em tese está funcionando
void init_game(Game *g){
    g->estado = menu;
    g->run = true;

    //Isto preenche o placar
    g->sc_board.msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board));//TODO Feito em tese

    //Isto define os botoes do menu
    g->btn_array_lenght = 2;
    g->btn_array = malloc(2* sizeof(button));
    g->btn_array[0] = init_button( WIDTH/2 -150, 400, 100, 50, "Start", 12, 1, branco, verde, preto);
    g->btn_array[1] = init_button( WIDTH/2 +50, 400, 100, 50, "QUIT", 12, 2, branco, verde,branco);
}

//Preenche um array com uma string TODO Em tese está funcionando
void stringtoarray(char *array, const char *string, int length ){
    for(int i = 0; i < length; i++){
        array[i] = string[i];
    }
    array[length] = '\0';
}

//Preenche a scoreboard, e insere uma mensagemm status para a operação;
//TODO Em tese está funcionando
void file_to_score(scoreboard *sc_board){
    //Isto limpa a varivavel que armazena os nomes score e a status msg
    //Isto será usado em drawScore
    for(int i = 0; i < N_SCR; i++){
        sc_board->scores[i].nome[0] = '\0';
    }
    sc_board->msg_status[0] = '\0';

    //Iste trecho preeenche os valores da scoreboard
    FILE *file;
    file = fopen("score", "r");


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

input_menu menu_management(){
    tela_texto(WIDTH/2, 100, 12, verde, "l1-t3-jvbrates");
    button btn_menu[2] = {
    init_button((WIDTH/2)-50, 300, 100, 50, "Start", 12, 1, branco, verde, preto),
    init_button((WIDTH/2)-50, 400, 100, 50, "QUIT", 12, 2, branco, verde,branco),
    };

    
    iterable_fp(btn_menu, 2, click_hold);//Efeito de botão apertado
    int t = iterable_f(btn_menu, 3, mouse_on);//Retorna o id do botão caso mouse esteja em cima
                    
    if(t){//FIXME transforma em função
        tela_texto_esq(btn_menu[t-1].pos_x-10, btn_menu[t-1].pos_y, btn_menu[t-1].y, btn_menu[t-1].cor_fundo, ">");
    }

    if(tela_rato_clicado()){
        switch (t) {
        case 1:
            return input_start;
        break;
        case 2:
            return input_quit;
        break;
        }
    }
    draw_score();
    iterable_f(btn_menu, 2, draw_button);//Desenha os botoes
    return 0;
}

//AUX //TODO REMOVA
void lines(){
    for(int i=0; i<700; i+=20){
        tela_linha((float)i, 700, (float)i, 0, 1, branco);
        tela_linha(0, (float)i, 700, (float)i, 1, branco);
    }
}

void draw_score(scoreboard sc_board){//FIXME Arrumar isto daqui após ler score
    int y = 0;
    tela_texto(HEIGHT/2, 190, 10, verde, "SCOREBOARD");
    for(; y<5; y++){
        if(sc_board.scores[y].nome[0] == '\0'){
            break;
        } else{
            char text[1500];
            sprintf(text, "%i. %3s  %2.2f %2.2f %2.2f",y+1, sc_board.scores[y].nome, sc_board.scores[y].score, sc_board.scores[y].accuracy, sc_board.scores[y].ppm);
            tela_texto(HEIGHT/2, 205.0f+(20.0f*(float)y+1), 10, verde, text);
        }
    }
    y++;
    if(sc_board.msg_status[0] != '\0'){
        tela_texto(HEIGHT/2, 205.0f+(20.0f*(float)y+1), 10, verde, sc_board.msg_status);
        y++;
    }
    tela_retangulo(HEIGHT / 2 - 140, 180, 490, 205.0f+(20.0f*(float)y+1), 3, verde, transparente);
}

//Função que gerencia todo o draw
void bfDraw(Game Gp) { //Gp Game pointer, not Gangplank
    
    //Desenha os bottoes na tela | Criar uma função para uma linha seria redundante
    iterable_f(Gp.btn_array, Gp.btn_array_lenght, draw_button);
    iterable_f(Gp.btn_array, Gp.btn_array_lenght, atoa);//Desenha uma setinha sobre o botão
    //Este switch vai aparecer diversas vezez
    switch (Gp.estado) {
    case menu:{
        tela_texto(WIDTH/2, HEIGHT/2 - 300, 24, verde, "L1-T3-Jvrates");
        draw_score(Gp.sc_board);

        //TODO Adicionar Tooltip Music
    }break;
    case play:{

    }break;
    case quit:{
        despedidas();
    }break;
    case save_score:{

    }break;
    }
}

