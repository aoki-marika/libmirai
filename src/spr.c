//
//  spr.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ctpk.h"

// MARK: - Constants

/// The size of the allocated space for each CTPK name within an SPR, in bytes.
const int ctpk_name_allocated_size = 32;

/// The size of the allocated space for each SCR name within an SPR, in bytes.
const int scr_name_allocated_size = 71;

// MARK: - Functions

/// Read the string at the current offset of the given file handle, of the given allocated size.
///
/// Strings within SPRs are stored in a semi-odd way where they allocate a fixed
/// amount of space, then all whitespace is replaced by terminator characters.
/// @param file The file handle to read the string from.
/// @param allocated_size The size of the allocated space for the string, in bytes.
/// @returns The string at the current offset of the given file handle. Allocated.
char *spr_string_read(FILE *file, int allocated_size)
{
    char characters[allocated_size];
    fread(characters, allocated_size, 1, file);

    // allocate the maximum size first,
    // then find the real size and reallocate it
    char *name = malloc(allocated_size + 1);
    int name_index = 0;
    int name_length = 0;
    for (int i = 0; i < allocated_size; i++)
    {
        if (characters[i] != '\0')
        {
            name[name_index] = characters[i];
            name_index++;
            name_length++;
        }
    }

    // +1 for the terminator character
    name = realloc(name, name_length + 1);
    name[name_length] = '\0';
    return name;
}

void spr_open(const char *path, struct spr_t *spr)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");

    // signature
    assert(fgetc(file) == 0x0);
    assert(fgetc(file) == 0x0);
    assert(fgetc(file) == 0x0);
    assert(fgetc(file) == 0x0);

    // read the ctpk count, pointer, and names pointer
    // this is actually the only count and pointer that is in the opposite order
    uint32_t num_ctpks, ctpks_pointer, ctpk_names_pointer;
    fread(&ctpks_pointer, sizeof(ctpks_pointer), 1, file);
    fread(&num_ctpks, sizeof(num_ctpks), 1, file);
    fread(&ctpk_names_pointer, sizeof(ctpk_names_pointer), 1, file);

    // padding
    for (int i = 0; i < 8; i++)
        assert(fgetc(file) == 0x0);

    // read the scr count and pointer
    uint32_t num_scrs, scrs_pointer;
    fread(&num_scrs, sizeof(num_scrs), 1, file);
    fread(&scrs_pointer, sizeof(scrs_pointer), 1, file);

    // read the textures
    // read the ctpks and then only keep their textures, as thats all thats needed
    // the pointer for each ctpk needs to be advanced by the last
    // as ctpks can be of any length
    spr->num_textures = num_ctpks;
    spr->textures = malloc(num_ctpks * sizeof(struct ctr_texture_t));
    spr->texture_names = malloc(num_ctpks * sizeof(char *));

    uint32_t ctpk_pointer = ctpks_pointer;
    for (int i = 0; i < num_ctpks; i++)
    {
        // seek to the ctpk
        fseek(file, ctpk_pointer, SEEK_SET);

        // read the pointer to the next ctpk
        uint32_t next_pointer;
        fread(&next_pointer, sizeof(next_pointer), 1, file);
        next_pointer += (uint32_t)ftell(file);

        // read the ctpk
        struct ctpk_t ctpk;
        ctpk_open(file, &ctpk);
        assert(ctpk.num_textures == 1);

        // read the name
        fseek(file, ctpk_names_pointer + (i * ctpk_name_allocated_size), SEEK_SET);

        char *name = spr_string_read(file, ctpk_name_allocated_size);

        // advance the ctpk pointer
        ctpk_pointer = next_pointer;

        // insert the texture
        spr->textures[i] = ctpk.textures[0];
        spr->texture_names[i] = name;

        // close the ctpk as its only needed to read the textures
        ctpk_close(&ctpk);
    }

    // read the scrs
    spr->num_scrs = num_scrs;
    spr->scrs = malloc(num_scrs * sizeof(struct scr_t));
    for (int i = 0; i < num_scrs; i++)
    {
        // array
        fseek(file, scrs_pointer + (i * 96), SEEK_SET);

        // read the texture index
        uint8_t texture_index;
        fread(&texture_index, sizeof(texture_index), 1, file);

        // read the name
        char *name = spr_string_read(file, scr_name_allocated_size);

        // read the top left and bottom right coordinates
        struct vec2_t top_left, bottom_right;
        vec2_read(file, &top_left);
        vec2_read(file, &bottom_right);
        assert(bottom_right.x > top_left.x);
        assert(bottom_right.y > top_left.y);

        // read the pixel space uv coordinates and size
        uint16_t x, y, width, height;
        fread(&x, sizeof(x), 1, file);
        fread(&y, sizeof(y), 1, file);
        fread(&width, sizeof(width), 1, file);
        fread(&height, sizeof(height), 1, file);

        // insert the scr
        struct scr_t scr;
        scr.name = name;
        scr.texture_index = texture_index;
        scr.top_left = top_left;
        scr.bottom_right = bottom_right;
        scr.x = x;
        scr.y = y;
        scr.width = width;
        scr.height = height;
        spr->scrs[i] = scr;
    }

    // set the file handle
    spr->file = file;
}

void spr_close(struct spr_t *spr)
{
    for (int i = 0; i < spr->num_textures; i++)
        free(spr->texture_names[i]);

    for (int i = 0; i < spr->num_scrs; i++)
        free(spr->scrs[i].name);

    free(spr->scrs);
    free(spr->texture_names);
    free(spr->textures);
    fclose(spr->file);
}

struct scr_t *spr_lookup(const struct spr_t *spr, char *scr_name)
{
    for (int i = 0; i < spr->num_scrs; i++)
    {
        struct scr_t *scr = &spr->scrs[i];
        if (strcmp(scr->name, scr_name) == 0)
            return scr;
    }

    return NULL;
}
