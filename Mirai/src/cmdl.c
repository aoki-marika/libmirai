//
//  cmdl.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "cmdl.h"

#include <assert.h>

// MARK: - Functions

void cmdl_open(FILE *file, struct cmdl_t *cmdl)
{
    // read the signature
    assert(fgetc(file) == 'C');
    assert(fgetc(file) == 'M');
    assert(fgetc(file) == 'D');
    assert(fgetc(file) == 'L');
}

void cmdl_close(struct cmdl_t *cmdl)
{
}
