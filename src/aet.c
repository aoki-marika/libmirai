//
//  aet.c
//  libmirai
//
//  Created by Marika on 2020-01-28.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "aet.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.h"

// MARK: - Functions

/// Read the sprite at the current position of the given file into the given sprite.
/// @param file The file to read the sprite from.
/// @param sprite The sprite to read the file into.
void aet_sprite_read(FILE *file, struct aet_sprite_t *sprite)
{
    // read the scr index
    uint32_t scr_index;
    fread(&scr_index, sizeof(scr_index), 1, file);

    // padding
    for (int i = 0; i < 8; i++)
        assert(fgetc(file) == 0x0);

    // 2x float unknown
    fseek(file, 2 * 4, SEEK_CUR);

    // initialize the sprite
    sprite->scr_index = scr_index;
}

/// Read the sprite group at the current position of the given file into the given sprite group.
/// @param file The file to read the sprite group from.
/// @param sprite_group The sprite group to read the file into.
void aet_sprite_group_read(FILE *file, struct aet_sprite_group_t *sprite_group)
{
    // read the multiply colour
    // only the rgb channels are available, so force full opacity
    uint8_t multiply_r, multiply_g, multiply_b;
    fread(&multiply_r, sizeof(multiply_r), 1, file);
    fread(&multiply_g, sizeof(multiply_g), 1, file);
    fread(&multiply_b, sizeof(multiply_b), 1, file);

    // padding
    assert(fgetc(file) == 0x0);

    // read the size
    uint16_t width, height;
    fread(&width, sizeof(width), 1, file);
    fread(&height, sizeof(height), 1, file);

    // 1x float unknown
    fseek(file, 4, SEEK_CUR);

    // read the sprite count and pointer
    uint32_t num_sprites, sprites_pointer;
    fread(&num_sprites, sizeof(num_sprites), 1, file);
    fread(&sprites_pointer, sizeof(sprites_pointer), 1, file);

    // initialize the sprite group
    sprite_group->multiply_color.r = (float)multiply_r / (float)UINT8_MAX;
    sprite_group->multiply_color.g = (float)multiply_g / (float)UINT8_MAX;
    sprite_group->multiply_color.b = (float)multiply_b / (float)UINT8_MAX;
    sprite_group->multiply_color.a = 1;
    sprite_group->width = width;
    sprite_group->height = height;

    // read the sprites
    sprite_group->num_sprites = num_sprites;
    sprite_group->sprites = malloc(num_sprites * sizeof(struct aet_sprite_t));
    for (int i = 0; i < num_sprites; i++)
    {
        // array
        fseek(file, sprites_pointer + (i * 20), SEEK_SET);

        // insert the sprite
        struct aet_sprite_t sprite;
        aet_sprite_read(file, &sprite);
        sprite_group->sprites[i] = sprite;
    }
}

/// Read the layer at the current position of the given file into the given layer.
/// @param file The file to read the layer from.
/// @param layer The layer to read the file into.
void aet_layer_read(FILE *file, struct aet_layer_t *layer)
{
    // read the name and seek back
    uint32_t name_pointer;
    fread(&name_pointer, sizeof(name_pointer), 1, file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // read the timeline properties
    // third float is unknown, skip it
    float timeline_start_frame, timeline_end_frame, timeline_speed;
    fread(&timeline_start_frame, sizeof(timeline_start_frame), 1, file);
    fread(&timeline_end_frame, sizeof(timeline_end_frame), 1, file);
    fseek(file, 4, SEEK_CUR);
    fread(&timeline_speed, sizeof(timeline_speed), 1, file);

    // unknown values
    //  - 1x u16 flags unknown
    //  - 1x u8 unknown
    fseek(file, 2 + 1, SEEK_CUR);

    // read the type and source pointer
    // init 0x0 and fread sizeof(u8) as sizeof(enum) is 4
    enum aet_layer_type_t type = 0x0;
    fread(&type, sizeof(uint8_t), 1, file);

    uint32_t source_pointer;
    fread(&source_pointer, sizeof(source_pointer), 1, file);

    // u32 parent layer pointer, unused
    fseek(file, 4, SEEK_CUR);

    // read the marker count and pointer
    uint32_t num_markers, markers_pointer;
    fread(&num_markers, sizeof(num_markers), 1, file);
    fread(&markers_pointer, sizeof(markers_pointer), 1, file);

    // read the properties pointer
    uint32_t properties_pointer;
    fread(&properties_pointer, sizeof(properties_pointer), 1, file);

    // padding
    for (int i = 0; i < 4; i++)
        assert(fgetc(file) == 0x0);

    // initialize the layer
    layer->name = name;
    layer->timeline_start_frame = timeline_start_frame;
    layer->timeline_end_frame = timeline_end_frame;
    layer->timeline_speed = timeline_speed;

    // read the source
    layer->type = type;
    switch (type)
    {
        case AET_LAYER_TYPE_SOURCE_SPRITE_GROUP:
        {
            // read the sprite group
            fseek(file, source_pointer, SEEK_SET);

            struct aet_sprite_group_t *sprite_group = malloc(sizeof(struct aet_sprite_group_t));
            aet_sprite_group_read(file, sprite_group);

            // set the layers source
            layer->sprite_group = sprite_group;
            layer->num_children = 0;
            layer->children = NULL;
            break;
        }
        case AET_LAYER_TYPE_NULL_OBJECT:
        {
            // read the child count and pointer
            fseek(file, source_pointer, SEEK_SET);

            uint32_t num_children, children_pointer;
            fread(&num_children, sizeof(num_children), 1, file);
            fread(&children_pointer, sizeof(children_pointer), 1, file);

            // read the children and set the layers source
            layer->sprite_group = NULL;
            layer->num_children = num_children;
            layer->children = malloc(num_children * sizeof(struct aet_layer_t));
            for (int i = 0; i < num_children; i++)
            {
                // array
                fseek(file, children_pointer + (i * 48), SEEK_SET);

                // read the layer
                struct aet_layer_t child;
                aet_layer_read(file, &child);

                // insert the child
                layer->children[i] = child;
            }
            break;
        }
        default:
            // should never be reached
            assert(0);
    }

    // read the markers
    layer->num_markers = num_markers;
    layer->markers = malloc(num_markers * sizeof(struct aet_marker_t));
    for (int i = 0; i < num_markers; i++)
    {
        // array
        fseek(file, markers_pointer + (i * 8), SEEK_SET);

        // read the frame
        float frame;
        fread(&frame, sizeof(frame), 1, file);

        // read the name
        uint32_t name_pointer;
        fread(&name_pointer, sizeof(name_pointer), 1, file);
        fseek(file, name_pointer, SEEK_SET);
        char *name = utils_read_string(file);

        // attempt to read the type
        enum aet_marker_type_t type = AET_MARKER_TYPE_UNKNOWN;
        if (strcmp(name, "ST_IN") == 0)
            type = AET_MARKER_TYPE_IN_START;
        else if (strcmp(name, "ED_IN") == 0)
            type = AET_MARKER_TYPE_IN_END;
        else if (strcmp(name, "ST_LP") == 0)
            type = AET_MARKER_TYPE_LOOP_START;
        else if (strcmp(name, "ED_LP") == 0)
            type = AET_MARKER_TYPE_LOOP_END;
        else if (strcmp(name, "ST_OUT") == 0)
            type = AET_MARKER_TYPE_OUT_START;
        else if (strcmp(name, "ED_OUT") == 0)
            type = AET_MARKER_TYPE_OUT_END;
        else if (strcmp(name, "ST_SP") == 0)
            type = AET_MARKER_TYPE_PRESS_START;
        else if (strcmp(name, "ED_SP") == 0)
            type = AET_MARKER_TYPE_PRESS_END;

        // insert the marker
        struct aet_marker_t *marker = &layer->markers[i];
        marker->frame = frame;
        marker->name = name;
        marker->type = type;
    }

    // read the properties
    fseek(file, properties_pointer, SEEK_SET);

    // read the blend mode
    enum aet_layer_blend_mode_t blend_mode = 0x0;
    fread(&blend_mode, sizeof(uint8_t), 1, file);
    layer->blend_mode = blend_mode;

    // read the temporal property keyframes
    // anchor point x/y, position x/y, rotation, scale x/y, opacity
    for (int i = 0; i < 8; i++)
    {
        // array
        // +4 to skip the preceding enums
        fseek(file, properties_pointer + 4 + (i * 8), SEEK_SET);

        // read the keyframe count and data array pointer
        uint32_t num_keyframes, data_pointer;
        fread(&num_keyframes, sizeof(num_keyframes), 1, file);
        fread(&data_pointer, sizeof(data_pointer), 1, file);

        // read the data
        assert(num_keyframes > 0);

        struct aet_layer_keyframes_t keyframes;
        keyframes.num_keyframes = num_keyframes;
        keyframes.values = malloc(num_keyframes * sizeof(float));
        if (num_keyframes == 1)
        {
            keyframes.type = AET_LAYER_KEYFRAMES_TYPE_SINGLE;
            keyframes.frames = NULL;

            // read and insert the single keyframe
            float value;
            fseek(file, data_pointer, SEEK_SET);
            fread(&value, sizeof(value), 1, file);

            keyframes.values[0] = value;
        }
        else
        {
            keyframes.type = AET_LAYER_KEYFRAMES_TYPE_MULTIPLE;
            keyframes.frames = malloc(num_keyframes * sizeof(float));

            // read and insert all the keyframes
            for (int i = 0; i < num_keyframes; i++)
            {
                // read the frame number
                float frame;
                fseek(file, data_pointer + (i * sizeof(float)), SEEK_SET);
                fread(&frame, sizeof(frame), 1, file);

                // read and insert the keyframe
                // the value is in a second array after the frames
                // with 2 floats per entry, so seek to the beginning of that array,
                // then seek to the item and take the value
                float value;
                fseek(file, data_pointer + (num_keyframes * sizeof(float)), SEEK_SET);
                fseek(file, (i * sizeof(float) * 2), SEEK_CUR);
                fread(&value, sizeof(value), 1, file);

                keyframes.values[i] = value;
                keyframes.frames[i] = frame;
            }
        }

        // set the property keyframes
        switch (i)
        {
            case 0: layer->anchor_point_x = keyframes; break;
            case 1: layer->anchor_point_y = keyframes; break;
            case 2: layer->position_x = keyframes; break;
            case 3: layer->position_y = keyframes; break;
            case 4: layer->rotation = keyframes; break;
            case 5: layer->scale_x = keyframes; break;
            case 6: layer->scale_y = keyframes; break;
            case 7: layer->opacity = keyframes; break;
            default: assert(0);
        }
    }
}

void aet_open(const char *path, struct aet_t *aet)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");
    aet->file = file;

    // read the header size
    uint32_t header_size;
    fread(&header_size, sizeof(header_size), 1, file);

    // read the composition pointers
    uint32_t compositions_pointer, composition_names_pointer, composition_scr_names_pointer;
    fread(&compositions_pointer, sizeof(compositions_pointer), 1, file);
    fread(&composition_names_pointer, sizeof(composition_names_pointer), 1, file);
    fread(&composition_scr_names_pointer, sizeof(composition_scr_names_pointer), 1, file);

    // padding
    for (int i = 0; i < header_size - 16; i++)
        assert(fgetc(file) == 0x0);

    // read the composition count
    uint32_t num_compositions;
    fread(&num_compositions, sizeof(num_compositions), 1, file);

    // initialize the aet
    aet->num_compositions = num_compositions;
    aet->compositions = malloc(num_compositions * sizeof(struct aet_composition_t));

    // read the compositions
    for (int i = 0; i < num_compositions; i++)
    {
        struct aet_composition_t composition;

        // read the name
        {
            // pointer table
            fseek(file, composition_names_pointer + (i * 4), SEEK_SET);

            uint32_t name_pointer;
            fread(&name_pointer, sizeof(name_pointer), 1, file);
            fseek(file, name_pointer, SEEK_SET);

            // read the string
            composition.name = utils_read_string(file);
        }

        // read the used scr names
        {
            // read the first and last entry pointers
            fseek(file, composition_scr_names_pointer + (i * 8), SEEK_SET);

            uint32_t first_entry_pointer, last_entry_pointer;
            fread(&first_entry_pointer, sizeof(first_entry_pointer), 1, file);
            fread(&last_entry_pointer, sizeof(last_entry_pointer), 1, file);

            // calculate the number of entries by comparing the first and last entry
            // pointers against the size of each entry
            unsigned int num_scr_names = (last_entry_pointer - first_entry_pointer) / 4;

            // read all the scr names
            composition.num_scr_names = num_scr_names;
            composition.scr_names = malloc(num_scr_names * sizeof(char *));
            for (int i = 0; i < num_scr_names; i++)
            {
                // pointer table
                fseek(file, first_entry_pointer + (i * 4), SEEK_SET);

                uint32_t scr_name_pointer;
                fread(&scr_name_pointer, sizeof(scr_name_pointer), 1, file);
                fseek(file, scr_name_pointer, SEEK_SET);

                // read the scr name
                composition.scr_names[i] = utils_read_string(file);
            }
        }

        // read the composition
        {
            // pointer table
            fseek(file, compositions_pointer + (i * 4), SEEK_SET);

            uint32_t composition_pointer;
            fread(&composition_pointer, sizeof(composition_pointer), 1, file);
            fseek(file, composition_pointer, SEEK_SET);

            // read the timeline properties
            float timeline_start_frame, timeline_end_frame, timeline_frame_rate;
            fread(&timeline_start_frame, sizeof(timeline_start_frame), 1, file);
            fread(&timeline_end_frame, sizeof(timeline_end_frame), 1, file);
            fread(&timeline_frame_rate, sizeof(timeline_frame_rate), 1, file);

            // 1x float unknown
            fseek(file, 4, SEEK_CUR);

            // read the size
            uint32_t width, height;
            fread(&width, sizeof(width), 1, file);
            fread(&height, sizeof(height), 1, file);
            assert(width == 320 || width == 400 || width == 1200);
            assert(height == 240 || height == 720);

            // padding
            for (int i = 0; i < 4; i++)
                assert(fgetc(file) == 0x0);

            // initialize the composition
            composition.timeline_start_frame = timeline_start_frame;
            composition.timeline_end_frame = timeline_end_frame;
            composition.timeline_frame_rate = timeline_frame_rate;
            composition.width = width;
            composition.height = height;

            // read the layer group count and pointer
            uint32_t num_layer_groups, layer_groups_pointer;
            fread(&num_layer_groups, sizeof(num_layer_groups), 1, file);
            fread(&layer_groups_pointer, sizeof(layer_groups_pointer), 1, file);

            // u32 sprite group count and pointer, unused
            // all the sprite groups are pointed to by layers when needed
            fseek(file, 2 * 4, SEEK_CUR);

            // padding
            for (int i = 0; i < 5; i++)
                assert(fgetc(file) == 0x0);

            // read the layer groups
            composition.num_layer_groups = num_layer_groups;
            composition.layer_groups = malloc(num_layer_groups * sizeof(struct aet_layer_group_t));
            for (int g = 0; g < num_layer_groups; g++)
            {
                // array
                fseek(file, layer_groups_pointer + (g * 8), SEEK_SET);

                // read the layer count and pointer
                uint32_t num_layers, layers_pointer;
                fread(&num_layers, sizeof(num_layers), 1, file);
                fread(&layers_pointer, sizeof(layers_pointer), 1, file);

                // read the layers
                struct aet_layer_group_t group;
                group.num_layers = num_layers;
                group.layers = malloc(num_layers * sizeof(struct aet_layer_t));
                for (int n = 0; n < num_layers; n++)
                {
                    // array
                    fseek(file, layers_pointer + (n * 48), SEEK_SET);

                    // read and insert the layer
                    struct aet_layer_t layer;
                    aet_layer_read(file, &layer);
                    group.layers[n] = layer;
                }

                // insert the layer group
                composition.layer_groups[g] = group;
            }
        }

        // insert the composition
        aet->compositions[i] = composition;
    }
}

/// Release the given layer keyframes and all of it's allocated resources.
/// @param keyframes The layer keyframes to free.
void aet_layer_keyframes_free(struct aet_layer_keyframes_t *keyframes)
{
    free(keyframes->values);
    switch (keyframes->type)
    {
        case AET_LAYER_KEYFRAMES_TYPE_MULTIPLE:
            free(keyframes->frames);
            break;
        default:
            break;
    }
}

void aet_close(struct aet_t *aet)
{
    for (int i = 0; i < aet->num_compositions; i++)
    {
        struct aet_composition_t *composition = &aet->compositions[i];
        for (int i = 0; i < composition->num_scr_names; i++)
            free(composition->scr_names[i]);

        for (int i = 0; i < composition->num_layer_groups; i++)
        {
            struct aet_layer_group_t *group = &composition->layer_groups[i];
            for (int i = 0; i < group->num_layers; i++)
            {
                struct aet_layer_t *layer = &group->layers[i];
                switch (layer->type)
                {
                    case AET_LAYER_TYPE_SOURCE_SPRITE_GROUP:
                        free(layer->sprite_group);
                        break;
                    case AET_LAYER_TYPE_NULL_OBJECT:
                        free(layer->children);
                        break;
                }

                for (int i = 0; i < layer->num_markers; i++)
                    free(layer->markers[i].name);

                free(layer->markers);

                aet_layer_keyframes_free(&layer->anchor_point_x);
                aet_layer_keyframes_free(&layer->anchor_point_y);
                aet_layer_keyframes_free(&layer->position_x);
                aet_layer_keyframes_free(&layer->position_y);
                aet_layer_keyframes_free(&layer->rotation);
                aet_layer_keyframes_free(&layer->scale_x);
                aet_layer_keyframes_free(&layer->scale_y);
                aet_layer_keyframes_free(&layer->opacity);
            }

            free(group->layers);
        }

        free(composition->layer_groups);
        free(composition->scr_names);
        free(composition->name);
    }

    free(aet->compositions);
    fclose(aet->file);
}

double aet_frame_to_ms(float frame, float framerate, float speed)
{
    double frame_duration = (1 / framerate) * 1000;
    double frame_ms = (frame * frame_duration) / speed;
    return frame_ms;
}
