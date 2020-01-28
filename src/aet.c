//
//  aet.c
//  libmirai
//
//  Created by Marika on 2020-01-28.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "aet.h"

#include <assert.h>

void aet_open(const char *path, struct aet_t *aet)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");

    // initialize the aet
    aet->file = file;
}

void aet_close(struct aet_t *aet)
{
    fclose(aet->file);
}
