#include "colisao.h"
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

bool colide(struct Hitbox r1, struct Hitbox r2) {

    if (r1.x + r1.largura < r2.x) {
        return false;
    }
    if (r1.x > r2.x + r2.largura) {
        return false;
    }
    if (r1.y + r1.altura < r2.y) {
        return false;
    }
    if (r1.y > r2.y + r2.altura) {
        return false;
    }

    return true;

}