#ifndef TIRO_H
#define TIRO_H

#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

struct tiro
{

    // Posição
    float x;
    float y;

    // Velocidade
    float vel_x;
    float vel_y;

    // Status
    bool ativo;
    int lado;

    // Atributos
    int altura;
    int largura;
    int dano;

    // Sprites e etc
    ALLEGRO_BITMAP *sprite;   // Ponteiro para a imagem/sprite atual do jogador.
    int frame_atual_animacao; // Para controlar animações de sprites.
};

struct ALLEGRO_KEYBOARD_STATE;

void atira(struct tiro *projetil, float x_inicial, float y_inicial, int direcao, int type, ALLEGRO_BITMAP *sprite_player, ALLEGRO_BITMAP *sprite_inimigo);

void tiro_atualiza(struct tiro *projetil, int largura_tela, int altura_tela, float offset_x_mundo);

void desenha_tiro(struct tiro *projetil, float x_tela, float y_tela);

void tiro_atualiza_na_arena(struct tiro *projetil, int largura_tela, int altura_tela);

#endif