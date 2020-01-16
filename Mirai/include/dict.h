//
//  dict.h
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// The data structure for a DICT file that has been opened.
struct dict_t
{
    /// The number of entries within this dictionary.
    unsigned int num_entries;

    /// All of the entries within this dictionary.
    ///
    /// The array and each entry are allocated.
    struct dict_entry_t **entries;
};

/// The data structure for a single entry within a DICT file.
struct dict_entry_t
{
    /// The name of this entry.
    ///
    /// Allocated.
    char *name;

    /// The offset of this entry's data within the DICT file, in bytes.
    size_t data_pointer;
};

// MARK: - Functions

/// Open the DICT file at the current offset of the given file handle into the given DICT.
///
/// Note that this is for opening an entry within a DICT list, so the offset and number of entries
/// are read from the current offset of the given file handle, then if there are any entries it seeks
/// to the actual DICT file and reads it.
///
/// The file handle is only advanced by the size of one entry (8 bytes) when this function returns.
/// @param file The file handle to open the DICT file from.
/// @param dict The DICT to open the file into.
void dict_open(FILE *file, struct dict_t *dict);

/// Close the given DICT, releasing all of it's allocated memory.
///
/// This must be called after a DICT is opened and before program execution completes.
/// @param dict The DICT to close.
void dict_close(struct dict_t *dict);
