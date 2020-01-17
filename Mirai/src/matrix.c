//
//  matrix.c
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "matrix.h"

// MARK: - Functions

void mat4_read43(FILE *file, struct mat4_t *matrix)
{
    fread(&matrix->data[0][0], sizeof(float), 1, file);
    fread(&matrix->data[1][0], sizeof(float), 1, file);
    fread(&matrix->data[2][0], sizeof(float), 1, file);
    fread(&matrix->data[3][0], sizeof(float), 1, file);

    fread(&matrix->data[0][1], sizeof(float), 1, file);
    fread(&matrix->data[1][1], sizeof(float), 1, file);
    fread(&matrix->data[2][1], sizeof(float), 1, file);
    fread(&matrix->data[3][1], sizeof(float), 1, file);

    fread(&matrix->data[0][2], sizeof(float), 1, file);
    fread(&matrix->data[1][2], sizeof(float), 1, file);
    fread(&matrix->data[2][2], sizeof(float), 1, file);
    fread(&matrix->data[3][2], sizeof(float), 1, file);

    // initialize the last column
    matrix->data[0][3] = 0;
    matrix->data[1][3] = 0;
    matrix->data[2][3] = 0;
    matrix->data[3][3] = 0;
}
