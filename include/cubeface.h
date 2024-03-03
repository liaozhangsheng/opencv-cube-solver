#ifndef __CUBEFACE_H__
#define __CUBEFACE_H__

#include <iostream>
#include <array>
#include <memory>

#include "cube.h"

using namespace cube;

// Cube on the block level
struct CubeFace
{
    std::array<ColorFace, 54> block = {U, U, U, U, U, U, U, U, U,
                                       R, R, R, R, R, R, R, R, R,
                                       F, F, F, F, F, F, F, F, F,
                                       D, D, D, D, D, D, D, D, D,
                                       L, L, L, L, L, L, L, L, L,
                                       B, B, B, B, B, B, B, B, B};
};

// forward declaration
struct CubeState;

// Map the corner positions to block positions. cornerblock[URF.ordinal()][0] e.g. gives the position of the
// block in the URF corner position, which defines the orientation.<br>
// cornerblock[URF.ordinal()][1] and cornerblock[URF.ordinal()][2] give the position of the other two blocks
// of the URF corner (clockwise).
extern Block cornerBlock[8][3];

// Map the edge positions to block positions. edgeblock[UR.ordinal()][0] e.g. gives the position of the block in
// the UR edge position, which defines the orientation.<br>
// edgeblock[UR.ordinal()][1] gives the position of the other block
extern Block edgeBlock[12][2];

// Map the corner positions to block colors.
extern ColorFace cornerColor[8][3];

// Map the edge positions to block colors.
extern ColorFace edgeColor[12][2];

CubeFace string_to_cubeface(const std::string &cubeString);
std::string cubeface_to_string(const CubeFace &cubeface);
CubeState to_CubeState(const CubeFace &cubeface);

#endif
