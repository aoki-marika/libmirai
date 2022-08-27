# Specification

Specification and documentation on the various proprietary file formats used in the Hatsune Miku: Project Mirai series of 3DS games.

Do note that these are based entirely on the state of each format within Hatsune Miku: Project Mirai DX, and it may have been different in earlier titles.

## Terminology

- SPR
    - A file format for containing texture and subsection data within Project Mirai, which combines multiple CTPKs and SCRs.
- CTPK
    - [CTR texture package](https://3dbrew.org/wiki/CTPK), a generic file format for containing texture data on the 3DS.
- SCR
    - A named subsection within the texture of a CTPK within an SPR.
- Pointer
    - An integer pointing to another position within a file, sometimes called offsets. These are almost always `0x4` in length.
- Pointer table
   - An array of `0x4` length pointers.
- Array
    - A series of back-to-back values.
        - For example, `0x01020304` could be an array of four 8-bit integers, the items then being `1, 2, 3, 4`.
- `[0x...]`
    - This is used to reference a value at the specified offset within the offset and length columns in this document.
    
# `SPR`

These files contain various data for loading textures within Project Mirai, such as texture data and atlas' for them.

## Notes

 - All integers within SPR files are in little endian.
 - All pointers within SPR files are absolute.
 - Strings within SPR files are a bit non-standard in that each string has a fixed allocated size within the file, then when the string is smaller than this size it has a variable amount of terminator characters surrounding it. It does **not** have the string at the beginning with a variable amount of terminator characters at the end, but rather terminator characters surrounding it.
    - libmirai reads these by reading each character within the allocated space and including it only if it is not a terminator character.

## Header
 
SPR header:
 
| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Signature `0x00000000` |
| `0x4` | `0x4` | Pointer to CTPKs array |
| `0x8` | `0x4` | Number of CTPKs |
| `0xc` | `0x4` | Pointer to CTPK names array (`0x20` length allocated strings) |
| `0x10` | `0x8` | Padding |
| `0x18` | `0x4` | Number of SCRs |
| `0x1c` | `0x4` | Pointer to SCRs array |

## CTPK

CTPKs within SPRs are entirely standard beyond the first `0x4` bytes being a pointer to the next CTPK.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | End relative pointer to next CTPK |
| `0x4` | `???` | CTPK file |

Also of note is that CTPKs within SPRs always contain one texture, never multiple.
This detail can be used to slightly simplify a Mirai-exclusive CTPK reader.

## SCR

SCRs are used to describe a named subsection within a CTPK's texture, functioning as a texture atlas.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x1` | CTPK index |
| `0x1` | `0x47` | Name (`0x47` length allocated string) |
| `0x48` | `0x8` | Top left UV coordinate (U/V floats, top-left origin) |
| `0x50` | `0x8` | Bottom right UV coordinate (U/V floats, top-left origin) |
| `0x58` | `0x2` | Top left U coordinate in pixels (top-left origin) |
| `0x5a` | `0x2` | Top left V coordinate in pixels (top-left origin) |
| `0x5c` | `0x2` | Width in pixels |
| `0x5e` | `0x2` | Height in pixels |

Note that as CTPKs within SPRs always have one texture, SCRs automatically infer referring to texture zero within their respective CTPK.

# `AET`

These files contain various data for utilising the data within SPR files, such as layout and animation of SCRs.
They also contain additional information for the chart line in PV AET files.

## Notes

 - All integers within AET files are in little endian.
 - All pointers within AET files are absolute.
 - Strings within AET files are regular null-terminated strings, read every character until the first terminator.
 - As no other people seem to have reversed this format, AET terminology used here is entirely made up based on what function each type appears to serve.

## Header

The header for AET files.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Header size, self-inclusive |
| `0x4` | `0x4` | Pointer to Scene Headers pointer table |
| `0x8` | `0x4` | Pointer to Scene names pointer table |
| `0xc` | `0x4` | Pointer to Scene SCR Names array |
| `0x10` | `[0x0 - 0x10]` | Padding? Always zero |
| `[0x0]` | `0x4` | Number of children |

## Scene SCR Names

Scenes use a table of SCR names to refer to SCRs throughout their contents, of which these items define.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Pointer to first pointer table entry |
| `0x4` | `0x4` | Pointer to last pointer table entry |

The first and last entry pointers refer to the pointers of the first and last item within the SCR names pointer table that the Scene uses.

libmirai reads these by getting the number of SCR names from `([last entry pointer] - [first entry pointer]) / 4`, then seeking to the first entry pointer and reading that number of entries.

## Scene Header

Scene header:

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Unknown float, typically `0` |
| `0x4` | `0x4` | Unknown float, typically whole numbers in the high thousands on more complex AETs, smaller on less complex |
| `0x8` | `0x4` | Unknown float, typically `30` (maybe framerate related?) |
| `0xc` | `0x4` | Unknown float, typically `0` |
| `0x10` | `0x4` | Width |
| `0x14` | `0x4` | Height |
| `0x18` | `0x4` | Unknown, always `0x00000000` |
| `0x1c` | `0x4` | Number of Node Groups |
| `0x20` | `0x4` | Pointer to Node Groups array |
| `0x24` | `0x4` | Number of Sprite Groups |
| `0x28` | `0x4` | Pointer to Sprite Group array |
| `0x2c` | `0x5` | Padding? Always `0x0000000000` |

Width is always one of the following:
 - `320` (3DS bottom screen width)
 - `400` (3DS top screen width)
 - `1200` (exclusive to PV lines)

And height is always one of the following:
 - `240` (3DS top and bottom screen height)
 - `720` (exclusive to PV lines)
 
 The Node Groups within a Scene seem to be commonly grouped into small parts of the screen, then have a full Node Group combining them as the last one.

## Node Group

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Number of Nodes |
| `0x4` | `0x4` | Pointer to Nodes array |

It seems that although this is an array, only the first Node should be read.
It appears to reference all the other nodes in the array as children, so maybe it's like Scene Header's Sprite Group array, where it's there but not used?

Not entirely sure but it appears to be missing some if only reading the first.
Definitely is.

These are potentially actually Node Groups, similar to Sprite Groups.
Some seem to use them for the same usage as Sprite Groups with multiple Sprites where it flips between animation frames, e.g. `aet_puyokai.bin`'s `LASER` Nodes.

## Node

These appear to be used for adding layout and animation data to Sprite Groups.
Potentially these define the scene graph while Sprite Groups and Sprites only define basic drawing information.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Name pointer |
| `0x4` | `0x4` | Unknown float, small whole numbers, typically `0.0` |
| `0x8` | `0x4` | Unknown float, small whole numbers, never `0.0` |
| `0xc` | `0x4` | Unknown float, whole numbers, typically `0.0` |
| `0x10` | `0x4` | Unknown float, ranges from `0.833333` to `2.0`, most commonly being `1.0` |
| `0x14` | `0x2` | Unknown, flags? |
| `0x16` | `0x1` | Unknown, related to Contents Type and Children, only ever `0x02` and `0x03` |
| `0x17` | `0x1` | Contents Type |
| `0x18` | `0x4` | Contents pointer |
| `0x1c` | `0x4` | Parent Node pointer |
| `0x20` | `0x4` | Number of Markers |
| `0x24` | `0x4` | Markers array pointer |
| `0x28` | `0x4` | Placement pointer |
| `0x2c` | `0x4` | Padding? Always `0x00000000` |

Contents define what is inside the node, which can either be a Sprite Group where the Node draws said Sprite Group, or an array of child Nodes that use their parent's properties as a base.

Not entirely sure what `0x16` is, thought it was related to Children as it appeared as if the 1reader was getting stuck on some files, but actually the puyo AETs just copy duplicate a ton of resources.

Given what `0x4`, `0x8`, and `0xc` usually are they are likely timing related and referring to frames like Markers. `0x10` is likely also related to them, maybe being some sort of playback speed multiplier? Unsure.

### Contents Type

| Value | Type |
| --- | --- |
| `0x01` | Sprite Group |
| `0x03` | Children |

### Children

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Number of Nodes |
| `0x4` | `0x4` | Nodes array pointer |

The Children seem to be used as references, where they all reference Nodes within their containing Node Group.
If the Children are read as separate Nodes then they end up mostly (if not always) being duplicates.
The `aet_puyo*.bin` files are a prime example of this, with hundreds of confetti duplicates.

### Marker

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Frame number float, fraction is ignored |
| `0x4` | `0x4` | Name pointer |

Based on the names these are definitely (at least related to) animation data.

Most AETs have Markers with names such as `ST_IN`, `ED_LP`, `ST_OUT`, etc. suggesting that these are related to defining different parts of an animations.

However in PVs such as `Gaikotsu Gakudan to Lilia` (`aet_pv052.bin`,) it has Markers on it's `LINE05` Node with names `BPM205` and `BPM130` (the effects of which can be seen [here](https://youtu.be/-YWqh2MPw_k?t=129),) suggesting that these can also be used to send events back to the engine. It's also possible that this is another PV line exclusive feature and the only place where it's used as such, like with the extra Scene sizes. Many PVs also use Markers named `X1`, `X3`, and `X6`, unsure what these are. Only guess is it's related to something scoring wise, possibly controlling one of the values first defined in `score_db.txt` like SP scale.

Some names are also empty or only a single number like `1` or `2`, but still have (seemingly) valid values. These are present in both PV and unrelated AETs.

Regardless the naming implies that the unknown float is timing related, and likely determines the time at which the Marker occurs. On many Nodes these are small whole numbers, possibly meaning that this time is a frame number. Given that the only times that it is a fraction it is very small (e.g. `.000366`,) it's likely these are rounding errors and that floats are only used for the larger range of values, so the fraction is ignored.

These seem to propagate to children as many Nodes have Markers but only single Placement properties.

Based on comparing these to properties within Placements and comparing them to the animations in-game, these are markers for points within a Node's animation that define ranges for different states, such as in transition, loop, and out transition. The standard names are noted below:

 - `ST_IN`: Start of the in transition animation.
 - `ED_IN`: End of the in transition animation.
 - `ST_LP`: Start of the loop animation.
 - `ED_LP`: End of the loop animation.
 - `ST_OUT`: Start of the out transition animation.
 - `ED_OUT`: End of the out transition animation.
 - `ST_SP`: Start of the press animation?
 - `ED_SP`: End of the press animation?
 - `X`: Unknown, only seen twice in PV lines, could be a typo.
 - `X1`: Unknown, only seen in PV lines, possibly scoring related.
 - `X3`: See above.
 - `X6`: See above.
 - `BPMNNN`: Where `NNN` is any three digit number, used in PV lines to mark BPM changes.
 - `N`: Where `N` is any single digit number, unknown, maybe sprite swapping animation related?
 - `CUT0N`: Where `N` is any single digit number, unknown, maybe masking related?

The remaining markers have either empty or broken names from invalid pointers (e.g. `\211\346\226ʃM\203C\203Y` and `\210ȑO\202̃T\203F\201[\203H\210ʒU`.) Unclear what these are.

### Placement

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Unknown, typically `0x03000000` |
| `0x4` | `0x4` | Number of origin X floats |
| `0x8` | `0x4` | Origin X floats array pointer |
| `0xc` | `0x4` | Number of origin Y floats |
| `0x10` | `0x4` | Origin Y floats array pointer |
| `0x14` | `0x4` | Number of position X floats |
| `0x18` | `0x4` | Position X floats array pointer |
| `0x1c` | `0x4` | Number of position Y floats |
| `0x20` | `0x4` | Position Y floats array pointer |
| `0x24` | `0x4` | Number of rotation floats | 
| `0x28` | `0x4` | Rotation floats array pointer |
| `0x2c` | `0x4` | Number of scale X floats | 
| `0x30` | `0x4` | Scale X floats array pointer |
| `0x34` | `0x4` | Number of scale Y floats | 
| `0x38` | `0x4` | Scale Y floats array pointer |
| `0x3c` | `0x4` | Number of opacity floats | 
| `0x40` | `0x4` | Opacity floats array pointer |
| `0x44` | `0x4` | Unknown pointer, sometimes `0x00000000` |

Unsure what the other floats in each array do, maybe animation frames?
However the first one always seems to be the starting property of the Node.

Note that each array always contains at least one float, they are never empty.

When there are multiple values for a property then they seem to be invalid. This is most commonly seen with opacity properties, for example the `LOGO_SEGA` node in `aet_adv.bin`, where every value is way outside the bounds of opacity, or the `LOGO_MAIN` node's scale properties where they are all way outside of what would be used. They instead seem to be referring to frame numbers at which changes would occur, but where the actual value is stored is a mystery.

Unknown pointer seems to point to data formatted in the same count array pointer format. Despite it pointing to different data that does not overlap, at least one array within it exactly matches the array of one of the animated properties within the Placement. They also do not both point to the same array within the file, but two separate arrays with identical data. As it's identical it also refers to frame numbers, and not final properties. This one also does not have a pointer at the end, it ends immediately before the Placement that pointed to it in the first place. There are also Placements with animated properties, but only single item arrays within this data (e.g. `LOGO_SEGA` in `aet_adv.bin`.) As this is the case it's likely this is not the animation frame properties.

There seems to be additional floats on the end of the pointed to float arrays when there is more than one entry, indicated by the pointer for the next array being far after the described floats. In `aet_alarm.bin`'s `HELP` Node, the Scale Y array is described to contain 13 entries, but the Opacity array pointer indicates there are actually 39 (heh) which is `13 * 3`, so seemingly there is three floats per entry when there are multiple entries. This lines up when testing.

So if a Placement property float array contains a single element, then the property is that element. If it contains multiple then the first `N` entries are the frames at which the properties occur (`N` being the element count before the array pointer,) and the properties are somewhere within the succedeing floats, of which there are `N * 2` elements. The succedeing floats seem to be interleaved, where there is two sets of floats that each have elements one after the other (first array item, second array item, first array item, etc.) This is based on the values only making some sense when read like this, as they jump around a lot and do not relate if it was a single array.

The first value seems to largely line up with what appears in-game, such as the `HELP` node in `aet_alarm.bin`, where the first array of succedeing floats for the Scale X property looks like the animation for the opening/closing animations of the help menu. However, as per usual, on PV lines these values are very high for their properties, such as the Position X/Y properties, where they are commonly in the high one thousands.

Position refers to the position on the screen of the Node, and Origin refers to the position within that Node that the Position is centered on. Child nodes are then positioned relative to their parents Origin.

## Sprite Group

Sprite groups, well, group Sprites.
Most of the time these only contain a single Sprite, but occasionally they contain multiple like in "glow" (PV 44, `AET_PV044_MAIN` Sprite Group `76`) and "Hello, Planet" (PV 74, `AET_PV074_MAIN` Sprite Group `6`,) where it is used to alternate between frames in an animation.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x3` | Multiply colour? Typically `0xffffff` where it could be `0xRRGGBB` |
| `0x3` | `0x1` | Padding? Always `0x00` |
| `0x4` | `0x2` | Width |
| `0x6` | `0x2` | Height |
| `0x8` | `0x4` | Unknown float, always `0.0` or `1.0` |
| `0xc` | `0x4` | Number of Sprites  |
| `0x10` | `0x4` | Pointer to Sprite array |

The width and height should always line up with the size of the Sprites, but if it does not then the Sprites should be scaled to fit the size.

## Sprite

Sprites describe how to draw images from SCRs within a Scene.

| Offset | Length | Description |
| --- | --- | --- |
| `0x0` | `0x4` | Index of SCR within the Scene |
| `0x4` | `0x8` | Padding? Always `0x0000000000000000` |
| `0xc` | `0x4` | Unknown float, always `1.0` |
| `0x10` | `0x4` | Unknown float, always `1.0` |

Sometimes the SCR name index points to the last item in the SCR names array, which is always a single space.
