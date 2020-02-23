//
//  texture.h
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <mirai/ctr_texture.h>

#include "gl.h"

// MARK: - Functions

/// Decode and upload the given texture's data to an OpenGL texture.
/// @param texture The texture to decode and upload the data of.
/// @param file The file handle containing the data of the given texture.
/// @param id The value to set to the created OpenGL texture's ID.
void texture_upload(struct ctr_texture_t *texture, FILE *file, GLuint *id);
