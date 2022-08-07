//João Vitor Belmonte Rates
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tela.h"
#include "gravidade.h"//usa math
#include "buttons.h"

#define WIDTH 700.0f //Largura da Tela
#define HEIGHT 700.0f //Altura da Tela
#define bool _Bool
#define N_SCR 5 //Numero de melhores pontuações gravadas

//Convenção nome_var nomeFunction structName struct_enumname enumname_indice  //FIXME Aplique isto depois

//Receberão as poisções do mouse
float mouse_x, mouse_y;
char tecla, tecla_buffer, tst;//Para evitar que uma tecla seja lida várias vezes em uma "teclada"

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
    float err;
} resultado_partida;
typedef struct {
    resultado_partida scores[5];
    int level;//TODO Estou usando?
} scoreboard;
typedef struct {
    bool clock;//Usado na função verTimestamp após x instervalo de tempo o clock é ativado
    double init;
    bool reset;
} timestamp;//Usado para tempos
typedef struct {
    int grau, nivel, ip, set_p, n_p;//Grau, nivel, indice palavras, setted_palavra, numero de palavras
    int typ, w_typ;//Types and Wrong types
    double time;
    resultado_partida result;
    partida_estados estado;
    palavra *palavras;
    timestamp *timers;
} partida;
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
void init_game(Game * g);//Define as varivaeis iniciais do jogo
void handlerGame(Game * Gp);

//Explicação normalizapal()
/*Na primeira que vez que o programa for executado ele buscará pelos arquvios de palavras com nome npal..
quando não encontrar ele vai criar os arquivos, são semelhantes aos arquivos pal normais com a diferença
 de que todas as linhas tem o mesmo numero de bytes, por exemplo a linha que continha "a" contem "a        ",
 assim, sabendo o numero de bytes de cada linha, o programa usa o comando fseek para mover o ponteiro e
 selecionar a palavra escolhida aleatoriamente, fiz isto porque julguei ser mais eficiênte que usar vários
 fscanf*/
void normalizapal(char *f1, char *f2);


void bfDraw(Game );
//O jogo conta com os estados, menu, quit, play e save_score

obj centro = {
{0,0},
{0,0},
    {WIDTH/2, HEIGHT/2},
    2000
};

//Indice timers(timestamps)
//0 -- Periodicamente ativa uma palavra da lista em espera.
//1 -- É ativado após alguns segundos (útil em tela de "prepare-se" e impedir que uma letra seja digitada sem querer ao entrar em um estado).
//2 -- Alternando clock entre true e false em um ciclo __/¨¨\__/¨¨\__,
//3 -- Cronometra o tempo de partida


int main(){

    normalizapal("pal-al.txt", "npal-al.txt");
    normalizapal("pal-qp.txt", "npal-qp.txt");
    normalizapal("pal-zm.txt", "npal-zm.txt");



    srand(time(0));


    //Inicia a tela
    tela_inicio(WIDTH, HEIGHT, "l1_t3_Jvrates");
    //Cria a estrutura do jogo e define seus valores iniciais

    Game jogo;
    init_game(&jogo);


    while (jogo.run) {

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

//Preenche um array como uma string char1 = char2
void stringtoarray(char *array, const char *string, int length ){
    for(int i = 0; i < length; i++){
        array[i] = string[i];
    }
    array[length] = '\0';
}

//Recupera a pontuação contida no arquivo, caso ele exista
void file_to_score(scoreboard *sc_board, char *filename){
    //Isto limpa a varivavel que armazena os nomes score_lvl1 e a status msg
    //Isto será usado em drawScore
    for(int i = 0; i < N_SCR; i++){
        sc_board->scores[i].nome[0] = '\0';
    }

    //Iste trecho preeenche os valores da scoreboard
    FILE *file;
    file = fopen(filename, "r");


    if(file == NULL){
        printf("Arquivo inexistente");
    }
    else {
        int verifica, y = 0;

        char nome[500];//Precisa ser grande, par ao caso de não haver nome, o numero de palavras por minutos tenta ser armazenado no char, se ele for muito pequeno ocorre um erro de overflow

        do{
            float ppm, pontos, err;
            verifica = fscanf(file, "%s |%f|%f|%f",nome, &ppm, &err, &pontos);
            if(verifica == 4){
                stringtoarray(sc_board->scores[y].nome, nome, 3);
                sc_board->scores[y].err = err;
                sc_board->scores[y].score = pontos;
                sc_board->scores[y].ppm = ppm;
                y++;//Conta o numero de scores registrados
            }

        }while(!feof(file) && verifica == 4 && y!=5);
        if(verifica != 4){
            printf("Sintxe do arquivo incorreta");
        }
    }

}
//Move a pontuação na memória para o arquivo
void score_to_file(scoreboard *sc_board, char *filename){
    FILE *file;
    file = fopen(filename, "w");
    for (int i = 0; sc_board->scores[i].nome[0] != '\0'; ++i) {
        fprintf(file, "%s |%f|%f|%f\n",sc_board->scores[i].nome, sc_board->scores[i].ppm, sc_board->scores[i].err, sc_board->scores[i].score);
    }
    fclose(file);
}

//Funções Draw
void drawScore(scoreboard sc_board, double x, double y, char *title){//TODO corrigir nome variavel
    int linha_y = 0;
    tela_texto(x, y-15, 10, verde, title);
    tela_texto(x, y, 10, verde, "      PPM | ERR | PTOS");
    for(; linha_y<5; linha_y++){

        if(sc_board.scores[linha_y].nome[0] == '\0'){
            char etext[1500];
            sprintf(etext, "%i.%s",linha_y+1, "EMPTY                 ");
            tela_texto(x, y+15+(20.0f*(float)linha_y+1), 10, verde, etext);
        } else{
            char text[1500];
            sprintf(text, "%i.%3s %05.2f %05.2f %05.2f",linha_y+1, sc_board.scores[linha_y].nome,  sc_board.scores[linha_y].ppm, sc_board.scores[linha_y].err, sc_board.scores[linha_y].score);
            tela_texto(x, y+15+(20.0f*(float)linha_y+1), 10, verde, text);
        }
    }


    tela_retangulo(x - 125, y-25, x+125, y+15+(20.0f*(float)linha_y+1), 3, verde, transparente);
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
    sprintf(score, "%.2lf | %.2lf | %.2lf", p.result.ppm, p.result.err, p.result.score);
    tela_texto(WIDTH / 2, 180, 14, branco, score);
    char nivel[100], grau[100];

    sprintf(grau,"GRAU %i", p.estado==partida_pause?p.grau:p.grau+1);
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
            sprintf(t, "(%.0lf)", 5 - (relogio() - p.timers[1].init));
            tela_texto(WIDTH / 2, 220, 14, branco, t);


            if(p.timers[2].clock == true){
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
            if(p.timers[2].clock == true) {
                tela_texto(WIDTH / 2, 200, 14, branco, "Press space to continie");
            }
            break;
        }
        case partida_gameOver:{

            drawStatus(p);
            tela_texto(WIDTH / 2, 250, 14, branco, "LOSE GAME");
            if(p.timers[2].clock == true) {
                tela_texto(WIDTH / 2, 200, 14, branco, "Press any key to continie");
            }
            break;
        }
    }

}
//Draw Principal
void bfDraw(Game g) {
    

    iterable_f(g.btn_array, g.btn_array_lenght, draw_button);

    switch (g.estado) {
    case menu:{
        tela_texto(WIDTH/2, HEIGHT/2 - 300, 24, verde, "L1-T3-Jvrates");
        drawScore(g.sc_board[0], HEIGHT/2    , 180, "SCOREBOARD LVL 1");
        drawScore(g.sc_board[1], HEIGHT/2 - 200    , 340, "SCOREBOARD LVL 2");
        drawScore(g.sc_board[2], HEIGHT/2 + 200    , 340, "SCOREBOARD LVL 3");

        //TODO Adicionar Tooltip Music
    }break;
    case play:{

        drawPlay(g.partida1);
    }break;
    case save_score:{
        tela_texto(WIDTH/2, HEIGHT/2 - 300, 24, verde, "Save your score!");
        tela_texto(WIDTH/2, HEIGHT/2 - 250, 24, verde, "Type your name:");
        drawScore(g.sc_board[g.partida1.nivel-1], WIDTH/2, HEIGHT/2 , "Scoreboard");
    }break;
    case level:{
        tela_texto(WIDTH/2, HEIGHT/2 - 300, 24, verde, "Select level");
        break;
    }
    case quit:{

    }break;
    }
}

//Configura os botões
void setBtnLevel(Game *Gp){
    free(Gp->btn_array);
    Gp->btn_array = malloc(4* sizeof(button));
    Gp->btn_array_lenght = 4;
    Gp->btn_array[0] = init_button( WIDTH/2 - 52, 200, 104, 50, "Level 1", 12, 1, verde, verde, branco);
    Gp->btn_array[1] = init_button( WIDTH/2 - 52, 300, 104, 50, "Level 2", 12, 2, amarelo, amarelo, branco);
    Gp->btn_array[2] = init_button( WIDTH/2 - 52, 400, 104, 50, "Level 3", 12, 3, vermelho, vermelho, branco);
    Gp->btn_array[3] = init_button( 48, 600, 104, 50, "Voltar", 12, -1, verde, verde, branco);
}
void setBtnMenu(Game *Gp) {
    free(Gp->btn_array);
    Gp->btn_array = malloc(2 * sizeof(button));
    Gp->btn_array_lenght = 2;
    Gp->btn_array[0] = init_button(WIDTH / 2 - 250, 500, 100, 50, "Start", 12, 3, verde, verde, branco);
    Gp->btn_array[1] = init_button(WIDTH / 2 + 150, 500, 100, 50, "QUIT", 12, 4, vermelho, vermelho, branco);
}

//Funções usadas para administrar os timestamps
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
void cron(timestamp *T, double *r){

    if(T->clock == true){
        //TODO observe aqui
        *r = relogio()-T->init;

    } else {

        T->init = relogio()-*r;
    }



}

//Funções inicialização de estruturas
void init_game(Game *g){
    g->estado = menu;
    g->run = true;

    //Isto preenche o placar
    file_to_score(&(g->sc_board[0]), "score_lvl1");
    //Lvl2
    file_to_score(&(g->sc_board[1]), "score_lvl2");
    //Lvl3
    file_to_score(&(g->sc_board[2]), "score_lvl3");

    //Isto define os botoes do menu
    g->btn_array = NULL;
    setBtnMenu(g);

}
void initPartida(partida *p){//Zera os valores da variavel partida//TODO revisar
    p->estado = load;
    p->result.nome[0] = '\0';
    p->result.err = 0.0;
    p->result.ppm = 0.0;
    p->result.score = 0;
    p->palavras = NULL;
    p->timers = malloc(5*sizeof(timestamp));
    initTimestamp(&p->timers[1]);
    p->grau = 0;
    p->set_p = -1;
    p->typ = 0;
    p->w_typ = 0;
    p->n_p = 0;
    p->time = 0;
}


//Explicação na main
void normalizapal(char *f1, char *f2){
    FILE *file;
    FILE  *nfile;
    file = fopen(f2, "r");



    if(file == NULL){
        printf("Dicionario de palavras padronizado não existe, gerando arquivo");

        file = fopen(f1, "r");
        nfile = fopen(f2, "w");
        char pal[24];
        while(!feof(file)) {
            fscanf(file, "%s", pal);
            for (int i = 0; i < 24; ++i) {
                if (pal[i] == '\0') {
                    for (int j = i; j < 24; j++) {
                        pal[j] = ' ';
                    }
                    break;

                }
            }
            pal[23] = '\n';

            fprintf(nfile, "%s", pal);
            printf("%s", pal);
        }

        fclose(nfile);
        fclose(file);
    }
    else {
        fclose(file);
    }

}

//Retorna uma palavra aleatoria com base no nivel
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

//Retorna um ponteiro para o vetor de palavras da rodada com base no nível e no grau
palavra * loadPalavras(int grau, int nivel){

    palavra *pal  = malloc((grau*6)*sizeof(palavra));

    for (int i = 0; i < grau*6; ++i) {
        aleatPalavra(pal[i].palavra, nivel);

        //Uma posição aleatoria no raio de 200 do centro
        pal[i].fisica.posicao = aleat_radius(WIDTH, HEIGHT, 300);

        //Estado inicial , ainda não afeta o game
        pal[i].estado = sleep;
        pal[i].fisica.massa = 10.0;//Implementações físicas

        //Velocidade inicial || Aleatoriedade
        pal[i].fisica.velocidade.x = aleat_entre(-20,20)/10;
        pal[i].fisica.velocidade.y = aleat_entre(-20,20)/10;

        //Testes pra deixar mais legal

    }

    return pal;
}

//Move todos os caracteres uma posição para a esquerda e joga o primeiro caractere fora
void shiftLeftString(char *string){
    for (int i = 0; string[i]!='\0' ; ++i) {
        string[i] = string[i+1];
    }
}

//Movimenta as palavras de forma análoga a lei da gravitação (Se não houvesse velocidade máxima eles poderiam entrar em orbita)
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

            //Velocidade Máxima || diminui pela metade
            if (p[i].fisica.velocidade.x + p[i].fisica.aceleracao.x > (grau*3)) {
                p[i].fisica.velocidade.x = (grau*3)*0.5;

            }
            if (p[i].fisica.velocidade.x + p[i].fisica.aceleracao.x < -(grau*3)) {
                p[i].fisica.velocidade.x = -(grau*3)*0.5;

            }
            if (p[i].fisica.velocidade.y + p[i].fisica.aceleracao.y > (grau*3)) {
                p[i].fisica.velocidade.y = (grau*3)*0.5;

            }
            if (p[i].fisica.velocidade.y + p[i].fisica.aceleracao.y < -(grau*3)) {
                p[i].fisica.velocidade.y = -(grau*3)*0.5;

            }
        }
    }
}

//Detecta estados especificos da partida
bool endRodada(partida  p){
    for (int i = 0; i < p.grau*6; ++i) {
        if(p.palavras[i].estado == active || p.palavras[i].estado == sleep){
            return false;
        }
    }
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


//Autoexplicativa
void atualizaScore(partida *p){
    p->result.err = p->typ!=0?(100.0/p->typ)*p->w_typ:0;
    p->result.score = p->typ - p->w_typ;//Pontução é o numero de letras corretas
    p->result.ppm = p->n_p/(p->time/60);
}

//Pega o resultado da partida e grava na scoreboard caso esteja entre os melhores
int gravaScore(resultado_partida r, scoreboard *scb){
    for (int i = 0; i < N_SCR; ++i) {
        printf("|%c|\n",scb->scores[i].nome[0]);
        if(scb->scores[i].nome[0] == '\0' || r.score > scb->scores[i].score){
            //TODO corrigir grau

            //Move as colocações em 1 para baixo
            for (int j = N_SCR-1; j > i; j--) {
                scb->scores[j].ppm = scb->scores[j-1].ppm;
                scb->scores[j].score = scb->scores[j-1].score;
                scb->scores[j].err = scb->scores[j-1].err;
                stringtoarray(scb->scores[j].nome, scb->scores[j-1].nome, 3);
            }

            //Grava a nova pontuação
            scb->scores[i].ppm = r.ppm;
            scb->scores[i].score = r.score;
            scb->scores[i].err = r.err;
            stringtoarray(scb->scores[i].nome, "***", 3);

            return i;
        }
    }
    return -1;
}

//Gerencia as estradas do teclado e seus efeitos sobre as palavras, a pontuação e os estados da partida (pause)
void inputPartida(partida *p){

    //printf("%c - %c - %c\n", tecla, tecla_buffer, tst);
    //printf("%i - %i\n", p->typ, p->w_typ);
    if(tecla == ' '){
        //tela_fim();
        p->estado = partida_pause;
        atualizaScore(p);
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
                p->n_p++;
            }
        }

        if(p->set_p == -1){
            p->w_typ++;
        }
    }
}

//Funções principais de fluxo
void handlerPlay(partida *p){

    char teste[500];
    initTimestamp(&p->timers[3]);
    if(p->estado!=playing){
        p->timers[3].clock = false;
    } else {
        p->timers[3].clock = true;
    }
    cron(&p->timers[3], &p->time);
    sprintf(teste, "%.4lf", p->time);
    tela_texto(50,50, 10, verde,teste);

    switch (p->estado) {

        case partida_pause:{
            if(tecla == ' '){
                p->estado = playing;
            }
            initTimestamp(&p->timers[2]);
            clockTimestamp(&p->timers[2], 0.75);//Alternando clock entre true e false*/
        break;}
        case playing: {
            //Isto vai ser grande

            initTimestamp(&(p->timers[0]));
            verTimestamp(&(p->timers[0]),4-(p->grau));

            fisicaPalavras(p->palavras, p->grau);
            inputPartida(p);
            //Aos poucos libera as palavras
            if(p->ip < p->grau*6 && p->timers[0].clock == true){
                p->palavras[p->ip].estado = active;
                p->ip++;//A próxima palavra da lista será ativada no próximo loop
                p->timers[0].reset = false;
                p->timers[0].clock = false;
            }
            if(endRodada(*p)){

                atualizaScore(p);
                p->estado = load;

            }
            if(gameOver(*p)){
                p->timers[1].reset = false;
                atualizaScore(p);
                p->estado = partida_gameOver;
                p->grau--;

            }
            break;}
        case load: {

            initTimestamp(&(p->timers[1]));
            initTimestamp(&(p->timers[2]));
            verTimestamp(&p->timers[1], 5);//Após 5 segundos timers[1].clock = true
            clockTimestamp(&p->timers[2], 0.75);//Alternando clock entre true e false em um ciclo de 0.75 segundos

            //Após 5 segundos a rodada inicia
            if(p->timers[1].clock == true){
                p->grau<3?p->grau++:0;


                free(p->palavras);//Libera a lista anterior de palavras na memoria
                p->palavras = loadPalavras(p->grau, p->nivel);//Gera a lista de palavras e devolve o ponteiro
                p->ip = 0; //Identifica qual palavra da lista de ser ativada a seguir
                p->estado = playing;

                p->timers[1].reset = false;//Prepara para o próximo load (reset = false significa que será resetado)
            }

            break;}
        case partida_gameOver:{
            initTimestamp(&p->timers[2]);
            initTimestamp(&p->timers[1]);
            clockTimestamp(&p->timers[2], 0.75);//Alternando clock entre true e false*/
            verTimestamp(&p->timers[1], 2);//Após 2 segundos clock fica true
            if(p->timers[1].clock) {
                if (tecla != '\0') {
                    p->result.nome[0] = '*';//end
                    p->result.nome[0] = '*';
                    p->result.nome[0] = '*';
                    free(p->timers);
                }
            }
                break;
        }

    }


}
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
            //Estou usando o nome[0] como indicador de fim da partida
            if(Gp->partida1.result.nome[0] != '\0'){
                Gp->estado = save_score;
                Gp->partida1.grau = 0;//Vou reaproveitar as variaveis em save_score
                Gp->partida1.ip = 0;
                Gp->partida1.set_p = 0;
                printf("Saída para save score");
            }

        break;}
        case save_score: {

            //Verifica se a pontuação está elegivel a ser registrada;
            if(Gp->partida1.grau == 0){
                Gp->partida1.ip = gravaScore((Gp->partida1.result), &Gp->sc_board[Gp->partida1.nivel-1]);
                Gp->partida1.grau = 1;
                printf("PASSOU AQUI");//Mas aqui sim
            }
            if(Gp->partida1.ip != -1){
                //Registra o nome
                if(Gp->partida1.set_p < 3){

                    if(tecla != '\0' && tecla != '\n' && tecla != '\b' && tecla != ' '){
                        Gp->sc_board[Gp->partida1.nivel-1].scores[Gp->partida1.ip].nome[Gp->partida1.set_p++] = tecla;
                    }

                }
                else {
                    char *c[3] = {
                            "score_lvl1",
                            "score_lvl2",
                            "score_lvl3"
                    };

                    Gp->partida1.ip = -1;
                    //Nivel está começando em 1, por isso a todo momento uso nivel-1
                    printf("aaaaaa%s",c[Gp->partida1.nivel-1]);
                    score_to_file(&Gp->sc_board[Gp->partida1.nivel-1], c[Gp->partida1.nivel-1]);
                }
            }
            else {
                Gp->estado = menu;

                setBtnMenu(Gp);
            }

            break;}
        case level: {

            if(tela_rato_clicado()){

                if(Gp->mouse_btn_on < 0){
                    Gp->estado = menu;
                    setBtnMenu(Gp);
                }
                else if(Gp->mouse_btn_on > 0){
                    Gp->estado = play;


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


