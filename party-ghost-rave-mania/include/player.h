#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

struct Player
{

    // Posição
    float x;
    float y;

    // Velocidade
    float vel_x;
    float vel_y;

    // Status
    bool andando;
    bool pulando;
    bool abaixado;
    bool vivo;
    int lado;
    bool atirando;
    bool mirando_cima;

    // Atributos
    int altura;
    int largura;
    int vida;
    int vida_max;
    int pontuacao;
    int municao;

    // Sprites e etc
    ALLEGRO_BITMAP* sprite_atual; // Ponteiro para a imagem/sprite atual do jogador.
    int frame_atual_animacao;    // Para controlar animações de sprites.
    int frame_timer;
    int frame_timer_andando;

    ALLEGRO_BITMAP* sprite_parado;
    ALLEGRO_BITMAP* sprite_pulando;
    ALLEGRO_BITMAP* sprite_andando;
    ALLEGRO_BITMAP* sprite_atirando;
    ALLEGRO_BITMAP* sprite_abaixado;
};

struct ALLEGRO_KEYBOARD_STATE;

struct Player *player_create(float x_inicial, float y_inicial);

void player_destroi(struct Player *jogador);

void player_input(struct Player *jogador, struct ALLEGRO_KEYBOARD_STATE *estado_teclado);

void player_atualiza(struct Player *jogador);

void desenha_player(struct Player *jogador, float x_tela, float y_tela);


void player_atualiza_na_arena(struct Player *jogador);

#endif