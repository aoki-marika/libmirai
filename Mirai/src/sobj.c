//
//  sobj.c
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "sobj.h"

#include <assert.h>

// MARK: - Functions

void sobj_open(FILE *file, struct sobj_t *sobj)
{
    // read the flags
    //  - bit 1: skeleton
    //  - bit 4: model
    uint32_t flags;
    fread(&flags, sizeof(flags), 1, file);

    // read the signature
    assert(fgetc(file) == 'S');
    assert(fgetc(file) == 'O');
    assert(fgetc(file) == 'B');
    assert(fgetc(file) == 'J');
}

void sobj_close(struct sobj_t *sobj)
{
}
