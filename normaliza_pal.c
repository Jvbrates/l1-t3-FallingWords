//
// Created by jvbrates on 8/5/22.
//
#include <stdlib.h>
#include <stdio.h>
void normalizapal(char *f1, char *f2){
    FILE *file;
    FILE  *nfile;

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

    fclose(file);
    fclose(nfile);
}
int main(){
    normalizapal("pal-al.txt", "npal-al.txt");
    normalizapal("pal-qp.txt", "npal-qp.txt");
    normalizapal("pal-zm.txt", "npal-zm.txt");
    return 0;
}