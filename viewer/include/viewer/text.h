//
//  text.h
//  viewer
//
//  Created by Marika on 2020-02-23.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include "gl.h"
#include "program.h"
#include "vertex_array.h"

// MARK: - Data Structures

/// The data structure for maintaining the state of a single text.
///
/// Text is used to render characters onto a grid, rasterize them, and then draw that to the screen.
struct text_t
{
    /// The width of this text, in characters.
    ///
    /// This is a static size, and characters cannot be placed outside of it.
    unsigned int width;

    /// The height of this text, in characters.
    ///
    /// This is a static size, and characters cannot be placed outside of it.
    unsigned int height;

    /// The unique identifier of this text's backing OpenGL texture.
    GLuint texture_id;

    /// The vertex array containing the quad for drawing this text's backing texture.
    struct vertex_array_t quad_array;
};

// MARK: - Functions

/// Create a new text from the given properties.
///
/// During this function the backing texture is bound to the currently active texture unit as `GL_TEXTURE_2D`.
/// @param width The width of the text, in characters.
/// @param height The height of the text, in characters.
/// @param program2d The program used to render the text, assumed to be the standard.
/// @param text The text to load the created text into.
void text_create(unsigned int width,
                 unsigned int height,
                 const struct program_t *program2d,
                 struct text_t *text);

/// Destroy the given text, releasing all of it's allocated memory.
///
/// This must be called after a text is created and before program execution completes.
/// @param text The text to destroy.
void text_destroy(struct text_t *text);

/// Put the given character at the given position onto the given text.
///
/// It is assumed that the given text's texture is bound to `GL_TEXTURE_2D` on the active texture unit.
/// @param x The X position to put the character, in characters. Top-left origin.
/// @param y The Y position to put the character, in characters. Top-left origin.
/// @param character The character to put.
/// @param text The text to put the character onto.
void text_put_character(unsigned int x,
                        unsigned int y,
                        char character,
                        struct text_t *text);

/// Put the given string at the given position onto the given text.
///
/// It is assumed that the given text's texture is bound to `GL_TEXTURE_2D` on the active texture unit.
/// @param x The X position to put the string, in characters. Top-left origin.
/// @param y The Y position to put the string, in characters. Top-left origin.
/// @param string The string to put.
/// @param text The text to put the string onto.
void text_put_string(unsigned int x,
                     unsigned int y,
                     const char *string,
                     struct text_t *text);

/// Clear the entire contents of the given text.
///
/// It is assumed that the given text's texture is bound to `GL_TEXTURE_2D` on the active texture unit.
/// @param text The text to clear.
void text_clear(struct text_t *text);

/// Draw the given text to the screen.
///
/// It is assumed that the given text's texture is bound to `GL_TEXTURE_2D` on the active texture unit.
/// @param text The text to draw.
void text_draw(struct text_t *text);
