//
//  utils.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "utils.h"

// MARK: - Functions

uint32_t utils_absolute_pointer(FILE *file)
{
    uint32_t base = (uint32_t)ftell(file);
    uint32_t offset;
    fread(&offset, sizeof(offset), 1, file);
    return base + offset;
}
