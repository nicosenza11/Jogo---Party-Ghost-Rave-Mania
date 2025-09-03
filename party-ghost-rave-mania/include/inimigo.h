#ifndef INIMIGO_H
#define INIMIGO_H

#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

struct Inimigo
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
    bool vivo;
    bool triggered;
    bool atirando;

    // Atributos
    int altura;
    int largura;
    int vida;
    int vida_max;

    // Sprites e etc
    ALLEGRO_BITMAP *sprite_atual; // Ponteiro para a imagem/sprite atual do jogador.
    int frame_atual_animacao;     // Para controlar animações de sprites.

    // Cooldowns
    float cooldown_tiro_total;
    float cooldown_tiro_restante;
    int direcao_tiro_inimigo;
    int frame_timer;

    ALLEGRO_BITMAP *sprite_tiro;
    ALLEGRO_BITMAP *sprite_inimigo_normal;
    ALLEGRO_BITMAP *sprite_inimigo_normal_atirando;
    ALLEGRO_BITMAP *sprite_inimigo_normal_derrotado;
    ALLEGRO_BITMAP *sprite_boss;
    ALLEGRO_BITMAP *sprite_boss_atirando;
    ALLEGRO_BITMAP *sprite_boss_derrotado;
};

struct ALLEGRO_KEYBOARD_STATE;

void inimigo_spawn(struct Inimigo *novo_inimigo, float x_inicial, float y_inicial, int type);

void inimigo_destroi(struct Inimigo *inimigo);

void inimigo_atualiza(struct Inimigo *inimigo, struct tiro array_projeteis_inimigos[], int max_projeteis, float offset_fundo_atual);

void desenha_inimigo(struct Inimigo *inimigo, float x_tela, float y_tela);

void boss_atualiza(struct Inimigo *inimigo, struct tiro array_projeteis_inimigos[], int max_projeteis);


#endif