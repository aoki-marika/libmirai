//
//  texture.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "texture.h"

#include <stdlib.h>

// MARK: - Functions

/// Decode the given texture format into it's OpenGL enums.
/// @param texture_format The texture format to decode.
/// @param internal_format A pointer to the value to assign the internal format to.
/// @param format A pointer to the value to assign the format to.
/// @param type A pointer to the value to assign the type to.
void texture_format_decode(enum texture_format_t texture_format,
                           GLenum *internal_format,
                           GLenum *format,
                           GLenum *type)
{
    switch (texture_format)
    {
        case TEXTURE_FORMAT_RGBA8888:
            *internal_format = GL_RGBA8;
            *format = GL_RGBA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_RGB888:
            *internal_format = GL_RGB8;
            *format = GL_RGB;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_RGBA5551:
            *internal_format = GL_RGB5_A1;
            *format = GL_RGBA;
            *type = GL_UNSIGNED_SHORT_5_5_5_1;
            break;
        case TEXTURE_FORMAT_RGB565:
            *internal_format = GL_RGB565;
            *format = GL_RGB;
            *type = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case TEXTURE_FORMAT_RGBA4444:
            *internal_format = GL_RGBA4;
            *format = GL_RGBA;
            *type = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case TEXTURE_FORMAT_LA88:
            *internal_format = GL_LUMINANCE8_ALPHA8;
            *format = GL_LUMINANCE_ALPHA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_HL8:
            fprintf(stderr, "WARNING: unable to decode texture format HL8 properly\n");
            *internal_format = GL_LUMINANCE8_ALPHA8;
            *format = GL_LUMINANCE_ALPHA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_L8:
            *internal_format = GL_LUMINANCE8;
            *format = GL_LUMINANCE;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_A8:
            *internal_format = GL_ALPHA8;
            *format = GL_ALPHA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_LA44:
            *internal_format = GL_LUMINANCE4_ALPHA4;
            *format = GL_LUMINANCE_ALPHA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_L4:
            *internal_format = GL_LUMINANCE4;
            *format = GL_LUMINANCE;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_A4:
            *internal_format = GL_ALPHA4;
            *format = GL_ALPHA;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_ETC1:
            *internal_format = GL_RGB8;
            *format = GL_RGB;
            *type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_FORMAT_ETC1_A4:
            *internal_format = GL_RGBA8;
            *format = GL_RGBA;
            *type = GL_UNSIGNED_BYTE;
            break;
    }
}

void texture_upload(struct texture_t *texture, FILE *file, GLuint *id)
{
    // create the texture
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // upload the data
    GLenum internal_format, format, type;
    texture_format_decode(texture->data_format,
                          &internal_format,
                          &format,
                          &type);

    uint8_t *decoded_data = texture_decode(texture, file);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internal_format,
                 texture->width,
                 texture->height,
                 0,
                 format,
                 type,
                 decoded_data);

    // the data is already uploaded, so release it
    free(decoded_data);
}
