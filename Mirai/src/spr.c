//
//  spr.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// MARK: - Data Structures

/// The data structure for the header of an spr file.
struct spr_header_t
{
    /// The signature for this file, must be `0x0000`.
    uint32_t signature;

    /// The pointer to the beginning of the CTPK data within this file.
    uint32_t ctpks_pointer;

    /// The number of CTPK files within this file..
    uint32_t num_ctpks;

    /// The pointer to the beginning of the CTPK names data within this file..
    uint32_t names_pointer;

    // have no idea what this is, seems to always be all zeroes
    uint8_t unknown[8];

    // unsure what these are, but the layout seems to indicate that theyre uint32s
    // on spr files unk2 seems to always line up with ctpk_pointer
    // but on spr_ae files it appears to be way after, probably after the block that appears to be 3d related data
    // unk2 seems to always line up with the offset of the reader after reading the ctpk names data
    uint32_t unk1;
    uint32_t unk2;
};

// MARK: - Functions

void spr_open(const char *path, struct spr_t *spr)
{
    // open the file for binary reading
    spr->handle = fopen(path, "rb");

    // read the header
    struct spr_header_t header;
    fread(&header, sizeof(struct spr_header_t), 1, spr->handle);
    assert(header.signature == 0x0000);

    // initialize the output file
    spr->num_ctpks = header.num_ctpks;
    spr->ctpk_names = malloc(header.num_ctpks * sizeof(char *));
    spr->ctpks = malloc(header.num_ctpks * sizeof(struct ctpk_t *));

    // read each ctpk file
    unsigned long long names_pointer = header.names_pointer;
    unsigned long long ctpks_pointer = header.ctpks_pointer;
    for (unsigned int i = 0; i < header.num_ctpks; i++)
    {
        // read the name
        // each name is actually within 32 bytes,
        // surrounded by an unknown amount of terminator characters
        // so read the maximum length then get the actual name by reading between the terminators
        const unsigned int name_max_length = 32;
        char name_full[name_max_length];
        fseek(spr->handle, names_pointer, SEEK_SET);
        fread(name_full, sizeof(name_full), 1, spr->handle);

        unsigned int name_start_index = 0;
        unsigned int name_length = 0;
        bool name_start_index_set = false;
        for (unsigned int i = 0; i < name_max_length; i++)
        {
            if (name_full[i] != '\0' && !name_start_index_set)
            {
                name_start_index = i;
                name_start_index_set = true;
            }

            if (name_start_index_set && name_full[i] == '\0')
            {
                name_length = i - name_start_index;
                break;
            }
        }

        // copy the null terminated name into a new string to get the final name
        char *name = malloc(name_length + 1);
        name[name_length] = '\0';
        memcpy(name, name_full + name_start_index, name_length);
        spr->ctpk_names[i] = name;

        // read the ctpk file
        // not sure what this is but theres an additional 4 bytes before the ctpk header
        // so read those and ignore them
        fseek(spr->handle, ctpks_pointer, SEEK_SET);
        fseek(spr->handle, 4, SEEK_CUR);

        struct ctpk_t *ctpk = malloc(sizeof(struct ctpk_t));
        ctpk_open(spr->handle, ctpk);
        spr->ctpks[i] = ctpk;

        // increment the pointers for the next ctpk
        // ctpk pointer needs to be handled specially as it should skip past texture data without reading it
        // but if there are no textures then the next ctpk should be immediately after
        names_pointer += name_max_length;

        if (ctpk->num_textures > 0)
        {
            struct ctpk_texture_t *last_texture = ctpk->textures[ctpk->num_textures - 1];
            ctpks_pointer = last_texture->data_pointer + last_texture->data_size;
        }
        else
        {
            ctpks_pointer = ftell(spr->handle);
        }
    }
}

void spr_close(struct spr_t *spr)
{
    for (unsigned int i = 0; i < spr->num_ctpks; i++)
    {
        free(spr->ctpk_names[i]);
        ctpk_close(spr->ctpks[i]);
    }

    free(spr->ctpk_names);
    free(spr->ctpks);
    fclose(spr->handle);
}
