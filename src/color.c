//
//  color.c
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "color.h"

// MARK: - Functions

void color4_readf(FILE *file, struct color4_t *color)
{
    fread(&color->r, sizeof(float), 1, file);
    fread(&color->g, sizeof(float), 1, file);
    fread(&color->b, sizeof(float), 1, file);
    fread(&color->a, sizeof(float), 1, file);
}

void color4_readu8(FILE *file, struct color4_t *color)
{
    uint8_t r, g, b, a;
    fread(&r, sizeof(r), 1, file);
    fread(&g, sizeof(g), 1, file);
    fread(&b, sizeof(b), 1, file);
    fread(&a, sizeof(a), 1, file);

    color->r = (float)r / UINT8_MAX;
    color->g = (float)g / UINT8_MAX;
    color->b = (float)b / UINT8_MAX;
    color->a = (float)a / UINT8_MAX;
}
