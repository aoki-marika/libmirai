//
//  vector.c
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "vector.h"

// MARK: - Functions

void vec2_read(FILE *file, struct vec2_t *vector)
{
    fread(&vector->x, sizeof(vector->x), 1, file);
    fread(&vector->y, sizeof(vector->y), 1, file);
}

void vec3_read(FILE *file, struct vec3_t *vector)
{
    fread(&vector->x, sizeof(vector->x), 1, file);
    fread(&vector->y, sizeof(vector->y), 1, file);
    fread(&vector->z, sizeof(vector->z), 1, file);
}
