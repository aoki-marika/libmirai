//
//  gl.h
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

// the glfw header contains many warnings, ignore them all
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop

// apple has a specific header for gl3
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif
