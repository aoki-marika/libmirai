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
/// @param num_related_layers The total number of layers within the given array.
/// @param related_layers All the possibly related layers that this layer can point to for children and parents.
/// It is asserted that the children and/or parent for this layer are within this array.
/// It is expected that this array is kept in memory until the layer is released.
/// The parents of these layers are set depending on if they are a child of the new layer.
/// @param related_layer_pointers The pointer to each layer within the given array,
/// within the given file handle.
/// @param num_sprite_groups The total number of sprite groups within the given array.
/// @param sprite_groups All the sprite groups that this layer can point to for it's source.
/// It is asserted that the sprite group for this layer is within this array.
/// It is expected that this array is kept in memory until the layer is released.
/// @param sprite_group_pointers The pointer to each sprite group within the given array,
/// within the given file handle.
/// @param layer The layer to read the file into.
void aet_layer_read(FILE *file,
                    unsigned int num_related_layers,
                    struct aet_layer_t *related_layers,
                    const size_t *related_layer_pointers,
                    unsigned int num_sprite_groups,
                    const struct aet_sprite_group_t *sprite_groups,
                    const size_t *sprite_group_pointers,
                    struct aet_layer_t *layer)
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
    // the parent is set later on when children are read
    // as the parent is not always read before this point
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
    layer->parent = NULL;

    // read the source
    layer->type = type;
    switch (type)
    {
        case AET_LAYER_TYPE_SOURCE_SPRITE_GROUP:
        {
            // get the sprite group
            const struct aet_sprite_group_t *sprite_group = NULL;
            for (int i = 0; i < num_sprite_groups; i++)
            {
                if (sprite_group_pointers[i] == source_pointer)
                {
                    sprite_group = &sprite_groups[i];
                    break;
                }
            }

            // assert that the sprite group was found
            assert(sprite_group != NULL);

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

            // get and set the children
            layer->sprite_group = NULL;
            layer->num_children = num_children;
            layer->children = calloc(num_children, sizeof(struct aet_layer_t));
            for (int c = 0; c < num_children; c++)
            {
                // array
                size_t child_pointer = children_pointer + (c * 48);
                for (int l = 0; l < num_related_layers; l++)
                {
                    if (related_layer_pointers[l] == child_pointer)
                    {
                        struct aet_layer_t *child = &related_layers[l];
                        child->parent = layer;
                        layer->children[c] = child;
                        break;
                    }
                }

                // assert that the child was found
                assert(layer->children[c] != NULL);
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

            // read the layer level count and pointer
            uint32_t num_layer_levels, layer_levels_pointer;
            fread(&num_layer_levels, sizeof(num_layer_levels), 1, file);
            fread(&layer_levels_pointer, sizeof(layer_levels_pointer), 1, file);

            // read the sprite group count and pointer
            uint32_t num_sprite_groups, sprite_groups_pointer;
            fread(&num_sprite_groups, sizeof(num_sprite_groups), 1, file);
            fread(&sprite_groups_pointer, sizeof(sprite_groups_pointer), 1, file);

            // padding
            for (int i = 0; i < 5; i++)
                assert(fgetc(file) == 0x0);

            // read the sprite groups
            // need to read the first so that layers can point to them
            // keep the pointers to the sprite groups to keep track of
            // which is which when matching them to layers
            size_t sprite_group_pointers[num_sprite_groups];
            composition.num_sprite_groups = num_sprite_groups;
            composition.sprite_groups = malloc(num_sprite_groups * sizeof(struct aet_sprite_group_t));
            for (int i = 0; i < num_sprite_groups; i++)
            {
                // array
                fseek(file, sprite_groups_pointer + (i * 20), SEEK_SET);

                // insert the pointer
                sprite_group_pointers[i] = ftell(file);

                // read and insert the sprite group
                struct aet_sprite_group_t sprite_group;
                aet_sprite_group_read(file, &sprite_group);
                composition.sprite_groups[i] = sprite_group;
            }

            // read the layers
            //
            // its difficult to understand what the purpose of layer levels are
            // but each group builds up a single "level" of layers,
            // of which then a succeeding layer level uses within its layers children
            // the children of a layer within a group are **always** within
            // a preceding group, however parents rarely are
            //
            // so the (seemingly) best way to read these is to only read
            // the top level layers of each group, getting their children
            // by comparing the pointer to the child with the pointer of already
            // read layers, and then setting them by reference
            //
            // iterate through the groups once first to get the total layer count
            // and avoid any annoying realloc business
            unsigned int num_layers = 0;
            for (int level = 0; level < num_layer_levels; level++)
            {
                // array
                fseek(file, layer_levels_pointer + (level * 8), SEEK_SET);

                // read the group layer count and increment the total
                uint32_t num_group_layers;
                fread(&num_group_layers, sizeof(num_group_layers), 1, file);

                num_layers += num_group_layers;
            }

            // reiterate and read all the groups and their layers
            composition.num_layers = num_layers;
            composition.layers = malloc(num_layers * sizeof(struct aet_layer_t));

            unsigned int layer_index = 0;
            size_t layer_pointers[num_layers];
            for (int level = 0; level < num_layer_levels; level++)
            {
                // array
                fseek(file, layer_levels_pointer + (level * 8), SEEK_SET);

                // read the group layer count and pointer
                uint32_t num_group_layers, group_layers_pointer;
                fread(&num_group_layers, sizeof(num_group_layers), 1, file);
                fread(&group_layers_pointer, sizeof(group_layers_pointer), 1, file);

                // read all the layers within this group
                for (int layer = 0; layer < num_group_layers; layer++)
                {
                    // array
                    fseek(file, group_layers_pointer + (layer * 48), SEEK_SET);

                    // set the pointer, read, and insert the layer
                    layer_pointers[layer_index] = ftell(file);

                    struct aet_layer_t layer;
                    aet_layer_read(file,
                                   num_layers,
                                   composition.layers,
                                   layer_pointers,
                                   num_sprite_groups,
                                   composition.sprite_groups,
                                   sprite_group_pointers,
                                   &layer);

                    composition.layers[layer_index] = layer;

                    // increment the layer index
                    layer_index++;
                }
            }

            // assert that the correct amount of layers were read
            assert(layer_index == num_layers);
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

        for (int i = 0; i < composition->num_layers; i++)
        {
            struct aet_layer_t *layer = &composition->layers[i];
            switch (layer->type)
            {
                case AET_LAYER_TYPE_NULL_OBJECT:
                    free(layer->children);
                    break;
                default:
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

        free(composition->sprite_groups);
        free(composition->layers);
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
