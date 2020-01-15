//
//  utils.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "utils.h"

#include <stdlib.h>
#include <string.h>

// MARK: - Functions

uint32_t utils_absolute_pointer(FILE *file)
{
    uint32_t base = (uint32_t)ftell(file);
    uint32_t offset;
    fread(&offset, sizeof(offset), 1, file);
    return base + offset;
}

char *utils_read_string(FILE *file)
{
    // use a generally safe max length of 256
    char string_fixed[256];
    int string_length = 0;
    while (1)
    {
        // dont allow the string to overflow
        if (string_length >= 256)
            break;

        char c = fgetc(file);
        string_fixed[string_length] = c;
        string_length++;

        // break on the first terminator
        if (c == '\0')
            break;
    }

    char *string = malloc(string_length);
    memcpy(string, string_fixed, string_length);
    return string;
}

void utils_read_vec3(FILE *file, struct vec3_t *vector)
{
    float x, y, z;
    fread(&x, sizeof(x), 1, file);
    fread(&y, sizeof(y), 1, file);
    fread(&z, sizeof(z), 1, file);

    vector->x = x;
    vector->y = y;
    vector->z = z;
}
