//
//  mtob.c
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "mtob.h"

#include <stdlib.h>
#include <assert.h>

#include "utils.h"

// MARK: - Functions

void mtob_open(FILE *file, struct mtob_t *mtob)
{
    // u32 preceding flags, unused
    fseek(file, 4, SEEK_CUR);

    // read the signature
    assert(fgetc(file) == 'M');
    assert(fgetc(file) == 'T');
    assert(fgetc(file) == 'O');
    assert(fgetc(file) == 'B');

    // u32 unknown
    fseek(file, 4, SEEK_CUR);

    // read the name and seek back
    uint32_t name_pointer = utils_read_relative_pointer(file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // initialize the mtob
    mtob->name = name;
}

void mtob_close(struct mtob_t *mtob)
{
    free(mtob->name);
}
