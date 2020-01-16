//
//  dict.c
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "dict.h"

#include <stdlib.h>
#include <assert.h>

#include "utils.h"

// MARK: - Functions

void dict_open(FILE *file, struct dict_t *dict)
{
    // read the number of entries
    uint32_t num_entries;
    fread(&num_entries, sizeof(num_entries), 1, file);

    // read the dict pointer
    uint32_t dict_pointer = utils_absolute_pointer(file);

    // initialize the dictionary
    dict->num_entries = num_entries;
    dict->entries = malloc(num_entries * sizeof(struct dict_entry_t));

    // only read the dict if there are any entries
    // if there arent then theres no data for it, and the signature check will fail
    if (num_entries > 0)
    {
        // store the current offset and seek to the dict
        // the current offset needs to be stored so that the file handle can
        // return to the end of the item in the list when this dict is read
        long return_offset = ftell(file);
        fseek(file, dict_pointer, SEEK_SET);

        // read the signature
        assert(fgetc(file) == 'D');
        assert(fgetc(file) == 'I');
        assert(fgetc(file) == 'C');
        assert(fgetc(file) == 'T');

        // u32 size of the dict in bytes, unused
        fseek(file, 4, SEEK_CUR);

        // read the entry count
        uint32_t num_entries;
        fread(&num_entries, sizeof(num_entries), 1, file);

        // 16 bytes for a root entry, unused
        fseek(file, 16, SEEK_CUR);

        // read the entries
        for (int i = 0; i < num_entries; i++)
        {
            // series of unused values
            //  - u32 reference bit
            //  - u16 left node
            //  - u16 right node
            // these are possibly for radix trees? not sure
            fseek(file, 8, SEEK_CUR);

            // read the name and data pointers
            uint32_t name_pointer = utils_absolute_pointer(file);
            uint32_t data_pointer = utils_absolute_pointer(file);

            // read the name and seek back
            long name_return = ftell(file);
            fseek(file, name_pointer, SEEK_SET);
            char *name = utils_read_string(file);
            fseek(file, name_return, SEEK_SET);

            // add the entry
            struct dict_entry_t *entry = malloc(sizeof(struct dict_entry_t));
            entry->name = name;
            entry->data_pointer = data_pointer;
            dict->entries[i] = entry;
        }

        // return to the dict list
        fseek(file, return_offset, SEEK_SET);
    }
}

void dict_close(struct dict_t *dict)
{
    for (int i = 0; i < dict->num_entries; i++)
    {
        struct dict_entry_t *entry = dict->entries[i];
        free(entry->name);
        free(entry);
    }

    free(dict->entries);
}
