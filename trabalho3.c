#include <time.h>
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
char tecla, tecla_buffer, tst;//

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
    bool clock;//Usado na função verTimestamp após x instervalo de tempo o clock é ativado
    double init;
    bool reset;
} timestamp;
typedef struct {
    int grau, nivel, ip, set_p;//Grau, nivel, indice palavras, setted_palavra
    int typ, w_typ;//Types and Wrong types
    double time;
    resultado_partida result;
    partida_estados estado;
    palavra *palavras;
    timestamp *timers;
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


//Funções
int aleat_entre(int m, int M);//Gera valor aleatorio entre min e máximo
void init_game(Game *g);//Define as varivaeis iniciais do jogo
void handlerGame(Game *Gp);
void verTimestamp(timestamp *t, double standby);
void initTimestamp(timestamp * t);


bool despedidas();
void bfDraw(Game );
//O jogo conta com os estados, menu, quit, play e save_score

obj centro = {
{0,0},
{0,0},
    {WIDTH/2, HEIGHT/2},
    2000
};

//TESTES
//Função auxiliar desenha uma grade para ajudar a posicionar a interface na tela
void lines();

//FIM TESTES
int main(){


    srand(time(0));

    //Inicia a tela
    tela_inicio(WIDTH, HEIGHT, "l1_t3_Jvrates");

    //Cria a estrutura do jogo e define seus valores iniciais
    Game jogo;
    init_game(&jogo);


    while (jogo.run) {
        //printf("%i -- %i\n", jogo.estado, jogo.partida1.estado);

        //Posições do mouse gravadas somente uma vez por loop
        mouse_x = tela_rato_x();
        mouse_y = tela_rato_y();
        tecla_buffer = tst;
        tst = tela_tecla();

        //Feito com intuito de atrasar a leitura de varias vezes a mesma tecla
        if( tst != tecla_buffer){
            tecla = tst;
        } else {
            tecla = '\0';
        }

        //Desenha circulo central
        tela_circulo(WIDTH/2, HEIGHT/2, 50, 0, branco, azul);

        handlerGame(&jogo);
        bfDraw(jogo);

        jogo.mouse_btn_on = iterable_f(jogo.btn_array, jogo.btn_array_lenght, mouse_on);

        //Desenha o mouse
        tela_circulo(mouse_x, mouse_y, 3, 1, rosa, rosa);
        tela_atualiza();
    }
    tela_fim();
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

void drawPalavra(palavra pal, int cort, int corc){
    if(pal.estado == active){
        tela_texto(pal.fisica.posicao.x, pal.fisica.posicao.y-10, 14, cort, pal.palavra);
        tela_circulo(pal.fisica.posicao.x, pal.fisica.posicao.y, 5, 1, corc, corc);
    }
}

void drawStatus(partida p){
    char score [200];
    tela_texto(WIDTH / 2, 160, 14, branco, "PPM | ERR | PTOS");
    sprintf(score, "%.2lf | %.2lf | %.2lf", p.result.ppm, p.result.accuracy, p.result.score);
    tela_texto(WIDTH / 2, 180, 14, branco, score);
    char nivel[100], grau[100];

    sprintf(grau,"GRAU %i", p.grau+1);
    tela_texto(WIDTH / 2, 120, 14, branco, grau);

    sprintf(nivel,"Nivel %i", p.nivel);
    tela_texto(WIDTH / 2, 100, 14, branco, nivel);

}
void drawPlayng(partida p){
    for(int i = 0; i<p.grau*6; i++){
        if(i == p.set_p){
            drawPalavra(p.palavras[i], amarelo, vermelho);
        } else {
            drawPalavra(p.palavras[i], verde, verde);
        }

    }
}
void drawPlay(partida p){
    switch (p.estado) {
        case load:{
            drawStatus(p);
            char t[20];
            sprintf(t, "(%.0lf)", 5 - (relogio() - p.timers[2].init));
            tela_texto(WIDTH / 2, 220, 14, branco, t);


            if(p.timers[3].clock == true){
                tela_texto(WIDTH/2, 200, 14, branco, "Prepare-se");
            }
            break;

        }
        case playing:{

            drawPlayng(p);
            break;
        }
        case partida_pause:{
            drawPlayng(p);
            drawStatus(p);
            if(p.timers[3].clock == true) {
                tela_texto(WIDTH / 2, 200, 14, branco, "Press space to continie");
            }
            break;
        }
        case partida_gameOver:{
            drawPlayng(p);
            drawStatus(p);
            tela_texto(WIDTH / 2, 300, 14, branco, "LOSE GAME");
            if(p.timers[3].clock == true) {
                tela_texto(WIDTH / 2, 200, 14, branco, "Press any key to continie");
            }
            break;
        }
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

        //TODO Adicionar Tooltip Music
    }break;
    case play:{

        drawPlay(g.partida1);
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

//Funções inicialização de estrutura
void init_game(Game *g){
    g->estado = menu;
    g->run = true;

    //Isto preenche o placar
    g->sc_board[0].msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board[0]), "score_lvl1");//TODO Feito em tese
    //Lvl2
    g->sc_board[1].msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board[1]), "score_lvl2");//TODO Feito em tese
    //Lvl3
    g->sc_board[2].msg_status = malloc(2*sizeof (char) );
    file_to_score(&(g->sc_board[2]), "score_lvl3");//TODO Feito em tese
    g->btn_array = NULL;
    //Isto define os botoes do menu
    setBtnMenu(g);

}
void initPartida(partida *p){//Zera os valores da variavel partida//TODO revisar
    p->estado = load;
    p->result.nome[0] = '\0';
    p->result.accuracy = 0.0;
    p->result.ppm = 0.0;
    p->result.score = 0;
    p->palavras = NULL;
    p->timers = malloc(7*sizeof(timestamp));
    initTimestamp(&p->timers[2]);
    initTimestamp(&p->timers[3]);
    p->grau = 0;
    p->set_p = -1;
    p->typ = 0;
    p->w_typ = 0;
}

//Retorna uma palavra aleatoria
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
    }

    FILE *file;
    printf("X : %i", x);
    if(x <= 93) {
        file = fopen("npal-al.txt", "r");
        fseek(file, x*24, SEEK_SET);
        fscanf(file, "%s", pal);

    } else if (x <= 30051){
        file = fopen("npal-qp.txt", "r");
        fseek(file, (x-94)*24, SEEK_SET );
        fscanf(file, "%s", pal);
    } else{
        file = fopen("npal-zm.txt", "r");
        fseek(file, (x-30052)*24 ,SEEK_SET );
        fscanf(file, "%s", pal);
    }
    fclose(file);


}

//Cria o vetor de palavras;
palavra * loadPalavras( int  grau, int nivel){

    palavra *pal  = malloc((grau*6)*sizeof(palavra));

    for (int i = 0; i < grau*6; ++i) {
        aleatPalavra(pal[i].palavra, nivel);

        //Uma posição aleatoria no raio de 200 do centro
        pal[i].fisica.posicao = aleat_radius(WIDTH, HEIGHT, 300);

        //Estado inicial , ainda não afeta o game
        pal[i].estado = sleep; //TODO AQUI
        pal[i].fisica.massa = 10.0;//Implementações físicas

        //Velocidade inicial || Aleatoriedade
        pal[i].fisica.velocidade.x = aleat_entre(-20,20)/10;
        pal[i].fisica.velocidade.y = aleat_entre(-20,20)/10;

        //Testes pra deixar mais legal

    }

    return pal;
};

void initTimestamp(timestamp *t){
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
void clockTimestamp(timestamp *t, double ciclo){
    if(relogio()-t->init > ciclo){
        t->clock = !(t->clock);
        t->init = relogio();
    }
}
void fisicaPalavras(palavra *p, int grau){
    //FÍSICA

    for (int i = 0; i < grau*6; ++i) {

        if(p[i].estado == active) {
            atualiza_obj(&(p[i].fisica), grau*3);
            grav_test(&(p[i].fisica), centro);


            //Não preciso excluir os objetos que sairem da tela se eles nao sairem da tela
            if (p[i].fisica.posicao.x > WIDTH || p[i].fisica.posicao.x < 0) {
                p[i].fisica.velocidade.x *= -1;
                p[i].fisica.aceleracao.x *= -1;
            }
            if (p[i].fisica.posicao.y > HEIGHT || p[i].fisica.posicao.y < 0) {
                p[i].fisica.velocidade.y *= -1;
                p[i].fisica.aceleracao.y *= -1;
            }

            //Velocidade Máxima
            if (p[i].fisica.velocidade.x + p[i].fisica.aceleracao.x > (grau*3)) {
                p[i].fisica.velocidade.x = (grau*3);

            }
            if (p[i].fisica.velocidade.x + p[i].fisica.aceleracao.x < -(grau*3)) {
                p[i].fisica.velocidade.x = -(grau*3);

            }
            if (p[i].fisica.velocidade.y + p[i].fisica.aceleracao.y > (grau*3)) {
                p[i].fisica.velocidade.y = (grau*3);

            }
            if (p[i].fisica.velocidade.y + p[i].fisica.aceleracao.y < -(grau*3)) {
                p[i].fisica.velocidade.y = -(grau*3);

            }
        }
    }
}
void shiftLeftString(char *string){
    for (int i = 0; string[i]!='\0' ; ++i) {
        string[i] = string[i+1];
    }
}
void inputPartida(partida *p){

    //printf("%c - %c - %c\n", tecla, tecla_buffer, tst);
    //printf("%i - %i\n", p->typ, p->w_typ);
    if(tecla == ' '){
        //tela_fim();
        p->estado = partida_pause;
    } else if(tecla != '\n' && tecla != '\0'){
        //printf("set_p %i --- %c\n", p->set_p, tecla);
        p->typ++;
        if(p->set_p == -1){

            for (int i = 0; i < p->grau*6   ; ++i) {

                if(p->palavras[i].estado == active && p->palavras[i].palavra[0] == tecla){

                    p->set_p = i;
                    break;
                }
            }
        }
        //Exclui a letra da palavra
        if(p->set_p != -1 && p->palavras[p->set_p].estado == active){
            if(p->palavras[p->set_p].palavra[0] == tecla) {
                shiftLeftString(p->palavras[p->set_p].palavra);
            } else {
                p->w_typ++;
            }
            if(p->palavras[p->set_p].palavra[0] == '\0' || p->palavras[p->set_p].palavra[0] == '\n' ){
                p->palavras[p->set_p].estado = dead;
                p->set_p = -1;
            }
        }

        if(p->set_p == -1){
            p->w_typ++;
        }
    }
}
bool endRodada(partida  p){
    for (int i = 0; i < p.grau*6; ++i) {
        if(p.palavras[i].estado == active || p.palavras[i].estado == sleep){
            return false;
        }
    }
    printf("Fim da rodada");
    return true;
}
bool gameOver(partida p){
    for (int i = 0; i < p.grau*6; ++i) {
        if(dist(p.palavras[i].fisica.posicao,centro.posicao) < 50 ){
            return true;
        }
    }
    return false;
}
void atualizaScore(partida *p){
    p->result.accuracy = p->typ!=0?(100.0/p->typ)*p->w_typ:0;
    p->result.score = p->typ - p->w_typ;//Pontução é o numero de letras corretas
    p->result.ppm = p->typ/(p->time/60);
}

void cron(timestamp *T, timestamp *T2, double *r){
    if(T->clock == true){

        *r = relogio()-T->init+T2->init;
        T2->reset = false;
    } else {
        T2->init = relogio()-T2->init;
    }



}
void handlerPlay(partida *p){

    char teste[500];
    initTimestamp(&p->timers[2]);
    initTimestamp(&p->timers[6]);
    if(p->estado!=playing){
        p->timers[2].clock = false;
    } else {
        p->timers[2].clock = true;
    }
    cron(&p->timers[2], &p->timers[6], &p->time);
    double pause;
    sprintf(teste, "%.4lf", p->time);
    tela_texto(50,50, 10, verde,teste);




    switch (p->estado) {

        case partida_pause:{
            if(tecla == ' '){
                p->estado = playing;
            }
            initTimestamp(&p->timers[3]);
            clockTimestamp(&p->timers[3], 0.75);//Alternando clock entre true e false*/
        break;}
        case playing: {
            //Isto vai ser grande

            initTimestamp(&(p->timers[1]));
            verTimestamp(&(p->timers[1]),4-(p->grau));

            fisicaPalavras(p->palavras, p->grau);
            inputPartida(p);
            //Aos poucos libera as palavras
            if(p->ip < p->grau*6 && p->timers[1].clock == true){
                p->palavras[p->ip].estado = active;
                p->ip++;
                p->timers[1].reset = false;
                p->timers[1].clock = false;
            }
            if(endRodada(*p)){

                atualizaScore(p);
                p->timers[2].reset = false;
                p->estado = load;

            }
            if(gameOver(*p)){
                p->timers[2].reset = false;
                atualizaScore(p);
                p->estado = partida_gameOver;

            }
            break;}
        case load: {

            initTimestamp(&(p->timers[2]));
            initTimestamp(&(p->timers[3]));
            verTimestamp(&p->timers[2], 5);//Após 5 segundos clock fica true
            clockTimestamp(&p->timers[3], 0.75);//Alternando clock entre true e false

            //Após 5 segundos a rodada inicia
            if(p->timers[2].clock == true){
                p->grau<3?p->grau++:0;
                free(p->palavras);
                p->palavras = loadPalavras(p->grau, p->nivel);
                p->ip = 0;
                p->estado = playing;
                p->timers[5].reset = false;
                p->timers[2].reset = false;
            };

            break;}
        case partida_gameOver:{
            initTimestamp(&p->timers[3]);
            initTimestamp(&p->timers[2]);
            clockTimestamp(&p->timers[3], 0.75);//Alternando clock entre true e false*/
            verTimestamp(&p->timers[2], 2);//Após 5 segundos clock fica true
            if(p->timers[2].clock) {
                if (tecla != '\0') {
                    p->result.nome[0] = 'e';//end
                }
            }
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

