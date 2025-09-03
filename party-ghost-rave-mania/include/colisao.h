#ifndef COLISAO_H
#define COLISAO_H

#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

struct Hitbox
{

    // Posição
    float x;
    float y;

    // Tamanho
    float altura, largura;
};

bool colide(struct Hitbox r1, struct Hitbox r2);

#endif