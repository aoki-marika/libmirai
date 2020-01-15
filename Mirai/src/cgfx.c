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

#include "utils.h"

// MARK: - Functions

/// Read the dictionary entry at the current offset of the given file handle into the given data structure.
/// @param file The file handle to read the entry from.
/// @param entry The data structure to read the file into.
void cgfx_dict_entry_read(FILE *file, struct cgfx_dict_entry_t *entry)
{
    // first 8 bytes are values that are unused, skip them
    // namely reference bit, left node, and right node
    fseek(file, 8, SEEK_CUR);

    // read the name and data pointers
    uint32_t name_pointer = utils_absolute_pointer(file);
    uint32_t data_pointer = utils_absolute_pointer(file);

    // read the name
    // the name is a null terminated string at the name pointer,
    // so seek, read, and then seek back
    long seek_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);

    char name_full[256];
    int name_length = 0;
    while (1)
    {
        char c = fgetc(file);
        name_full[name_length] = c;
        name_length++;

        if (c == '\0')
            break;
    }

    char *name = malloc(name_length);
    memcpy(name, name_full, name_length);
    fseek(file, seek_return, SEEK_SET);

    // create the entry
    entry->name = name;
    entry->data_pointer = data_pointer;
}

/// Read the dictionary at the current offset of the given file handle into the given data structure.
///
/// Note that this is for dictionary lists, where it first reads the size and offset of the dictionary, then seeks to and reads the actual dictionary, then returns to the next dictionary in the list.
/// When this function returns the file offset is advanced by two u32s.
/// @param file The file handle to read the dictionary from.
/// @param dict The data structure to read the file into.
void cgfx_dict_read(FILE *file, struct cgfx_dict_t *dict)
{
    // read the number of entries in the dictionary
    uint32_t num_entries;
    fread(&num_entries, sizeof(num_entries), 1, file);

    // read the offset of the dictionary, in bytes
    uint32_t pointer = utils_absolute_pointer(file);

    // initialize the dictionary
    dict->num_entries = num_entries;
    dict->entries = malloc(num_entries * sizeof(struct cgfx_dict_entry_t));

    // only read the dictionary if there are any entries
    // if there arent then theres no data for it, and the signature check will fail
    if (num_entries > 0)
    {
        // store the current offset and seek to the dictionary
        // the current offset needs to be stored so it can be returned to to read the next in the list
        long return_offset = ftell(file);
        fseek(file, pointer, SEEK_SET);

        // read the signature
        assert(fgetc(file) == 'D');
        assert(fgetc(file) == 'I');
        assert(fgetc(file) == 'C');
        assert(fgetc(file) == 'T');

        // read the size and entry count
        uint32_t size, num_entries;
        fread(&size, sizeof(size), 1, file);
        fread(&num_entries, sizeof(num_entries), 1, file);

        // the next 16 bytes are an unused root entry, skip it
        fseek(file, 16, SEEK_CUR);

        // read the entries
        for (int i = 0; i < num_entries; i++)
        {
            struct cgfx_dict_entry_t *entry = malloc(sizeof(struct cgfx_dict_entry_t));
            cgfx_dict_entry_read(file, entry);
            dict->entries[i] = entry;
        }

        // return to the dictionary list
        fseek(file, return_offset, SEEK_SET);
    }
}

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
    data->models = malloc(sizeof(struct cgfx_dict_t));
    data->textures = malloc(sizeof(struct cgfx_dict_t));
    data->lookup_tables = malloc(sizeof(struct cgfx_dict_t));
    data->materials = malloc(sizeof(struct cgfx_dict_t));
    data->shaders = malloc(sizeof(struct cgfx_dict_t));
    data->cameras = malloc(sizeof(struct cgfx_dict_t));
    data->lights = malloc(sizeof(struct cgfx_dict_t));
    data->fogs = malloc(sizeof(struct cgfx_dict_t));
    data->scenes = malloc(sizeof(struct cgfx_dict_t));
    data->skeletal_animations = malloc(sizeof(struct cgfx_dict_t));
    data->material_animations = malloc(sizeof(struct cgfx_dict_t));
    data->visibility_animations = malloc(sizeof(struct cgfx_dict_t));
    data->camera_animations = malloc(sizeof(struct cgfx_dict_t));
    data->light_animations = malloc(sizeof(struct cgfx_dict_t));
    data->emitters = malloc(sizeof(struct cgfx_dict_t));

    cgfx_dict_read(file, data->models);
    cgfx_dict_read(file, data->textures);
    cgfx_dict_read(file, data->lookup_tables);
    cgfx_dict_read(file, data->materials);
    cgfx_dict_read(file, data->shaders);
    cgfx_dict_read(file, data->cameras);
    cgfx_dict_read(file, data->lights);
    cgfx_dict_read(file, data->fogs);
    cgfx_dict_read(file, data->scenes);
    cgfx_dict_read(file, data->skeletal_animations);
    cgfx_dict_read(file, data->material_animations);
    cgfx_dict_read(file, data->visibility_animations);
    cgfx_dict_read(file, data->camera_animations);
    cgfx_dict_read(file, data->light_animations);
    cgfx_dict_read(file, data->emitters);
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
}

/// Free the given dictionary, releasing all of it's allocated memory.
/// @param dict The dictionary to free.
void cgfx_dict_free(struct cgfx_dict_t *dict)
{
    for (int i = 0; i < dict->num_entries; i++)
    {
        struct cgfx_dict_entry_t *entry = dict->entries[i];
        free(entry->name);
        free(entry);
    }

    free(dict->entries);
}

void cgfx_close(struct cgfx_t *cgfx)
{
    cgfx_dict_free(cgfx->data->models);
    cgfx_dict_free(cgfx->data->textures);
    cgfx_dict_free(cgfx->data->lookup_tables);
    cgfx_dict_free(cgfx->data->materials);
    cgfx_dict_free(cgfx->data->shaders);
    cgfx_dict_free(cgfx->data->cameras);
    cgfx_dict_free(cgfx->data->lights);
    cgfx_dict_free(cgfx->data->fogs);
    cgfx_dict_free(cgfx->data->scenes);
    cgfx_dict_free(cgfx->data->skeletal_animations);
    cgfx_dict_free(cgfx->data->material_animations);
    cgfx_dict_free(cgfx->data->visibility_animations);
    cgfx_dict_free(cgfx->data->camera_animations);
    cgfx_dict_free(cgfx->data->light_animations);
    cgfx_dict_free(cgfx->data->emitters);

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
    fclose(cgfx->file);
}
