#include "tela.h"
#include "buttons.h"
#include <stdio.h>
extern float mouse_x, mouse_y;

//Execuuta a função sobre todos os itens
int iterable_f(button btn[], int length, int (*f)(button)){
    int r;
    for(int i = 0; i<length; i++)
    {
        r = f(btn[i]);
        if( r != 0){
           
            break;
        }
    }    
    return r;
}

int iterable_fp(button btn[], int length, int (*f)(button *)){
    int r;
    for(int i = 0; i<length; i++)
    {
        r = f(&btn[i]);
        if( r != 0){
            
            break;
        }
    }    
    return r;
}

//Cribtn e retorna um button;
button init_button(int pos_x, int pos_y, int x, int y, char *texto, int lt, int id, int cor_borda, int cor_fundo, int cor_texto){
    button btn;
    btn.pos_x = pos_x;
    btn.pos_y = pos_y;
    btn.x = x;
    btn.y = y;
    btn.cor_borda = cor_borda;
    btn.cor_fundo = cor_fundo;
    btn.cor_texto = cor_texto;
    btn.texto = texto;
    btn.id = id;
    btn.lt = lt;
    return btn;
}

//Desenha o button
int draw_button(button btn){
    if(tela_rato_apertado() && mouse_on(btn) == btn.id){
        btn.x -= 20;
        btn.pos_x += 10;
        btn.y -= 10;
        btn.pos_y += 5;
    }

    tela_retangulo(btn.pos_x, btn.pos_y, btn.x+btn.pos_x, btn.pos_y+btn.y, 5, btn.cor_borda, btn.cor_fundo);
    tela_texto(btn.pos_x+((float)btn.x/2), btn.pos_y+((float)btn.y/2), btn.lt, btn.cor_texto, btn.texto);

    if(mouse_x>btn.pos_x && mouse_y>btn.pos_y && mouse_x<btn.pos_x+btn.x && mouse_y<btn.pos_y+btn.y){
        tela_texto_esq(btn.pos_x-10, btn.pos_y, btn.y, btn.cor_fundo, ">");

    }
    

    return 0;
}


//Enquanto o mouse estiver pressionado e em cima do botão. este é encolhido
int click_hold(button *btn){
    
    //printf("%i", btn->id);
    if(tela_rato_apertado() && mouse_x>btn->pos_x && mouse_y>btn->pos_y && mouse_x<btn->pos_x+btn->x && mouse_y<btn->pos_y+btn->y){
        btn->x -= 20;
        btn->pos_x += 10;
        btn->y -= 10;
        btn->pos_y += 5;
    }
    return 0;
}

// Retorna o id do botão caso o mouse esteja em cima
int mouse_on(button btn){
    if(mouse_x>btn.pos_x && mouse_y>btn.pos_y && mouse_x<btn.pos_x+btn.x && mouse_y<btn.pos_y+btn.y){
        return btn.id;
        tela_texto_dir(btn.pos_x-10, btn.pos_y+10, btn.lt, btn.cor_texto, ">>>");
    }
    return 0;
}

//Atoa, desenha uma seta ">" quando o mouse está acima do botao
int atoa(button btn){
    if(mouse_x>btn.pos_x && mouse_y>btn.pos_y && mouse_x<btn.pos_x+btn.x && mouse_y<btn.pos_y+btn.y){
        tela_texto_esq(btn.pos_x-10, btn.pos_y, btn.y, btn.cor_fundo, ">");
        return 0;
    }
    return 0;
}

