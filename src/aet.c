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

/// Read the node at the current position of the given file into the given node.
/// @param file The file to read the node from.
/// @param node The node to read the file into.
void aet_node_read(FILE *file, struct aet_node_t *node)
{
    // read the name and seek back
    uint32_t name_pointer;
    fread(&name_pointer, sizeof(name_pointer), 1, file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // unknown values
    //  - 4x float unknown
    //  - 1x u16 flags unknown
    //  - 1x u8 unknown
    fseek(file, (4 * 4) + 2 + 1, SEEK_CUR);

    // read the contents type and pointer
    // init 0x0 and fread sizeof(u8) as sizeof(enum) is 4
    enum aet_node_contents_type_t contents_type = 0x0;
    fread(&contents_type, sizeof(uint8_t), 1, file);

    uint32_t contents_pointer;
    fread(&contents_pointer, sizeof(contents_pointer), 1, file);

    // unknown values
    //  - u32 node pointer unknown
    //  - u32 unknown 3 count
    //  - u32 unknown 3 array pointer
    //  - u32 unknown 4 pointer
    fseek(file, 4 * 4, SEEK_CUR);

    // padding
    for (int i = 0; i < 4; i++)
        assert(fgetc(file) == 0x0);

    // initialize the node
    node->name = name;

    // read the contents
    node->contents_type = contents_type;
    switch (contents_type)
    {
        case AET_NODE_CONTENTS_TYPE_SPRITE_GROUP:
        {
            // read the sprite group
            fseek(file, contents_pointer, SEEK_SET);

            struct aet_sprite_group_t *sprite_group = malloc(sizeof(struct aet_sprite_group_t));
            aet_sprite_group_read(file, sprite_group);

            // set the nodes contents
            node->sprite_group = sprite_group;
            node->num_children = 0;
            node->children = NULL;
            break;
        }
        case AET_NODE_CONTENTS_TYPE_CHILDREN:
        {
            // read the child count and pointer
            fseek(file, contents_pointer, SEEK_SET);

            uint32_t num_children, children_pointer;
            fread(&num_children, sizeof(num_children), 1, file);
            fread(&children_pointer, sizeof(children_pointer), 1, file);

            // read the children and set the nodes contents
            node->sprite_group = NULL;
            node->num_children = num_children;
            node->children = malloc(num_children * sizeof(struct aet_node_t));
            for (int i = 0; i < num_children; i++)
            {
                // array
                fseek(file, children_pointer + (i * 48), SEEK_SET);

                // read the node
                struct aet_node_t child;
                aet_node_read(file, &child);

                // insert the child
                node->children[i] = child;
            }
            break;
        }
        default:
            // should never be reached
            assert(0);
    }
}

void aet_open(const char *path, struct aet_t *aet)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");

    // read the header size
    uint32_t header_size;
    fread(&header_size, sizeof(header_size), 1, file);

    // read the scene pointers
    uint32_t scene_headers_pointer, scene_names_pointer, scene_scr_names_pointer;
    fread(&scene_headers_pointer, sizeof(scene_headers_pointer), 1, file);
    fread(&scene_names_pointer, sizeof(scene_names_pointer), 1, file);
    fread(&scene_scr_names_pointer, sizeof(scene_scr_names_pointer), 1, file);

    // ensure the extra bytes within the header are 0x0
    for (int i = 0; i < header_size - 16; i++)
        assert(fgetc(file) == 0x0);

    // read the scene count
    uint32_t num_scenes;
    fread(&num_scenes, sizeof(num_scenes), 1, file);

    // initialize the aet
    aet->num_scenes = num_scenes;
    aet->scenes = malloc(num_scenes * sizeof(struct aet_scene_t));

    // read the scenes
    for (int i = 0; i < num_scenes; i++)
    {
        struct aet_scene_t scene;

        // read the name
        {
            // pointer table
            fseek(file, scene_names_pointer + (i * 4), SEEK_SET);

            uint32_t name_pointer;
            fread(&name_pointer, sizeof(name_pointer), 1, file);
            fseek(file, name_pointer, SEEK_SET);

            // read the string
            scene.name = utils_read_string(file);
        }

        // read the used scr names
        {
            // read the first and last entry pointers
            fseek(file, scene_scr_names_pointer + (i * 8), SEEK_SET);

            uint32_t first_entry_pointer, last_entry_pointer;
            fread(&first_entry_pointer, sizeof(first_entry_pointer), 1, file);
            fread(&last_entry_pointer, sizeof(last_entry_pointer), 1, file);

            // calculate the number of entries by comparing the first and last entry
            // pointers against the size of each entry
            unsigned int num_scr_names = (last_entry_pointer - first_entry_pointer) / 4;

            // read all the scr names
            scene.num_scr_names = num_scr_names;
            scene.scr_names = malloc(num_scr_names * sizeof(char *));
            for (int i = 0; i < num_scr_names; i++)
            {
                // pointer table
                fseek(file, first_entry_pointer + (i * 4), SEEK_SET);

                uint32_t scr_name_pointer;
                fread(&scr_name_pointer, sizeof(scr_name_pointer), 1, file);
                fseek(file, scr_name_pointer, SEEK_SET);

                // read the scr name
                scene.scr_names[i] = utils_read_string(file);
            }
        }

        // read the header
        {
            // pointer table
            fseek(file, scene_headers_pointer + (i * 4), SEEK_SET);

            uint32_t header_pointer;
            fread(&header_pointer, sizeof(header_pointer), 1, file);
            fseek(file, header_pointer, SEEK_SET);

            // 16x byte unknown
            fseek(file, 16, SEEK_CUR);

            // read the size
            uint32_t width, height;
            fread(&width, sizeof(width), 1, file);
            fread(&height, sizeof(height), 1, file);
            assert(width == 320 || width == 400 || width == 1200);
            assert(height == 240 || height == 720);

            scene.width = width;
            scene.height = height;

            // 4x byte unknown
            fseek(file, 4, SEEK_CUR);

            // read the node group count and pointer
            uint32_t num_node_groups, node_groups_pointer;
            fread(&num_node_groups, sizeof(num_node_groups), 1, file);
            fread(&node_groups_pointer, sizeof(node_groups_pointer), 1, file);

            // read the sprite group count and pointer
            // TODO: Are sprite groups actually supposed to be read here, or are they all read from nodes?
            uint32_t num_sprite_groups, sprite_groups_pointer;
            fread(&num_sprite_groups, sizeof(num_sprite_groups), 1, file);
            fread(&sprite_groups_pointer, sizeof(sprite_groups_pointer), 1, file);

            // padding
            for (int i = 0; i < 5; i++)
                assert(fgetc(file) == 0x0);

            // read the node groups
            scene.num_node_groups = num_node_groups;
            scene.node_groups = malloc(num_node_groups * sizeof(struct aet_node_group_t));
            for (int g = 0; g < num_node_groups; g++)
            {
                // array
                fseek(file, node_groups_pointer + (g * 8), SEEK_SET);

                // read the node count and pointer
                uint32_t num_nodes, nodes_pointer;
                fread(&num_nodes, sizeof(num_nodes), 1, file);
                fread(&nodes_pointer, sizeof(nodes_pointer), 1, file);

                // read the nodes
                struct aet_node_group_t group;
                group.num_nodes = num_nodes;
                group.nodes = malloc(num_nodes * sizeof(struct aet_node_t));
                for (int n = 0; n < num_nodes; n++)
                {
                    // read the node
                    fseek(file, nodes_pointer + (n * 48), SEEK_SET);

                    struct aet_node_t node;
                    aet_node_read(file, &node);

                    // insert the node
                    group.nodes[n] = node;
                }

                // insert the node group
                scene.node_groups[g] = group;
            }
        }

        // insert the scene
        aet->scenes[i] = scene;
    }

    // initialize the aet
    aet->file = file;
}

void aet_close(struct aet_t *aet)
{
    for (int i = 0; i < aet->num_scenes; i++)
    {
        struct aet_scene_t *scene = &aet->scenes[i];
        for (int i = 0; i < scene->num_scr_names; i++)
            free(scene->scr_names[i]);

        for (int i = 0; i < scene->num_node_groups; i++)
        {
            struct aet_node_group_t *group = &scene->node_groups[i];
            for (int i = 0; i < group->num_nodes; i++)
            {
                struct aet_node_t *node = &group->nodes[i];
                switch (node->contents_type)
                {
                    case AET_NODE_CONTENTS_TYPE_SPRITE_GROUP:
                        free(node->sprite_group);
                        break;
                    case AET_NODE_CONTENTS_TYPE_CHILDREN:
                        free(node->children);
                        break;
                }
            }

            free(group->nodes);
        }

        free(scene->node_groups);
        free(scene->scr_names);
        free(scene->name);
    }

    free(aet->scenes);
    fclose(aet->file);
}
