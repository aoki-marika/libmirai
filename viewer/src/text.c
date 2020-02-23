//
//  text.c
//  viewer
//
//  Created by Marika on 2020-02-23.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "text.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "constants.h"

// MARK: - Constants

/// The width of a single character within a text, in pixels.
const unsigned int text_character_width = 8;

/// The height of a single character within a text, in pixels.
const unsigned int text_character_height = 8;

/// The number of pixels that the cursor advances horizontally for each drawn character within a text.
const unsigned int text_character_advance = 6;

// MARK: - Functions

/// Get the 1bpp (1 bit per pixel) template for the given character.
///
/// Note that only a very limited character set is supported, that being `A-Z`, `0-9`, `_`, and ` `.
/// @param character The character to get the template of.
/// @param out The array to load the template into. Must be be at least `text_character_height` in length.
void text_character_template(char character, uint8_t *out)
{
    switch (character)
    {
        case 'A':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00111100;
            out[5] = 0b00100100;
            out[6] = 0b00100100;
            out[7] = 0b00000000;
            break;
        case 'B':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00100100;
            out[3] = 0b00111000;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case 'C':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00100000;
            out[4] = 0b00100000;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case 'D':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case 'E':
            out[0] = 0b00000000;
            out[1] = 0b00111100;
            out[2] = 0b00100000;
            out[3] = 0b00111000;
            out[4] = 0b00100000;
            out[5] = 0b00100000;
            out[6] = 0b00111100;
            out[7] = 0b00000000;
            break;
        case 'F':
            out[0] = 0b00000000;
            out[1] = 0b00111100;
            out[2] = 0b00100000;
            out[3] = 0b00111000;
            out[4] = 0b00100000;
            out[5] = 0b00100000;
            out[6] = 0b00100000;
            out[7] = 0b00000000;
            break;
        case 'G':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00100000;
            out[4] = 0b00101100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case 'H':
            out[0] = 0b00000000;
            out[1] = 0b00100100;
            out[2] = 0b00100100;
            out[3] = 0b00111100;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00100100;
            out[7] = 0b00000000;
            break;
        case 'I':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00010000;
            out[3] = 0b00010000;
            out[4] = 0b00010000;
            out[5] = 0b00010000;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case 'J':
            out[0] = 0b00000000;
            out[1] = 0b00000100;
            out[2] = 0b00000100;
            out[3] = 0b00000100;
            out[4] = 0b00000100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case 'K':
            out[0] = 0b00000000;
            out[1] = 0b00101000;
            out[2] = 0b00101000;
            out[3] = 0b00110000;
            out[4] = 0b00101000;
            out[5] = 0b00100100;
            out[6] = 0b00100100;
            out[7] = 0b00000000;
            break;
        case 'L':
            out[0] = 0b00000000;
            out[1] = 0b00100000;
            out[2] = 0b00100000;
            out[3] = 0b00100000;
            out[4] = 0b00100000;
            out[5] = 0b00100000;
            out[6] = 0b00111100;
            out[7] = 0b00000000;
            break;
        case 'M':
            out[0] = 0b00000000;
            out[1] = 0b01000100;
            out[2] = 0b01101100;
            out[3] = 0b01010100;
            out[4] = 0b01000100;
            out[5] = 0b01000100;
            out[6] = 0b01000100;
            out[7] = 0b00000000;
            break;
        case 'N':
            out[0] = 0b00000000;
            out[1] = 0b00100100;
            out[2] = 0b00110100;
            out[3] = 0b00110100;
            out[4] = 0b00101100;
            out[5] = 0b00101100;
            out[6] = 0b00100100;
            out[7] = 0b00000000;
            break;
        case 'O':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case 'P':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00111000;
            out[5] = 0b00100000;
            out[6] = 0b00100000;
            out[7] = 0b00000000;
            break;
        case 'Q':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00011100;
            out[7] = 0b00000010;
            break;
        case 'R':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00111000;
            out[5] = 0b00100100;
            out[6] = 0b00100100;
            out[7] = 0b00000000;
            break;
        case 'S':
            out[0] = 0b00000000;
            out[1] = 0b00011100;
            out[2] = 0b00100000;
            out[3] = 0b00011000;
            out[4] = 0b00000100;
            out[5] = 0b00000100;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case 'T':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00010000;
            out[3] = 0b00010000;
            out[4] = 0b00010000;
            out[5] = 0b00010000;
            out[6] = 0b00010000;
            out[7] = 0b00000000;
            break;
        case 'U':
            out[0] = 0b00000000;
            out[1] = 0b00100100;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case 'V':
            out[0] = 0b00000000;
            out[1] = 0b00100100;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00100100;
            out[5] = 0b00010100;
            out[6] = 0b00001100;
            out[7] = 0b00000000;
            break;
        case 'W':
            out[0] = 0b00000000;
            out[1] = 0b01000100;
            out[2] = 0b01000100;
            out[3] = 0b01000100;
            out[4] = 0b01010100;
            out[5] = 0b01101100;
            out[6] = 0b01000100;
            out[7] = 0b00000000;
            break;
        case 'X':
            out[0] = 0b00000000;
            out[1] = 0b00100100;
            out[2] = 0b00100100;
            out[3] = 0b00010000;
            out[4] = 0b00001000;
            out[5] = 0b00100100;
            out[6] = 0b00100100;
            out[7] = 0b00000000;
            break;
        case 'Y':
            out[0] = 0b00000000;
            out[1] = 0b01000100;
            out[2] = 0b01000100;
            out[3] = 0b00101000;
            out[4] = 0b00010000;
            out[5] = 0b00010000;
            out[6] = 0b00010000;
            out[7] = 0b00000000;
            break;
        case 'Z':
            out[0] = 0b00000000;
            out[1] = 0b00111100;
            out[2] = 0b00000100;
            out[3] = 0b00001000;
            out[4] = 0b00010000;
            out[5] = 0b00100000;
            out[6] = 0b00111100;
            out[7] = 0b00000000;
            break;
        case '0':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00101100;
            out[4] = 0b00110100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case '1':
            out[0] = 0b00000000;
            out[1] = 0b00010000;
            out[2] = 0b00110000;
            out[3] = 0b00010000;
            out[4] = 0b00010000;
            out[5] = 0b00010000;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case '2':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00000100;
            out[4] = 0b00011000;
            out[5] = 0b00100000;
            out[6] = 0b00111100;
            out[7] = 0b00000000;
            break;
        case '3':
            out[0] = 0b00000000;
            out[1] = 0b00111000;
            out[2] = 0b00000100;
            out[3] = 0b00011000;
            out[4] = 0b00000100;
            out[5] = 0b00000100;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case '4':
            out[0] = 0b00000000;
            out[1] = 0b00100000;
            out[2] = 0b00101000;
            out[3] = 0b00101000;
            out[4] = 0b00111100;
            out[5] = 0b00001000;
            out[6] = 0b00001000;
            out[7] = 0b00000000;
            break;
        case '5':
            out[0] = 0b00000000;
            out[1] = 0b00111100;
            out[2] = 0b00100000;
            out[3] = 0b00111000;
            out[4] = 0b00000100;
            out[5] = 0b00000100;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case '6':
            out[0] = 0b00000000;
            out[1] = 0b00011100;
            out[2] = 0b00100000;
            out[3] = 0b00111000;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case '7':
            out[0] = 0b00000000;
            out[1] = 0b00111100;
            out[2] = 0b00000100;
            out[3] = 0b00001000;
            out[4] = 0b00001000;
            out[5] = 0b00001000;
            out[6] = 0b00001000;
            out[7] = 0b00000000;
            break;
        case '8':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00011000;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00011000;
            out[7] = 0b00000000;
            break;
        case '9':
            out[0] = 0b00000000;
            out[1] = 0b00011000;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00011100;
            out[5] = 0b00000100;
            out[6] = 0b00111000;
            out[7] = 0b00000000;
            break;
        case '_':
            out[0] = 0b00000000;
            out[1] = 0b00000000;
            out[2] = 0b00000000;
            out[3] = 0b00000000;
            out[4] = 0b00000000;
            out[5] = 0b00000000;
            out[6] = 0b00111100;
            out[7] = 0b00000000;
            break;
        case ' ':
            out[0] = 0b00000000;
            out[1] = 0b00000000;
            out[2] = 0b00000000;
            out[3] = 0b00000000;
            out[4] = 0b00000000;
            out[5] = 0b00000000;
            out[6] = 0b00000000;
            out[7] = 0b00000000;
            break;
        // use an empty rectangle for missing characters
        default:
            out[0] = 0b00000000;
            out[1] = 0b00111100;
            out[2] = 0b00100100;
            out[3] = 0b00100100;
            out[4] = 0b00100100;
            out[5] = 0b00100100;
            out[6] = 0b00111100;
            out[7] = 0b00000000;
            break;
    }
}

void text_create(unsigned int width,
                 unsigned int height,
                 const struct program_t *program2d,
                 struct text_t *text)
{
    // create empty base data for the texture
    const unsigned int w = width * text_character_advance;
    const unsigned int h = height * text_character_height;
    uint8_t *empty_data = calloc(1, w * h * 4); //*4 for rgba channels

    // create the texture
    glGenTextures(1, &text->texture_id);
    glBindTexture(GL_TEXTURE_2D, text->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 width * text_character_advance,
                 height * text_character_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 empty_data);

    // free the empty data now that its uploaded
    free(empty_data);

    // create the vertex array
    const int vertex_stride = 4 * sizeof(float);
    const struct vertex_component_t vertex_components[] =
    {
        {
            .data_type = GL_FLOAT,
            .num_values = 2,
            .offset = 0 * sizeof(float),
            .stride = vertex_stride,
            .attribute_name = PROGRAM_2D_COMPONENT_POSITION,
        },
        {
            .data_type = GL_FLOAT,
            .num_values = 2,
            .offset = 2 * sizeof(float),
            .stride = vertex_stride,
            .attribute_name = PROGRAM_2D_COMPONENT_TEXCOORD,
        },
    };

    const float vertices[] =
    {
    //  x  y    u    v
        0, 0, 0.0, 1.0, //top-left
        0, h, 0.0, 0.0, //bottom-left
        w, h, 1.0, 0.0, //bottom-right

        0, 0, 0.0, 1.0, //top-left
        w, 0, 1.0, 1.0, //top-right
        w, h, 1.0, 0.0, //bottom-right
    };

    vertex_array_create(vertices,
                        sizeof(vertices),
                        sizeof(vertex_components) / sizeof(struct vertex_component_t),
                        vertex_components,
                        program2d,
                        &text->quad_array);

    // initialize the text
    text->width = width;
    text->height = height;
}

void text_destroy(struct text_t *text)
{
    vertex_array_destroy(&text->quad_array);
    glDeleteTextures(1, &text->texture_id);
}

void text_put_character(unsigned int x,
                        unsigned int y,
                        char character,
                        struct text_t *text)
{
    assert(x < text->width);
    assert(y < text->height);

    // get the template
    uint8_t template[text_character_height];
    text_character_template(character, template);

    // translate the template into rgba data
    uint8_t data[text_character_width * text_character_height * 4];
    for (int byte = 0; byte < text_character_height; byte++)
    {
        for (int bit = 0; bit < 8; bit++)
        {
            // the bit needs to be reversed so the data is ordered left to right
            // the byte also needs to be flipped when getting the pixel offset
            // so that the rows are ordered bottom to top, not top to bottom
            uint8_t on = (template[byte] << bit) & (0b1 << 7);
            int pixel = ((7 - byte) * text_character_height) + bit;
            int offset = pixel * 4;

            if (on)
            {
                data[offset + 0] = 0xff;
                data[offset + 1] = 0xff;
                data[offset + 2] = 0xff;
                data[offset + 3] = 0xff;
            }
            else
            {
                data[offset + 0] = 0x00;
                data[offset + 1] = 0x00;
                data[offset + 2] = 0x00;
                data[offset + 3] = 0xff;
            }
        }
    }

    // blit on the character data
    // the y coordinate needs to be inversed to have a top-left origin
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    x * text_character_advance,
                    (text->height * text_character_height) - text_character_height - (y * text_character_height),
                    text_character_width,
                    text_character_height,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    data);
}

void text_put_string(unsigned int x,
                     unsigned int y,
                     const char *string,
                     struct text_t *text)
{
    for (int i = 0; i < strlen(string); i++)
        text_put_character(x + i, y, string[i], text);
}

void text_draw(struct text_t *text)
{
    glBindVertexArray(text->quad_array.id);
    glDrawArrays(GL_TRIANGLES, 0, 6); //6 vertices per quad
}
