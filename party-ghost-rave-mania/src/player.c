#include "player.h"
#include "tiro.h"
#include "config.h"
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

struct Player *player_create(float x_inicial, float y_inicial)
{

    struct Player *novo_jogador = (struct Player *)malloc(sizeof(struct Player));

    if (novo_jogador == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria para o jogador!\n");
        return NULL; // Retorna NULL para indicar falha
    }

    novo_jogador->x = x_inicial;
    novo_jogador->y = y_inicial;

    novo_jogador->vel_x = 0.0f;
    novo_jogador->vel_y = 0.0f;

    novo_jogador->sprite_atual = al_load_bitmap("assets/principal1.png");

    novo_jogador->sprite_parado = al_load_bitmap("assets/principal1.png");
    novo_jogador->sprite_andando = al_load_bitmap("assets/principal2.png");
    novo_jogador->sprite_pulando = al_load_bitmap("assets/principal2.png");
    novo_jogador->sprite_atirando = al_load_bitmap("assets/principal_atirando.png");
    novo_jogador->sprite_abaixado = al_load_bitmap("assets/principal_abaixado.png");

    novo_jogador->frame_atual_animacao = 1;
    if (novo_jogador->sprite_atual == NULL)
    {
        fprintf(stderr, "Erro ao carregar o sprite do jogador em assets/principal1.png\n");

        free(novo_jogador);
        return NULL;
    }

    novo_jogador->largura = 256;
    novo_jogador->altura = 256;

    novo_jogador->andando = false;
    novo_jogador->pulando = false;
    novo_jogador->abaixado = false;
    novo_jogador->vivo = true;
    novo_jogador->atirando = false;
    novo_jogador->mirando_cima = false;

    novo_jogador->vida = 10;
    novo_jogador->vida_max = novo_jogador->vida;
    novo_jogador->lado = 1;
    novo_jogador->municao = 10;
    novo_jogador->pontuacao = 0;

    novo_jogador->frame_timer_andando = 15;

    return novo_jogador;
};

void player_destroi(struct Player *jogador)
{

    if (jogador == NULL)
    {
        fprintf(stderr, "Tentativa de destruir um jogador NULO.\n");
        return;
    }

    if (jogador->sprite_atual != NULL)
    {
        al_destroy_bitmap(jogador->sprite_atual);
        jogador->sprite_atual = NULL;
    }

    printf("Jogador destruido.\n");

    free(jogador);
}

void desenha_player(struct Player *jogador, float x_tela, float y_tela)
{

    if (jogador == NULL)
    {
        fprintf(stderr, "Erro: Tentativa de desenhar um jogador NULO em player_draw.\n");
        return;
    }

    int flag_lado = 0;

    if (jogador->lado == -1)
    {
        flag_lado = ALLEGRO_FLIP_HORIZONTAL;
    }

    if (jogador->sprite_atual != NULL)
    {
        al_draw_scaled_bitmap(jogador->sprite_atual, 0, 0, al_get_bitmap_width(jogador->sprite_atual), al_get_bitmap_height(jogador->sprite_atual), x_tela, y_tela, jogador->largura, jogador->altura, flag_lado);
    }
    else
    {
        al_draw_filled_rectangle(
            jogador->x,
            jogador->y,
            jogador->x + jogador->largura,
            jogador->y + jogador->altura,
            al_map_rgb(255, 0, 0));
    }
}

void player_input(struct Player *jogador, struct ALLEGRO_KEYBOARD_STATE *estado_teclado)
{

    if (jogador == NULL || estado_teclado == NULL)
    {
        return;
    }

    float velocidade_movimento = 5.0f; // velocidade do jogador
    float velocidade_de_pulo = -15.0f;
    float velocidade_de_pulo_arena = -24.0f;

    if (al_key_down(estado_teclado, ALLEGRO_KEY_LEFT) || al_key_down(estado_teclado, ALLEGRO_KEY_A))
    {
        jogador->vel_x = -velocidade_movimento;
        jogador->lado = -1;
        jogador->andando = true;
    }
    else if (al_key_down(estado_teclado, ALLEGRO_KEY_RIGHT) || al_key_down(estado_teclado, ALLEGRO_KEY_D))
    {
        jogador->vel_x = velocidade_movimento;
        jogador->lado = 1;
        jogador->andando = true;
    }
    else
    {
        jogador->vel_x = 0.0f;
        jogador->andando = false;
    }

    if (((al_key_down(estado_teclado, ALLEGRO_KEY_SPACE)) || al_key_down(estado_teclado, ALLEGRO_KEY_W)) && (jogador->pulando == false) && (jogador->abaixado == false))
    {
        if (jogador->altura == ALTURA_JOGADOR_ARENA)
            jogador->vel_y = velocidade_de_pulo_arena;
        else
            jogador->vel_y = velocidade_de_pulo;
        jogador->pulando = true;
    }

    if ((al_key_down(estado_teclado, ALLEGRO_KEY_UP)))
    {
        jogador->mirando_cima = true;
    }
    else
    {
        jogador->mirando_cima = false;
    }

    if ((al_key_down(estado_teclado, ALLEGRO_KEY_DOWN) || al_key_down(estado_teclado, ALLEGRO_KEY_S) || al_key_down(estado_teclado, ALLEGRO_KEY_LSHIFT)))
    {
        jogador->abaixado = true;
    }
    else
    {
        jogador->abaixado = false;
    }
}

void player_atualiza(struct Player *jogador)
{
    if (jogador->frame_timer_andando != 0)
        jogador->frame_timer_andando -= 1;

    if (jogador->frame_timer_andando == 0)
    {
        jogador->frame_atual_animacao++;
        jogador->frame_timer_andando = 15;
    }

    if (jogador->frame_atual_animacao > 1)
    {
        jogador->frame_atual_animacao = 0;
        jogador->frame_timer_andando = 15;
    }

    if (jogador == NULL)
    {
        return;
    }

    jogador->x += jogador->vel_x;
    jogador->y += jogador->vel_y;

    if (jogador->abaixado)
        jogador->altura = 198;
    else
        jogador->altura = jogador->largura;

    if (jogador->frame_timer != 0)
        jogador->frame_timer -= 1;

    if (jogador->frame_timer == 0)
    {
        jogador->atirando = false;
    }

    if (jogador->pulando == true)
    {
        jogador->vel_y += 0.8f; // gravidade
    }

    if ((jogador->abaixado == true) && (jogador->pulando == false))
        jogador->y = 756.0f - jogador->altura;
    else if ((jogador->abaixado == true) && (jogador->pulando == true))
        jogador->vel_y += 1.8f;

    if (jogador->y + jogador->altura >= 756.0f)
    {
        jogador->y = 756.0f - jogador->altura; // Alinha a BASE do jogador com o chão
        jogador->vel_y = 0.0f;
        jogador->pulando = false;
    }

    if (jogador->x < 0)
        jogador->x = 0;

    if (jogador->x + jogador->largura > LARGURA_TOTAL_MUNDO)
    {
        jogador->x = LARGURA_TOTAL_MUNDO - jogador->largura;
    }

    if (jogador->abaixado)
    {
        jogador->sprite_atual = jogador->sprite_abaixado;
    }
    else if (jogador->atirando)
    {
        jogador->sprite_atual = jogador->sprite_atirando;
    }
    else if (jogador->pulando)
    {
        jogador->sprite_atual = jogador->sprite_pulando;
    }
    else if (jogador->frame_atual_animacao == 0)
    {
        jogador->sprite_atual = jogador->sprite_parado;
    }
    else if (jogador->frame_atual_animacao == 1)
    {
        jogador->sprite_atual = jogador->sprite_andando;
    }
}

void player_atualiza_na_arena(struct Player *jogador)
{
    if (jogador->frame_timer_andando != 0)
        jogador->frame_timer_andando -= 1;

    if (jogador->frame_timer_andando == 0)
    {
        jogador->frame_atual_animacao++;
        jogador->frame_timer_andando = 15;
    }

    if (jogador->frame_atual_animacao > 1)
    {
        jogador->frame_atual_animacao = 0;
        jogador->frame_timer_andando = 15;
    }

    if (jogador == NULL)
    {
        return;
    }

    jogador->x += jogador->vel_x;
    jogador->y += jogador->vel_y;

    if (jogador->abaixado)
        jogador->altura = 99;
    else
        jogador->altura = jogador->largura;

    if (jogador->frame_timer != 0)
        jogador->frame_timer -= 1;

    if (jogador->frame_timer == 0)
    {
        jogador->atirando = false;
    }

    if (jogador->pulando == true)
    {
        jogador->vel_y += 0.8f; // gravidade
    }

    if ((jogador->abaixado == true) && (jogador->pulando == false))
        jogador->y = ALTURA_DO_CHÃO_ARENA - jogador->altura;
    else if ((jogador->abaixado == true) && (jogador->pulando == true))
        jogador->vel_y += 1.8f;

    if (jogador->y + jogador->altura >= ALTURA_DO_CHÃO_ARENA)
    {
        jogador->y = ALTURA_DO_CHÃO_ARENA - jogador->altura; // Alinha a BASE do jogador com o chão
        jogador->vel_y = 0.0f;
        jogador->pulando = false;
    }

    if (jogador->x < 0)
        jogador->x = 0;

    if (jogador->x + jogador->largura > LARGURA_DA_TELA)
    {
        jogador->x = LARGURA_DA_TELA - jogador->largura;
    }

    if (jogador->abaixado)
    {
        jogador->sprite_atual = jogador->sprite_abaixado;
    }
    else if (jogador->atirando)
    {
        jogador->sprite_atual = jogador->sprite_atirando;
    }
    else if (jogador->pulando)
    {
        jogador->sprite_atual = jogador->sprite_pulando;
    }
    else if (jogador->frame_atual_animacao == 0)
    {
        jogador->sprite_atual = jogador->sprite_parado;
    }
    else if (jogador->frame_atual_animacao == 1)
    {
        jogador->sprite_atual = jogador->sprite_andando;
    }
}