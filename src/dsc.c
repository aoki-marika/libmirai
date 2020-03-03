//
//  dsc.c
//  libmirai
//
//  Created by Marika on 2020-03-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "dsc.h"

// MARK: - Functions

void dsc_open(const char *path, struct dsc_t *dsc)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");
    dsc->file = file;
}

void dsc_close(struct dsc_t *dsc)
{
    fclose(dsc->file);
}
