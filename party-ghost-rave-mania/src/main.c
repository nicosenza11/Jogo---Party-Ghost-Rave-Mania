#include <stdio.h>
#include "player.h"
#include "tiro.h"
#include "colisao.h"
#include "inimigo.h"
#include "config.h"
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

enum GameState
{
    STATE_MENU,
    STATE_JOGANDO,
    STATE_GAME_OVER_VITORIA,
    STATE_GAME_OVER_DERROTA
};

enum GameState estado_atual_jogo;

ALLEGRO_BITMAP *fundo = NULL;
ALLEGRO_BITMAP *chao = NULL;
ALLEGRO_BITMAP *fundo_vitoria = NULL;
ALLEGRO_BITMAP *fundo_derrota = NULL;
ALLEGRO_BITMAP *logo = NULL;
ALLEGRO_BITMAP *fundo_arena = NULL;
ALLEGRO_BITMAP *sprite_chefe = NULL;
ALLEGRO_BITMAP *sprite_tiro_player = NULL;
ALLEGRO_BITMAP *sprite_tiro_inimigo = NULL;
float offset_fundo = 0.0f;
int largura_fundo = 0;
int altura_fundo = 0;

int inimigos_derrotados = 0;
int inimigos_para_o_boss = 6;
bool fase_chefe = false;
bool boss_derrotado = false;

int fadiga_ataque = MAX_TIROS;

void desenha_barra_vida_jogador(struct Player *jogador, float hud_x, float hud_y, int barra_largura, int barra_altura, ALLEGRO_FONT *fonte_debug)
{
    if (jogador == NULL)
        return;

    // --- Barra de Fundo
    ALLEGRO_COLOR cor_fundo_vida = al_map_rgb(50, 50, 50); // Cinza escuro
    al_draw_filled_rectangle(hud_x, hud_y, hud_x + barra_largura, hud_y + barra_altura, cor_fundo_vida);

    // --- Barra de Vida Atual
    if (jogador->vida > 0 && jogador->vida_max > 0)
    { // Só desenha se tiver vida e vida_maxima válidas
        float porcentagem_vida = (float)jogador->vida / (float)jogador->vida_max;
        float largura_vida_atual = barra_largura * porcentagem_vida;

        ALLEGRO_COLOR cor_vida_atual;
        if (porcentagem_vida > 0.6f)
        {
            cor_vida_atual = al_map_rgb(0, 200, 0); // Verde
        }
        else if (porcentagem_vida > 0.3f)
        {
            cor_vida_atual = al_map_rgb(255, 255, 0); // Amarelo
        }
        else
        {
            cor_vida_atual = al_map_rgb(200, 0, 0); // Vermelho
        }

        al_draw_filled_rectangle(hud_x, hud_y, hud_x + largura_vida_atual, hud_y + barra_altura, cor_vida_atual);
    }

    // --- Contorno da Barra
    ALLEGRO_COLOR cor_contorno = al_map_rgb(200, 200, 200);                                                   // Branco/Cinza claro
    al_draw_rectangle(hud_x, hud_y, hud_x + barra_largura + 1, hud_y + barra_altura + 1, cor_contorno, 1.5f); // +1 para o contorno ficar "por fora"

    // --- Texto da Vida
    if (fonte_debug != NULL)
    {
        char texto_vida_hud[32];
        sprintf(texto_vida_hud, "Vida: %d / %d", jogador->vida, jogador->vida_max);
        // Desenha o texto um pouco à direita da barra ou sobre ela
        al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), hud_x + barra_largura + 10, hud_y, ALLEGRO_ALIGN_LEFT, texto_vida_hud);
    }
}

void desenha_barra_vida_boss(struct Inimigo *boss, float hud_x, float hud_y, int barra_largura, int barra_altura, ALLEGRO_FONT *fonte_debug)
{
    if (boss == NULL)
        return;

    // --- Barra de Fundo
    ALLEGRO_COLOR cor_fundo_vida = al_map_rgb(50, 50, 50); // Cinza escuro
    al_draw_filled_rectangle(hud_x, hud_y, hud_x + barra_largura, hud_y + barra_altura, cor_fundo_vida);

    // --- Barra de Vida Atual
    if (boss->vida > 0 && boss->vida_max > 0)
    { // Só desenha se tiver vida e vida_maxima válidas
        float porcentagem_vida = (float)boss->vida / (float)boss->vida_max;
        float largura_vida_atual = barra_largura * porcentagem_vida;

        ALLEGRO_COLOR cor_vida_atual;
        cor_vida_atual = al_map_rgb(200, 0, 0); // Vermelho

        al_draw_filled_rectangle(hud_x, hud_y, hud_x + largura_vida_atual, hud_y + barra_altura, cor_vida_atual);
    }

    // --- Contorno da Barra
    ALLEGRO_COLOR cor_contorno = al_map_rgb(200, 200, 200);                                                   // Branco/Cinza claro
    al_draw_rectangle(hud_x, hud_y, hud_x + barra_largura + 1, hud_y + barra_altura + 1, cor_contorno, 1.5f); // +1 para o contorno ficar "por fora"

    // --- Texto da Vida
    if (fonte_debug != NULL)
    {
        char texto_vida_hud[32];
        sprintf(texto_vida_hud, "Vida do Boss: %d / %d", boss->vida, boss->vida_max);
        // Desenha o texto um pouco à direita da barra ou sobre ela
        al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), hud_x + barra_largura + 10, hud_y, ALLEGRO_ALIGN_LEFT, texto_vida_hud);
    }
}

void desenha_barra_de_fadiga(struct Player *jogador, float hud_x, float hud_y, int barra_largura, int barra_altura, ALLEGRO_FONT *fonte_debug)
{
    if (jogador == NULL)
        return;

    // --- Barra de Fundo
    ALLEGRO_COLOR cor_fundo = al_map_rgb(50, 50, 50); // Cinza escuro
    al_draw_filled_rectangle(hud_x, hud_y, hud_x + barra_largura, hud_y + barra_altura, cor_fundo);

    // --- Barra de Vida Atual
    if (jogador->vida > 0 && jogador->vida_max > 0)
    { // Só desenha se tiver vida e vida_maxima válidas
        float porcentagem_fadiga = (float)fadiga_ataque / (float)MAX_TIROS;
        float largura_fadiga_atual = barra_largura * porcentagem_fadiga;

        ALLEGRO_COLOR cor_vida_atual;
        if (porcentagem_fadiga > 0.6f)
        {
            cor_vida_atual = al_map_rgb(0, 200, 0); // Verde
        }
        else if (porcentagem_fadiga > 0.3f)
        {
            cor_vida_atual = al_map_rgb(255, 255, 0); // Amarelo
        }
        else
        {
            cor_vida_atual = al_map_rgb(200, 0, 0); // Vermelho
        }

        al_draw_filled_rectangle(hud_x, hud_y, hud_x + largura_fadiga_atual, hud_y + barra_altura, cor_vida_atual);
    }

    // --- Contorno da Barra
    ALLEGRO_COLOR cor_contorno = al_map_rgb(200, 200, 200);                                                   // Branco/Cinza claro
    al_draw_rectangle(hud_x, hud_y, hud_x + barra_largura + 1, hud_y + barra_altura + 1, cor_contorno, 1.5f); // +1 para o contorno ficar "por fora"

    // --- Texto da Vida
    if (fonte_debug != NULL)
    {
        char texto_vida_hud[32];
        sprintf(texto_vida_hud, "Stamina: %d / %d", fadiga_ataque, MAX_TIROS);
        // Desenha o texto um pouco à direita da barra ou sobre ela
        al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), hud_x + barra_largura + 10, hud_y, ALLEGRO_ALIGN_LEFT, texto_vida_hud);
    }
}

void verifica_erro(bool teste, const char *descricao)
{
    if (teste)
        return;

    printf("Não foi possível inicializar %s\n", descricao);
    exit(1);
}

int main()
{
    srand(time(NULL));

    verifica_erro(al_init(), "Allegro");

    // NÃO MEXER ANTES

    verifica_erro(al_init_font_addon(), "addon de fonte");
    verifica_erro(al_init_ttf_addon(), "addon de fontes TTF");

    ALLEGRO_FONT *fonte_menu = al_load_font("assets/fontes/fonte.ttf", 36, 0);
    ALLEGRO_FONT *fonte_game_over = al_load_font("assets/fontes/fonte.ttf", 48, 0);
    verifica_erro(fonte_menu, "fonte do menu");
    verifica_erro(fonte_game_over, "fonte game over");

    estado_atual_jogo = STATE_MENU;

    verifica_erro(al_init_primitives_addon(), "addon de primitivas");

    verifica_erro(al_init_image_addon(), "addon de imagem");

    verifica_erro(al_install_keyboard(), "teclado");

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    verifica_erro(timer, "timer");

    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    verifica_erro(fila_eventos, "fila_eventos");

    ALLEGRO_DISPLAY *janela = al_create_display(LARGURA, ALTURA);
    verifica_erro(janela, "janela");

    struct Inimigo chefe;

    struct tiro projeteis[MAX_TIROS];

    for (int i = 0; i < MAX_TIROS; i++)
    {
        projeteis[i].ativo = false;
        projeteis[i].sprite = NULL;
    }

    struct tiro projeteis_inimigos[MAX_TIROS_INIMIGOS];

    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
    {
        projeteis_inimigos[i].ativo = false;
        projeteis_inimigos[i].sprite = NULL;
    }

    struct Player *jogador_principal = player_create(100.0f, ALTURA - 100.0f);
    if (jogador_principal == NULL)
    {
        fprintf(stderr, "Nao foi possivel criar o jogador principal. Encerrando.\n");
        if (janela)
            al_destroy_display(janela);
        if (timer)
            al_destroy_timer(timer);
        if (fila_eventos)
            al_destroy_event_queue(fila_eventos);
        return -1;
    }

    struct Inimigo inimigos[MAX_INIMIGOS];

    for (int i = 0; i < MAX_INIMIGOS; i++)
    {
        inimigos[i].vivo = false;
        inimigos[i].sprite_atual = NULL;
    }

    if (MAX_INIMIGOS > 0)
    {
        inimigo_spawn(&inimigos[0], 2000.0f, ALTURA - 256.0f, 0);
    }
    /**/
    if (MAX_INIMIGOS > 1)
    {
        inimigo_spawn(&inimigos[1], 4000.0f, ALTURA - 256.0f, 0);
    }
    if (MAX_INIMIGOS > 2)
    {
        inimigo_spawn(&inimigos[2], 5000.0f, ALTURA - 256.0f, 0);
    }
    if (MAX_INIMIGOS > 3)
    {
        inimigo_spawn(&inimigos[3], 7000.0f, ALTURA - 256.0f, 0);
    }
    if (MAX_INIMIGOS > 4)
    {
        inimigo_spawn(&inimigos[4], 8000.0f, ALTURA - 256.0f, 0);
    }
    if (MAX_INIMIGOS > 5)
    {
        inimigo_spawn(&inimigos[5], 9000.0f, ALTURA - 256.0f, 0);
    }

    fundo = al_load_bitmap("assets/fundoteste.png");
    chao = al_load_bitmap("assets/chao.png");
    fundo_vitoria = al_load_bitmap("assets/fundo_vitoria.png");
    fundo_derrota = al_load_bitmap("assets/fundo_derrota.png");
    fundo_arena = al_load_bitmap("assets/fundo_arena.png");
    logo = al_load_bitmap("assets/logo.png");
    sprite_chefe = al_load_bitmap("assets/sprites/boss.png");
    sprite_tiro_player = al_load_bitmap("assets/tiro.png");
    sprite_tiro_inimigo = al_load_bitmap("assets/tiro_inimigo.png");

    if (fundo == NULL)
    {
        fprintf(stderr, "Nao foi possivel criar o fundo. Encerrando.\n");
        if (jogador_principal)
            player_destroi(jogador_principal);
        if (janela)
            al_destroy_display(janela);
        if (timer)
            al_destroy_timer(timer);
        if (fila_eventos)
            al_destroy_event_queue(fila_eventos);
        return -1;
    }

    largura_fundo = al_get_bitmap_width(fundo);
    altura_fundo = al_get_bitmap_height(fundo);

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    bool rodando = true;
    bool redesenhar = true;

    ALLEGRO_EVENT evento;
    ALLEGRO_KEYBOARD_STATE kbd_estado;

    al_start_timer(timer);

    while (rodando)
    {
        al_get_keyboard_state(&kbd_estado);

        al_wait_for_event(fila_eventos, &evento);

        switch (estado_atual_jogo)
        {
        case STATE_MENU:

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {

                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    // A FAZER
                    // reiniciar_jogo();
                    estado_atual_jogo = STATE_JOGANDO;
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {

                    rodando = false;
                }
            }
            break;

        case STATE_JOGANDO:

            switch (evento.type)
            {

            case ALLEGRO_EVENT_TIMER:

                if (jogador_principal != NULL && jogador_principal->vida <= 0)
                    estado_atual_jogo = STATE_GAME_OVER_DERROTA;

                int projeteis_ativos_jogador_contador = 0;

                for (int k = 0; k < MAX_TIROS; k++)
                { // Itera sobre o array de projéteis do jogador
                    if (projeteis[k].ativo)
                    {
                        projeteis_ativos_jogador_contador++;
                    }
                }

                fadiga_ataque = MAX_TIROS - projeteis_ativos_jogador_contador;

                if (fase_chefe)
                {

                    offset_fundo = 0;

                    if (jogador_principal != NULL && jogador_principal->vivo)
                    {
                        player_input(jogador_principal, &kbd_estado);
                        // Atualiza jogador com limites da ARENA e chão da ARENA
                        player_atualiza_na_arena(jogador_principal);
                    }

                    if (chefe.vivo)
                    {
                        boss_atualiza(&chefe, projeteis_inimigos, MAX_TIROS_INIMIGOS);
                    }

                    for (int i = 0; i < MAX_TIROS; i++)
                    {

                        if (projeteis[i].ativo)
                        {
                            tiro_atualiza_na_arena(&projeteis[i], LARGURA, ALTURA);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
                    {
                        if (projeteis_inimigos[i].ativo)
                        {
                            tiro_atualiza_na_arena(&projeteis_inimigos[i], LARGURA, ALTURA);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
                    {
                        if (projeteis_inimigos[i].ativo)
                        {
                            struct Hitbox hitbox_tiro_inimigo =
                                {
                                    projeteis_inimigos[i].x,
                                    projeteis_inimigos[i].y,
                                    (float)projeteis_inimigos[i].largura,
                                    (float)projeteis_inimigos[i].altura};

                            if (jogador_principal->vivo)
                            {

                                struct Hitbox hitbox_player =
                                    {
                                        jogador_principal->x,
                                        jogador_principal->y,
                                        (float)jogador_principal->largura,
                                        (float)jogador_principal->altura};

                                if (colide(hitbox_tiro_inimigo, hitbox_player))
                                {
                                    jogador_principal->vida -= 1;
                                    projeteis_inimigos[i].ativo = false;

                                    if (jogador_principal->vida <= 0)
                                    {
                                        jogador_principal->vivo = false;
                                        printf("Player derrotado!\n");
                                    }

                                    break;
                                }
                            }
                        }
                    }

                    if (chefe.vivo && chefe.vida > 0)
                    {

                        for (int i = 0; i < MAX_TIROS; i++)
                        {

                            if (projeteis[i].ativo)
                            {
                                struct Hitbox hb_tiro_j = {projeteis[i].x, projeteis[i].y, (float)projeteis[i].largura, (float)projeteis[i].altura};
                                struct Hitbox hb_chefe = {chefe.x, chefe.y, (float)chefe.largura, (float)chefe.altura};

                                if (colide(hb_tiro_j, hb_chefe))
                                {

                                    chefe.vida -= 1;
                                    projeteis[i].ativo = false;

                                    if (chefe.vida <= 0)
                                    {
                                        chefe.vivo = false;
                                        printf("CHEFE DERROTADO! VITORIA!\n");
                                        estado_atual_jogo = STATE_GAME_OVER_VITORIA;
                                        boss_derrotado = true;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {

                    if (jogador_principal != NULL)
                    {

                        player_input(jogador_principal, &kbd_estado);
                        player_atualiza(jogador_principal);

                        float pos_jogador_alvo_tela_x = (LARGURA / 2.0f) - (jogador_principal->largura / 2.0f);

                        offset_fundo = pos_jogador_alvo_tela_x - jogador_principal->x;

                        if (offset_fundo > 0)
                        {
                            offset_fundo = 0;
                        }

                        if (largura_fundo > LARGURA)
                        {

                            if (offset_fundo < -(largura_fundo - LARGURA))
                            {
                                offset_fundo = -(largura_fundo - LARGURA);
                            }
                        }
                    }
                    else
                    {
                        offset_fundo = 0;
                    }

                    for (int i = 0; i < MAX_TIROS; i++)
                    {
                        if (projeteis[i].ativo)
                        {
                            tiro_atualiza(&projeteis[i], LARGURA, ALTURA, offset_fundo);
                        }
                    }

                    for (int i = 0; i < MAX_INIMIGOS; i++)
                    {
                        if (inimigos[i].vivo)
                        {
                            inimigo_atualiza(&inimigos[i], projeteis_inimigos, MAX_TIROS_INIMIGOS, offset_fundo);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
                    {
                        if (projeteis_inimigos[i].ativo)
                        {
                            tiro_atualiza(&projeteis_inimigos[i], LARGURA, ALTURA, offset_fundo);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS; i++)
                    {
                        if (projeteis[i].ativo)
                        {
                            struct Hitbox hitbox_tiro =
                                {
                                    projeteis[i].x,
                                    projeteis[i].y,
                                    (float)projeteis[i].largura,
                                    (float)projeteis[i].altura};

                            for (int j = 0; j < MAX_INIMIGOS; j++)
                            {
                                if (inimigos[j].vivo && inimigos[j].vida > 0)
                                {

                                    struct Hitbox hitbox_inimigo =
                                        {
                                            inimigos[j].x,
                                            inimigos[j].y,
                                            (float)inimigos[j].largura,
                                            (float)inimigos[j].altura};

                                    if (colide(hitbox_tiro, hitbox_inimigo))
                                    {
                                        inimigos[j].vida -= 1;
                                        projeteis[i].ativo = false;

                                        if (inimigos[j].vida <= 0)
                                        {

                                            inimigos[j].vivo = false;
                                            printf("Inimigo %d derrotado!\n", j);

                                            if (!fase_chefe)
                                            { // Só incrementa e verifica se AINDA não estamos na fase do chefe

                                                inimigos_derrotados++;
                                                printf("Inimigos derrotados: %d / %d\n", inimigos_derrotados, inimigos_para_o_boss);

                                                if (inimigos_derrotados >= inimigos_para_o_boss)
                                                {
                                                    fase_chefe = true;

                                                    for (int k = 0; k < MAX_INIMIGOS; k++)
                                                        inimigos[k].vivo = false;

                                                    for (int k = 0; k < MAX_TIROS; k++)
                                                        projeteis[k].ativo = false;

                                                    for (int k = 0; k < MAX_TIROS_INIMIGOS; k++)
                                                        projeteis_inimigos[k].ativo = false;

                                                    if (jogador_principal != NULL)
                                                    {

                                                        jogador_principal->x = 150.0f;
                                                        jogador_principal->altura = ALTURA_JOGADOR_ARENA;
                                                        jogador_principal->largura = ALTURA_JOGADOR_ARENA;
                                                        jogador_principal->y = ALTURA_DO_CHÃO_ARENA - jogador_principal->altura;

                                                        jogador_principal->vel_x = 0.0f;
                                                        jogador_principal->vel_y = 0.0f;
                                                        jogador_principal->pulando = false;
                                                    }

                                                    inimigo_spawn(&chefe, LARGURA + LARGURA_DO_BOSS, ALTURA_DO_CHÃO_ARENA - ALTURA_DO_BOSS, 1);

                                                    chefe.vida = 50; // Mais vida
                                                    chefe.vida_max = chefe.vida;
                                                    chefe.largura = LARGURA_DO_BOSS;
                                                    chefe.altura = ALTURA_DO_BOSS;
                                                    chefe.vel_x = -10.0f;            // Movimento do chefe na arena
                                                    chefe.direcao_tiro_inimigo = -1; // Começa atirando para esquerda
                                                    chefe.cooldown_tiro_total = 120; // Cooldown em frames
                                                    chefe.cooldown_tiro_restante = chefe.cooldown_tiro_total;

                                                    if (sprite_chefe != NULL)
                                                    {

                                                        if (chefe.sprite_atual != NULL && chefe.sprite_atual != sprite_chefe)
                                                        {

                                                            al_destroy_bitmap(chefe.sprite_atual);
                                                        }

                                                        chefe.sprite_atual = sprite_chefe;
                                                    }

                                                    fase_chefe = true;
                                                }
                                            }
                                        }

                                        break;
                                    }
                                }
                            }
                        }
                    }

                    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
                    {

                        if (projeteis_inimigos[i].ativo)
                        {

                            struct Hitbox hitbox_tiro_inimigo =
                                {
                                    projeteis_inimigos[i].x,
                                    projeteis_inimigos[i].y,
                                    (float)projeteis_inimigos[i].largura,
                                    (float)projeteis_inimigos[i].altura};

                            if (jogador_principal->vivo)
                            {

                                struct Hitbox hitbox_player =
                                    {
                                        jogador_principal->x,
                                        jogador_principal->y,
                                        (float)jogador_principal->largura,
                                        (float)jogador_principal->altura};

                                if (colide(hitbox_tiro_inimigo, hitbox_player))
                                {
                                    jogador_principal->vida -= 1;
                                    projeteis_inimigos[i].ativo = false;

                                    if (jogador_principal->vida <= 0)
                                    {
                                        jogador_principal->vivo = false;
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }

                redesenhar = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                // Verifica se a tecla ESC foi pressionada
                if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    rodando = false; // Termina o loop
                }

                if (evento.keyboard.keycode == ALLEGRO_KEY_X || evento.keyboard.keycode == ALLEGRO_KEY_LCTRL)
                {
                    if (jogador_principal != NULL)
                    {

                        jogador_principal->atirando = true;
                        jogador_principal->frame_timer = 15;

                        for (int i = 0; i < MAX_TIROS; i++)
                        {

                            if (!projeteis[i].ativo)
                            {

                                float tiro_offset_y = jogador_principal->altura / 2.0f - ALTURA_PROJETIL / 2.0f;
                                float tiro_offset_x = 0;

                                if (jogador_principal->lado == 1)
                                {
                                    tiro_offset_x = jogador_principal->largura;
                                }
                                else
                                {
                                    tiro_offset_x = -LARGURA_PROJETIL;
                                }

                                if ((jogador_principal->mirando_cima) && (!fase_chefe))
                                {
                                    atira(&projeteis[i],
                                          jogador_principal->x - 200.0f + tiro_offset_x,
                                          jogador_principal->y + tiro_offset_y,
                                          0, 0, sprite_tiro_player, sprite_tiro_inimigo);
                                }
                                else if ((jogador_principal->mirando_cima) && (fase_chefe))
                                {
                                    atira(&projeteis[i],
                                          jogador_principal->x + tiro_offset_x,
                                          jogador_principal->y + tiro_offset_y,
                                          0, 0, sprite_tiro_player, sprite_tiro_inimigo);
                                }
                                else
                                {
                                    atira(&projeteis[i],
                                          jogador_principal->x + tiro_offset_x,
                                          jogador_principal->y + tiro_offset_y,
                                          jogador_principal->lado, 0, sprite_tiro_player, sprite_tiro_inimigo);
                                }

                                break;
                            }
                        }
                    }
                }

                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // O usuário clicou no botão de fechar da janela
                rodando = false; // Termina o loop
                break;
            }
            break;

        case STATE_GAME_OVER_VITORIA:
        case STATE_GAME_OVER_DERROTA:

            // Processar input da tela de Game Over
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    estado_atual_jogo = STATE_MENU; // Volta para o menu
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    rodando = false; // Sair do jogo
                }
            }
            break;
        }

        if (!rodando)
        {
            break;
        }

        if (redesenhar && al_is_event_queue_empty(fila_eventos))
        {
            switch (estado_atual_jogo)
            {

            case STATE_MENU:

                // Desenhar tela de menu
                al_draw_bitmap(fundo, offset_fundo, 0, 0);

                float y_texto_iniciar;

                if (logo != NULL)
                {
                    int largura_original_logo = al_get_bitmap_width(logo);
                    int altura_original_logo = al_get_bitmap_height(logo);

                    float largura_desenhada_logo = largura_original_logo;
                    float altura_desenhada_logo = altura_original_logo;

                    float logo_dx = (LARGURA / 2.0f) - (largura_desenhada_logo / 2.0f);
                    float logo_dy = (ALTURA * 0.4f) - (altura_desenhada_logo / 2.0f);

                    al_draw_scaled_bitmap(
                        logo,
                        0, 0,
                        largura_original_logo,
                        altura_original_logo,
                        logo_dx,
                        logo_dy,
                        largura_desenhada_logo,
                        altura_desenhada_logo,
                        0);

                    y_texto_iniciar = (ALTURA * 0.4f) + (altura_desenhada_logo / 2.0f) + 60;
                }

                al_draw_text(fonte_menu, al_map_rgb(200, 200, 200), LARGURA / 2, y_texto_iniciar, ALLEGRO_ALIGN_CENTRE, "Pressione ENTER para Iniciar");
                al_draw_text(fonte_menu, al_map_rgb(150, 150, 150), LARGURA / 2, y_texto_iniciar + 50, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para Sair");
                break;

            case STATE_JOGANDO:

                al_clear_to_color(al_map_rgb(0, 0, 0));

                if (fase_chefe)
                {

                    if (fundo_arena != NULL)
                    {
                        al_draw_bitmap(fundo_arena, 0, 0, 0);
                    }

                    al_draw_filled_rectangle(0, ALTURA_DO_CHÃO_ARENA, LARGURA, ALTURA, al_map_rgb(80, 80, 100));

                    if (chefe.vivo)
                    {
                        desenha_inimigo(&chefe, chefe.x, chefe.y);
                    }

                    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
                    {

                        if (projeteis_inimigos[i].ativo)
                        {
                            desenha_tiro(&projeteis_inimigos[i], projeteis_inimigos[i].x, projeteis_inimigos[i].y);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS; i++)
                    {
                        if (projeteis[i].ativo)
                        {
                            desenha_tiro(&projeteis[i], projeteis[i].x, projeteis[i].y);
                        }
                    }

                    if (jogador_principal != NULL && jogador_principal->vivo)
                    {
                        desenha_player(jogador_principal, jogador_principal->x, jogador_principal->y);
                    }

                    if (jogador_principal != NULL)
                    {
                        float hud_pos_x = 90;
                        float hud_pos_y = 90;
                        int hud_barra_largura = 200;
                        int hud_barra_altura = 25;
                        desenha_barra_vida_boss(&chefe, hud_pos_x, hud_pos_y, hud_barra_largura, hud_barra_altura, fonte_menu);
                    }
                }
                else
                {

                    if (fundo != NULL)
                    {
                        float largura_ciclo_completo = 2.0f * largura_fundo;

                        float offset_fundo_normalizado = fmodf(offset_fundo, largura_ciclo_completo);

                        if (offset_fundo_normalizado > 0)
                            offset_fundo_normalizado -= largura_fundo;

                        for (float x_desenho = offset_fundo_normalizado; x_desenho < LARGURA; x_desenho += largura_ciclo_completo)
                        {

                            al_draw_bitmap(fundo, x_desenho, 0, 0);
                            al_draw_bitmap(fundo, x_desenho + largura_fundo, 0, ALLEGRO_FLIP_HORIZONTAL);
                        }

                        float offset_chao_normalizado = fmodf(offset_fundo, (float)ALTURA - ALTURA_DO_CHÃO);
                        if (offset_chao_normalizado > 0)
                        {
                            offset_chao_normalizado -= ALTURA - ALTURA_DO_CHÃO;
                        }

                        for (int j = offset_chao_normalizado; j < LARGURA; j += ALTURA - ALTURA_DO_CHÃO)
                        {
                            al_draw_scaled_bitmap(chao, 0, 0, al_get_bitmap_width(chao), al_get_bitmap_height(chao), j, ALTURA_DO_CHÃO, ALTURA - ALTURA_DO_CHÃO, ALTURA - ALTURA_DO_CHÃO, 0);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS; i++)
                    {

                        if (projeteis[i].ativo)
                        {

                            float tiro_x = projeteis[i].x + offset_fundo;
                            float tiro_y = projeteis[i].y;

                            desenha_tiro(&projeteis[i], tiro_x, tiro_y);
                        }
                    }

                    for (int i = 0; i < MAX_INIMIGOS; i++)
                    {

                        if (inimigos[i].vivo)
                        {
                            float inimigo_x = inimigos[i].x + offset_fundo;
                            float inimigo_y = inimigos[i].y;

                            desenha_inimigo(&inimigos[i], inimigo_x, inimigo_y);
                        }
                    }

                    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++)
                    {

                        if (projeteis_inimigos[i].ativo)
                        {

                            float tiro_inimigo_x = projeteis_inimigos[i].x + offset_fundo;
                            float tiro_inimigo_y = projeteis_inimigos[i].y;

                            desenha_tiro(&projeteis_inimigos[i], tiro_inimigo_x, tiro_inimigo_y);
                        }
                    }

                    if (jogador_principal != NULL && jogador_principal->vivo)
                    {

                        float x_jogador_tela = jogador_principal->x + offset_fundo;
                        float y_jogador_tela = jogador_principal->y;
                        desenha_player(jogador_principal, x_jogador_tela, y_jogador_tela);
                    }
                }

                if (jogador_principal != NULL)
                {
                    float hud_pos_x = 20;
                    float hud_pos_y = 20;
                    int hud_barra_largura = 200;
                    int hud_barra_altura = 25;
                    desenha_barra_vida_jogador(jogador_principal, hud_pos_x, hud_pos_y, hud_barra_largura, hud_barra_altura, fonte_menu);
                }

                if (jogador_principal != NULL)
                {
                    float hud_pos_x = 1000;
                    float hud_pos_y = 20;
                    int hud_barra_largura = 200;
                    int hud_barra_altura = 25;
                    desenha_barra_de_fadiga(jogador_principal, hud_pos_x, hud_pos_y, hud_barra_largura, hud_barra_altura, fonte_menu);
                }

                al_flip_display();
                break;

            case STATE_GAME_OVER_VITORIA:
                al_draw_bitmap(fundo_vitoria, offset_fundo, 0, 0);

                al_draw_text(fonte_game_over, al_map_rgb(0, 255, 0), LARGURA / 2, ALTURA / 3, ALLEGRO_ALIGN_CENTRE, "VOCE VENCEU!");
                al_draw_text(fonte_menu, al_map_rgb(200, 200, 200), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para Sair");
                break;
            case STATE_GAME_OVER_DERROTA:
                al_draw_bitmap(fundo_derrota, offset_fundo, 0, 0);

                al_draw_text(fonte_menu, al_map_rgb(200, 200, 200), LARGURA / 2, 950.0f, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para Sair");
                break;
            }
            al_flip_display();
            redesenhar = false;
        }
    }

    al_destroy_font(fonte_menu);
    al_destroy_font(fonte_game_over);

    if (jogador_principal != NULL)
    {
        player_destroi(jogador_principal);
        jogador_principal = NULL;
    }

    if (fundo != NULL)
    {
        al_destroy_bitmap(fundo);
        fundo = NULL;
    }

    if (janela != NULL)
    {
        al_destroy_display(janela);
        janela = NULL;
    }
    if (timer != NULL)
    {
        al_destroy_timer(timer);
        timer = NULL;
    }
    if (fila_eventos != NULL)
    {
        al_destroy_event_queue(fila_eventos);
        fila_eventos = NULL;
    }

    al_shutdown_image_addon();
    al_shutdown_primitives_addon();

    al_uninstall_keyboard();

    al_uninstall_system();

    printf("Jogo terminado.\n");
    return 0;
}