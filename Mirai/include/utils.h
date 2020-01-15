//
//  utils.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Functions

/// Read the relative offset at the current offset of the given file handle, and convert it to an absolute pointer within the file.
///
/// Some 3DS file formats use this for pointers, where there is an offset u32 for the pointer, which is relative to the beginning of said u32.
/// So to get the absolute offset of that offset within the file, it can add the offset to the current offset of the file handle before reading the offset.
/// @param file The file handle to read the offset from and use the base offset of.
/// @returns The absolute pointer within the file of the offset at the current offset of the given file handle.
uint32_t utils_absolute_pointer(FILE *file);
