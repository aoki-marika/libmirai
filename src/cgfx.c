//
//  cgfx.c
//  Mirai
//
//  Created by Marika on 2020-01-14.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "cgfx.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// MARK: - Functions

/// Read the data section at the current offset of the given file handle into the given data structure.
/// @param file The file handle to read the data section from.
/// @param data The data structure to read the file into.
void cgfx_data_read(FILE *file, struct cgfx_data_t *data)
{
    // read the signature
    assert(fgetc(file) == 'D');
    assert(fgetc(file) == 'A');
    assert(fgetc(file) == 'T');
    assert(fgetc(file) == 'A');

    // the next 4 bytes are a u32 for the size of the data section in bytes
    // not used here so skip
    fseek(file, 4, SEEK_CUR);

    // create and read all of the dictionaries
    data->models = malloc(sizeof(struct dict_t));
    data->textures = malloc(sizeof(struct dict_t));
    data->lookup_tables = malloc(sizeof(struct dict_t));
    data->materials = malloc(sizeof(struct dict_t));
    data->shaders = malloc(sizeof(struct dict_t));
    data->cameras = malloc(sizeof(struct dict_t));
    data->lights = malloc(sizeof(struct dict_t));
    data->fogs = malloc(sizeof(struct dict_t));
    data->scenes = malloc(sizeof(struct dict_t));
    data->skeletal_animations = malloc(sizeof(struct dict_t));
    data->material_animations = malloc(sizeof(struct dict_t));
    data->visibility_animations = malloc(sizeof(struct dict_t));
    data->camera_animations = malloc(sizeof(struct dict_t));
    data->light_animations = malloc(sizeof(struct dict_t));
    data->emitters = malloc(sizeof(struct dict_t));

    dict_open(file, data->models);
    dict_open(file, data->textures);
    dict_open(file, data->lookup_tables);
    dict_open(file, data->materials);
    dict_open(file, data->shaders);
    dict_open(file, data->cameras);
    dict_open(file, data->lights);
    dict_open(file, data->fogs);
    dict_open(file, data->scenes);
    dict_open(file, data->skeletal_animations);
    dict_open(file, data->material_animations);
    dict_open(file, data->visibility_animations);
    dict_open(file, data->camera_animations);
    dict_open(file, data->light_animations);
    dict_open(file, data->emitters);
}

void cgfx_open(const char *path, struct cgfx_t *cgfx)
{
    // open the file for binary reading
    cgfx->file = fopen(path, "rb");

    // read the signature
    assert(fgetc(cgfx->file) == 'C');
    assert(fgetc(cgfx->file) == 'G');
    assert(fgetc(cgfx->file) == 'F');
    assert(fgetc(cgfx->file) == 'X');

    // all of the header values are unused here
    // in order, they are:
    //  - a two byte byteorder enum, mirai always uses little endian
    //  - a u16 for the size of the header in bytes, not useful
    //  - a u32 for the cgfx format revision used, not useful
    //  - a u32 for the file size (in bytes,) not useful
    //  - a u32 for the number of entries in the file, not sure what this refers to but dont think its needed
    fseek(cgfx->file, 2 + 2 + 4 + 4 + 4, SEEK_CUR);

    // read the data section
    cgfx->data = malloc(sizeof(struct cgfx_data_t));
    cgfx_data_read(cgfx->file, cgfx->data);

    // read the data structures for each data section
    // models
    cgfx->num_models = cgfx->data->models->num_entries;
    cgfx->models = malloc(cgfx->num_models * sizeof(struct cmdl_t *));
    for (int i = 0; i < cgfx->num_models; i++)
    {
        struct dict_entry_t *entry = cgfx->data->models->entries[i];

        // read the cmdl
        struct cmdl_t *cmdl = malloc(sizeof(struct cmdl_t));
        fseek(cgfx->file, entry->data_pointer, SEEK_SET);
        cmdl_open(cgfx->file, cmdl);

        // insert the cmdl
        cgfx->models[i] = cmdl;
    }

    // textures
    cgfx->num_textures = cgfx->data->textures->num_entries;
    cgfx->textures = malloc(cgfx->num_textures * sizeof(struct txob_t *));
    for (int i = 0; i < cgfx->num_textures; i++)
    {
        struct dict_entry_t *entry = cgfx->data->textures->entries[i];

        // read the txob
        struct txob_t *txob = malloc(sizeof(struct txob_t));
        fseek(cgfx->file, entry->data_pointer, SEEK_SET);
        txob_open(cgfx->file, txob);

        // insert the txob
        cgfx->textures[i] = txob;
    }
}

void cgfx_close(struct cgfx_t *cgfx)
{
    for (int i = 0; i < cgfx->num_models; i++)
    {
        cmdl_close(cgfx->models[i]);
        free(cgfx->models[i]);
    }

    for (int i = 0; i < cgfx->num_textures; i++)
    {
        txob_close(cgfx->textures[i]);
        free(cgfx->textures[i]);
    }

    dict_close(cgfx->data->models);
    dict_close(cgfx->data->textures);
    dict_close(cgfx->data->lookup_tables);
    dict_close(cgfx->data->materials);
    dict_close(cgfx->data->shaders);
    dict_close(cgfx->data->cameras);
    dict_close(cgfx->data->lights);
    dict_close(cgfx->data->fogs);
    dict_close(cgfx->data->scenes);
    dict_close(cgfx->data->skeletal_animations);
    dict_close(cgfx->data->material_animations);
    dict_close(cgfx->data->visibility_animations);
    dict_close(cgfx->data->camera_animations);
    dict_close(cgfx->data->light_animations);
    dict_close(cgfx->data->emitters);

    free(cgfx->data->models);
    free(cgfx->data->textures);
    free(cgfx->data->lookup_tables);
    free(cgfx->data->materials);
    free(cgfx->data->shaders);
    free(cgfx->data->cameras);
    free(cgfx->data->lights);
    free(cgfx->data->fogs);
    free(cgfx->data->scenes);
    free(cgfx->data->skeletal_animations);
    free(cgfx->data->material_animations);
    free(cgfx->data->visibility_animations);
    free(cgfx->data->camera_animations);
    free(cgfx->data->light_animations);
    free(cgfx->data->emitters);
    free(cgfx->data);
    free(cgfx->textures);
    free(cgfx->models);
    fclose(cgfx->file);
}
