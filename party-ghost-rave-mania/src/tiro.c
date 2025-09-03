#include "tiro.h"
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#define VELOCIDADE_PROJETIL 10.0f
#define LARGURA_PROJETIL 10
#define ALTURA_PROJETIL 5

void atira(struct tiro *projetil, float x_inicial, float y_inicial, int direcao, int type, ALLEGRO_BITMAP *sprite_player, ALLEGRO_BITMAP *sprite_inimigo)
{

    if (projetil == NULL)
        return;

    projetil->x = x_inicial;
    projetil->y = y_inicial;

    projetil->lado = direcao;

    projetil->vel_x = VELOCIDADE_PROJETIL * direcao;

    if (direcao == 0)
        projetil->vel_y = -VELOCIDADE_PROJETIL;
    else
        projetil->vel_y = 0.0f;

    projetil->ativo = true;

    if (projetil->sprite == NULL)
    {
        if (type == 0)
            projetil->sprite = sprite_player;
        else
            projetil->sprite = sprite_inimigo;

        if (!projetil->sprite)
        {
            fprintf(stderr, "Falha ao carregar sprite do projetil\n");
            projetil->ativo = false;
            return;
        }

        if (type == 0)
        {
            projetil->largura = 128;
            projetil->altura = 128;
        }
        else
        {
            projetil->largura = 64;
            projetil->altura = 64;
        }
    }
}

void tiro_atualiza(struct tiro *projetil, int largura_tela, int altura_tela, float offset_x_mundo)
{

    if (projetil == NULL || !projetil->ativo)
        return;

    projetil->x += projetil->vel_x;
    projetil->y += projetil->vel_y;

    // Coordenadas de mundo que correspondem às bordas da tela
    float limite_esquerdo_mundo_na_tela = -offset_x_mundo;
    float limite_direito_mundo_na_tela = -offset_x_mundo + largura_tela;

    float limite_superior_mundo_na_tela = 0;
    float limite_inferior_mundo_na_tela = (float)altura_tela;

    if ((projetil->vel_x < 0 && projetil->x + projetil->largura < limite_esquerdo_mundo_na_tela) || // Saiu totalmente pela esquerda
        (projetil->vel_x > 0 && projetil->x > limite_direito_mundo_na_tela) ||                      // Saiu totalmente pela direita
        (projetil->vel_y < 0 && projetil->y + projetil->altura < limite_superior_mundo_na_tela) ||  // Saiu totalmente por cima
        (projetil->vel_y > 0 && projetil->y > limite_inferior_mundo_na_tela))                       // Saiu totalmente por baixo
    {
        projetil->ativo = false;
    }
}

void desenha_tiro(struct tiro *projetil, float x_tela, float y_tela)
{

    if (projetil == NULL || !projetil->ativo)
        return;

    int flag_lado = 0;

    if (projetil->lado == -1)
    {
        flag_lado = ALLEGRO_FLIP_HORIZONTAL;
    }

    if (projetil->sprite != NULL)
    {
        al_draw_scaled_bitmap(projetil->sprite, 0, 0, al_get_bitmap_width(projetil->sprite), al_get_bitmap_height(projetil->sprite), x_tela, y_tela, projetil->altura, projetil->largura, flag_lado);
    }
    else
    {
        al_draw_filled_rectangle(
            projetil->x,
            projetil->y,
            projetil->x + projetil->largura,
            projetil->y + projetil->altura,
            al_map_rgb(255, 0, 0));
    }
}

void tiro_atualiza_na_arena(struct tiro *projetil, int largura_tela, int altura_tela)
{

    if (projetil == NULL || !projetil->ativo)
        return;

    projetil->largura = 64;
    projetil->altura = 64;

    projetil->x += projetil->vel_x;
    projetil->y += projetil->vel_y;

    float limite_esquerdo_mundo_na_tela = 0;
    float limite_direito_mundo_na_tela = (float)largura_tela;

    float limite_superior_mundo_na_tela = 0;
    float limite_inferior_mundo_na_tela = (float)altura_tela;

    if ((projetil->vel_x < 0 && projetil->x + projetil->largura < limite_esquerdo_mundo_na_tela) || // Saiu totalmente pela esquerda
        (projetil->vel_x > 0 && projetil->x > limite_direito_mundo_na_tela) ||                      // Saiu totalmente pela direita
        (projetil->vel_y < 0 && projetil->y + projetil->altura < limite_superior_mundo_na_tela) ||  // Saiu totalmente por cima
        (projetil->vel_y > 0 && projetil->y > limite_inferior_mundo_na_tela))                       // Saiu totalmente por baixo
    {
        projetil->ativo = false;
    }
}