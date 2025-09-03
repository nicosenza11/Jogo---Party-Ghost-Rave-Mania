#include "tiro.h"
#include "inimigo.h"
#include "config.h"
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <time.h>

void inimigo_spawn(struct Inimigo *novo_inimigo, float x_inicial, float y_inicial, int type)
{

    if (novo_inimigo == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria para o inimigo!\n");
        return;
    }

    novo_inimigo->x = x_inicial;
    novo_inimigo->y = y_inicial;

    novo_inimigo->vel_x = -2.0f;
    novo_inimigo->vel_y = 0.0f;

    novo_inimigo->cooldown_tiro_total = 50;
    novo_inimigo->cooldown_tiro_restante = novo_inimigo->cooldown_tiro_total;
    novo_inimigo->direcao_tiro_inimigo = -1;

    novo_inimigo->largura = 128;
    novo_inimigo->altura = 128;

    novo_inimigo->andando = false;
    novo_inimigo->pulando = false;
    novo_inimigo->vivo = true;
    novo_inimigo->triggered = false;

    novo_inimigo->frame_timer = 15;
    novo_inimigo->vida = 5;
    novo_inimigo->vida_max = 5;

    novo_inimigo->sprite_tiro = al_load_bitmap("assets/tiro_inimigo.png");

    novo_inimigo->sprite_tiro = al_load_bitmap("assets/tiro_inimigo.png");
    novo_inimigo->sprite_inimigo_normal = al_load_bitmap("assets/inimigo1.png");
    novo_inimigo->sprite_inimigo_normal_atirando = al_load_bitmap("assets/inimigo_atirando.png");
    novo_inimigo->sprite_inimigo_normal_derrotado = al_load_bitmap("assets/tiro_inimigo.png");
    novo_inimigo->sprite_boss = al_load_bitmap("assets/boss.png");
    novo_inimigo->sprite_boss_atirando = al_load_bitmap("assets/tiro_inimigo.png");
    novo_inimigo->sprite_boss_derrotado = al_load_bitmap("assets/tiro_inimigo.png");

    if (type == 0)
        novo_inimigo->sprite_atual = novo_inimigo->sprite_inimigo_normal;
    else
        novo_inimigo->sprite_atual = novo_inimigo->sprite_boss;

    novo_inimigo->frame_atual_animacao = 1;
}

void inimigo_destroi(struct Inimigo *inimigo)
{

    if (inimigo == NULL)
    {
        fprintf(stderr, "Tentativa de destruir um inimigo NULO.\n");
        return;
    }

    if (inimigo->sprite_atual != NULL)
    {
        al_destroy_bitmap(inimigo->sprite_atual);
        inimigo->sprite_atual = NULL;
    }

    printf("Inimigo destruido.\n");

    free(inimigo);
}

void desenha_inimigo(struct Inimigo *inimigo, float x_tela, float y_tela)
{

    if (inimigo == NULL)
    {
        fprintf(stderr, "Erro: Tentativa de desenhar um inimigo NULO em desenha_inimigo.\n");
        return;
    }

    if (inimigo->sprite_atual != NULL)
    {
        al_draw_scaled_bitmap(inimigo->sprite_atual, 0, 0, al_get_bitmap_width(inimigo->sprite_atual), al_get_bitmap_height(inimigo->sprite_atual), x_tela, y_tela, inimigo->largura, inimigo->altura, 1);
    }
    else
    {
        al_draw_filled_rectangle(
            inimigo->x,
            inimigo->y,
            inimigo->x + inimigo->largura,
            inimigo->y + inimigo->altura,
            al_map_rgb(255, 0, 0));
    }
}

void inimigo_atualiza(struct Inimigo *inimigo, struct tiro array_projeteis_inimigos[], int max_projeteis, float offset_fundo_atual)
{

    if (inimigo == NULL)
    {
        return;
    }

    if (inimigo->vida == 0)
    {
        inimigo_destroi(inimigo);
        printf("Inimigo foi vencido, morto");
        return;
    }

    inimigo->x += inimigo->vel_x;
    inimigo->y += inimigo->vel_y;

    inimigo->frame_atual_animacao++;

    if (inimigo->frame_atual_animacao > 2)
        inimigo->frame_atual_animacao = 1;

    if (inimigo->pulando == true)
    {

        inimigo->vel_y += 0.8f; // gravidade
    }

    if (inimigo->y + inimigo->altura >= 550.0f)
    {
        inimigo->y = 550.0f - inimigo->altura;
        inimigo->vel_y = 0.0f;
        inimigo->pulando = false;
    }

    if (inimigo->x < 0)
        inimigo->x = 0;

    if (inimigo->x + inimigo->largura > LARGURA_TOTAL_MUNDO)
    {
        inimigo->x = LARGURA_TOTAL_MUNDO - inimigo->largura;
    }

    float x_inimigo_tela = inimigo->x + offset_fundo_atual;
    float y_inimigo_tela = inimigo->y;

    bool esta_na_tela_horizontal = (x_inimigo_tela + inimigo->largura > 0 && x_inimigo_tela < LARGURA_DA_TELA);
    bool esta_na_tela_vertical = (y_inimigo_tela + inimigo->altura > 0 && y_inimigo_tela < ALTURA_DA_TELA);
    bool inimigo_esta_visivel = esta_na_tela_horizontal && esta_na_tela_vertical;

    if (inimigo_esta_visivel)
        inimigo->cooldown_tiro_restante -= 1;

    if (inimigo->cooldown_tiro_restante <= 0)
    {

        for (int i = 0; i < max_projeteis; i++)
        {
            if (!array_projeteis_inimigos[i].ativo)
            {
                float tiro_offset_y = inimigo->altura / 2.0f - array_projeteis_inimigos[i].altura / 2.0f;
                float tiro_offset_x = (inimigo->direcao_tiro_inimigo == 1) ? inimigo->largura : -array_projeteis_inimigos[i].largura;

                atira(&array_projeteis_inimigos[i],
                      inimigo->x + tiro_offset_x, // Coords de MUNDO
                      inimigo->y + tiro_offset_y,
                      inimigo->direcao_tiro_inimigo, 1, 0, inimigo->sprite_tiro);

                inimigo->atirando = true;
                inimigo->frame_timer = 15;
                inimigo->cooldown_tiro_restante = 30 + (int)(rand() % (int)(inimigo->cooldown_tiro_total - 30 + 1));
                break;
            }
        }
    }

    if (inimigo->frame_timer != 0)
        inimigo->frame_timer -= 1;

    if (inimigo->frame_timer == 0)
    {
        inimigo->atirando = false;
    }

    if ((inimigo->atirando) && (inimigo->frame_timer != 0))
    {
        inimigo->sprite_atual = inimigo->sprite_inimigo_normal_atirando;
    }
    else
        inimigo->sprite_atual = inimigo->sprite_inimigo_normal;
}

void boss_atualiza(struct Inimigo *boss, struct tiro array_projeteis_inimigos[], int max_projeteis)
{

    if (boss == NULL)
    {
        return;
    }

    if (boss->vida <= 0)
    {
        return;
    }

    if (boss->x <= LARGURA - 700.0f)
    {
        boss->vel_x = 0;
    }

    boss->x += boss->vel_x;
    boss->y += boss->vel_y;

    boss->frame_atual_animacao++;

    if (boss->frame_atual_animacao > 2)
        boss->frame_atual_animacao = 1;

    if (boss->pulando == true)
    {
        boss->vel_y += 0.8f; // gravidade
    }

    if (boss->y + boss->altura >= ALTURA_DO_CHÃO_ARENA)
    {
        boss->y = ALTURA_DO_CHÃO_ARENA - boss->altura;
        boss->vel_y = 0.0f;
        boss->pulando = false;
    }

    if (boss->x < 0)
        boss->x = 0;

    float x_inimigo_tela = boss->x;
    float y_inimigo_tela = boss->y;

    bool esta_na_tela_horizontal = (x_inimigo_tela + boss->largura > 0 && x_inimigo_tela < LARGURA_DA_TELA);
    bool esta_na_tela_vertical = (y_inimigo_tela + boss->altura > 0 && y_inimigo_tela < ALTURA_DA_TELA);
    bool inimigo_esta_visivel = esta_na_tela_horizontal && esta_na_tela_vertical;

    if (inimigo_esta_visivel)
        boss->cooldown_tiro_restante -= 1;

    if (boss->cooldown_tiro_restante <= 0)
    {
        for (int i = 0; i < max_projeteis; i++)
        {
            if (!array_projeteis_inimigos[i].ativo)
            {
                float y_tiro_min = boss->y + boss->altura / 2.0f - array_projeteis_inimigos[i].altura / 2.0f;
                float y_tiro_max = ALTURA_DO_CHÃO_ARENA - array_projeteis_inimigos[i].altura - 50;

                if (y_tiro_max < y_tiro_min + array_projeteis_inimigos[i].altura)
                {
                    y_tiro_max = y_tiro_min + array_projeteis_inimigos[i].altura;
                }

                float y_spawn_tiro_chefe;
                if (y_tiro_max > y_tiro_min)
                {
                    y_spawn_tiro_chefe = y_tiro_min + (float)(rand() % (int)(y_tiro_max - y_tiro_min + 1));
                }
                else
                {
                    y_spawn_tiro_chefe = boss->y + (boss->altura / 2.0f) - (array_projeteis_inimigos[i].altura / 2.0f);
                }

                atira(&array_projeteis_inimigos[i],
                      boss->x,
                      y_spawn_tiro_chefe,
                      boss->direcao_tiro_inimigo, 1, 0, boss->sprite_tiro);

                boss->atirando = true;
                boss->cooldown_tiro_restante = 5 + (int)(rand() % (int)(boss->cooldown_tiro_total - 5 + 1));
                break;
            }
        }
    }

    if (boss->frame_timer != 0)
        boss->frame_timer -= 1;

    if (boss->frame_timer == 0)
    {
        boss->atirando = false;
    }
}