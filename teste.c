//
// Created by jvbrates on 7/27/22.
//

void file_to_score(scoreboard *sc_board){
    //Limpa a varivavel que armazena os nomes score_lvl1 e a status msg
    //Isto será usado em drawScore

    for(int i = 0; i < N_SCR; i++){
        sc_board->scores[i].nome[0] = '\0';
    }
    sc_board->msg_status[0] = '\0';

    FILE *file;
    file = fopen("score_lvl1", "r");


    if(file == NULL){
        realloc(sc_board->msg_status, 21*sizeof (char));
        stringtoarray(sc_board->msg_status, "score_lvl1 file not found", 20);
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

        }while(!feof(file) && verifica == 4);

        if(verifica != 4){
            realloc(sc_board->msg_status, 26*sizeof (char));
            stringtoarray(sc_board->msg_status, "Syntax error in score_lvl1 file", 20);
        }
    }

}