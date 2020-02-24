//
//  texture.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "texture.h"

#include <stdlib.h>
#include <string.h>

// MARK: - Functions

void texture_upload(struct ctr_texture_t *texture, FILE *file, GLuint *id)
{
    // create the texture
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // decode and unpack the texture data
    uint8_t *decoded_data = ctr_texture_decode(texture, file);
    uint8_t *unpacked_data = ctr_texture_unpack(texture, decoded_data);

    // flip the texture data
    // as the decoded/unpacked data is left to right rows from top to bottom,
    // and opengl is left to right rows from bottom to top,
    // all the rows have to be flipped
    unsigned int bpp = (unsigned int)texture->unpacked_data_size / (texture->width * texture->height);
    unsigned int bpr = bpp * texture->width;
    uint8_t flipped_data[texture->unpacked_data_size];
    for (int i = 0; i < texture->unpacked_data_size / bpr; i++)
        memcpy(&flipped_data[i * bpr], &unpacked_data[texture->unpacked_data_size - ((i + 1) * bpr)], bpr);

    // upload the texture data
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 texture->width,
                 texture->height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 flipped_data);

    // the data is already uploaded, so release it
    free(unpacked_data);
    free(decoded_data);
}
