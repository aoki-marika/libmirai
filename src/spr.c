//
//  spr.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr.h"

#include <stdlib.h>
#include <assert.h>

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

    // read the ctpks
    // the pointer for each ctpk needs to be advanced by the last
    // as ctpks can be of any length
    spr->num_ctpks = num_ctpks;
    spr->ctpks = malloc(num_ctpks * sizeof(struct ctpk_t));
    spr->ctpk_names = malloc(num_ctpks * sizeof(char *));

    size_t ctpk_pointer = ctpks_pointer;
    for (int i = 0; i < num_ctpks; i++)
    {
        // read the ctpk
        // +4 to skip the flags
        fseek(file, ctpk_pointer + 4, SEEK_SET);

        struct ctpk_t ctpk;
        ctpk_open(file, &ctpk);

        // read the name
        fseek(file, ctpk_names_pointer + (i * ctpk_name_allocated_size), SEEK_SET);

        char *name = spr_string_read(file, ctpk_name_allocated_size);

        // advance the ctpk pointer
        // use the end of the last texture to advance the pointer
        // as ctpks are variable in size
        assert(ctpk.num_textures > 0);

        struct texture_t *last_texture = &ctpk.textures[ctpk.num_textures - 1];
        ctpk_pointer = last_texture->data_pointer + last_texture->data_size;

        // insert the ctpk
        spr->ctpks[i] = ctpk;
        spr->ctpk_names[i] = name;
    }

    // read the scrs
    spr->num_scrs = num_scrs;
    spr->scrs = malloc(num_scrs * sizeof(struct scr_t));
    for (int i = 0; i < num_scrs; i++)
    {
        // array
        fseek(file, scrs_pointer + (i * 96), SEEK_SET);

        // read the ctpk index
        uint8_t ctpk_index;
        fread(&ctpk_index, sizeof(ctpk_index), 1, file);

        // read the name
        char *name = spr_string_read(file, scr_name_allocated_size);

        // read the top left and bottom right coordinates
        struct vec2_t top_left, bottom_right;
        vec2_read(file, &top_left);
        vec2_read(file, &bottom_right);
        assert(bottom_right.x > top_left.x);
        assert(bottom_right.y > top_left.y);

        // 8x byte unknown
        fseek(file, 8, SEEK_CUR);

        // insert the scr
        struct scr_t scr;
        scr.name = name;
        scr.ctpk_index = ctpk_index;
        scr.top_left = top_left;
        scr.bottom_right = bottom_right;
        spr->scrs[i] = scr;
    }

    // set the file handle
    spr->file = file;
}

void spr_close(struct spr_t *spr)
{
    for (int i = 0; i < spr->num_ctpks; i++)
        free(spr->ctpk_names[i]);

    for (int i = 0; i < spr->num_scrs; i++)
        free(spr->scrs[i].name);

    free(spr->scrs);
    free(spr->ctpk_names);
    free(spr->ctpks);
    fclose(spr->file);
}
