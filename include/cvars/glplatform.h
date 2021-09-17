/* This file is ported from the Pangolin project
 *
 * http://github.com/stevenlovegrove/Pangolin
 *
 * as part of the CVars project.
 *
 * Copyright (c) 2011 Steven Lovegrove, Richard Newcombe,
 *               2015 Christoffer Heckman
 *
 */

#ifndef CVARS_GLPLATFORM_H
#define CVARS_GLPLATFORM_H

//////////////////////////////////////////////////////////
// Attempt to portably include Necessary OpenGL headers
//////////////////////////////////////////////////////////

#include <cvars/config.h>

#ifdef WIN32
    // Define maths quantities when using <cmath> to match posix systems
    #define _USE_MATH_DEFINES

    // Don't define min / max macros in windows.h or other unnecessary macros
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    // Undef nuisance Windows.h macros which interfere with our methods
    #undef LoadImage
    #undef near
    #undef far
#endif

#include <glad/glad.h>


#endif // CVARS_GLPLATFORM_H
