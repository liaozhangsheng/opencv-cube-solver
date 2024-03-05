#include <iostream>
#include <memory>

#include "include/cubeface.h"
#include "include/cubestate.h"

Block cornerBlock[8][3] = {{U9, R1, F3}, {U7, F1, L3}, {U1, L1, B3}, {U3, B1, R3}, {D3, F9, R7}, {D1, L9, F7}, {D7, B9, L7}, {D9, R9, B7}};

Block edgeBlock[12][2] = {{U6, R2}, {U8, F2}, {U4, L2}, {U2, B2}, {D6, R8}, {D2, F8}, {D4, L8}, {D8, B8}, {F6, R4}, {F4, L6}, {B6, L4}, {B4, R6}};

ColorFace cornerColor[8][3] = {{U, R, F}, {U, F, L}, {U, L, B}, {U, B, R}, {D, F, R}, {D, L, F}, {D, B, L}, {D, R, B}};

ColorFace edgeColor[12][2] = {{U, R}, {U, F}, {U, L}, {U, B}, {D, R}, {D, F}, {D, L}, {D, B}, {F, R}, {F, L}, {B, L}, {B, R}};

CubeFace string_to_cubeface(const std::string &cubeString)
{
    CubeFace res;

    for (int i = 0; i < 54; i++)
    {
        switch (cubeString[i])
        {
        case 'U':
            res.block[i] = U;
            break;
        case 'R':
            res.block[i] = R;
            break;
        case 'F':
            res.block[i] = F;
            break;
        case 'D':
            res.block[i] = D;
            break;
        case 'L':
            res.block[i] = L;
            break;
        case 'B':
            res.block[i] = B;
            break;
        default:
            break;
        }
    }

    return res;
}

std::string cubeface_to_string(const CubeFace &cubeface)
{
    std::string res;
    for (int i = 0; i < 54; i++)
    {
        switch (cubeface.block[i])
        {
        case U:
            res[i] = 'U';
            break;
        case R:
            res[i] = 'R';
            break;
        case F:
            res[i] = 'F';
            break;
        case D:
            res[i] = 'D';
            break;
        case L:
            res[i] = 'L';
            break;
        case B:
            res[i] = 'B';
            break;
        default:
            break;
        }
    };

    return res;
}

CubeState to_CubeState(const CubeFace &cubeface)
{
    int ori;
    ColorFace col1, col2;
    CubeState ccRet;
    for (int i = 0; i < 8; i++)
        ccRet.cornersPermutation[i] = URF; // invalidate corners
    for (int i = 0; i < 12; i++)
        ccRet.edgesPermutation[i] = UR; // and edges

    for (int i = 0; i < CORNER_COUNT; i++)
    {
        // get the colors of the cubie at corner i, starting with U/D
        for (ori = 0; ori < 3; ori++)
            if (cubeface.block[cornerBlock[i][ori]] == U || cubeface.block[cornerBlock[i][ori]] == D)
                break;
        col1 = cubeface.block[cornerBlock[i][(ori + 1) % 3]];
        col2 = cubeface.block[cornerBlock[i][(ori + 2) % 3]];

        for (int j = 0; j < CORNER_COUNT; j++)
        {
            if (col1 == cornerColor[j][1] && col2 == cornerColor[j][2])
            {
                // in cornerposition i we have cornercubie j
                ccRet.cornersPermutation[i] = static_cast<Corner>(j);
                ccRet.cornersOrientation[i] = ori % 3;
                break;
            }
        }
    }

    for (int i = 0; i < EDGE_COUNT; i++)
    {
        for (int j = 0; j < EDGE_COUNT; j++)
        {
            if (cubeface.block[edgeBlock[i][0]] == edgeColor[j][0] && cubeface.block[edgeBlock[i][1]] == edgeColor[j][1])
            {
                ccRet.edgesPermutation[i] = static_cast<Edge>(j);
                ccRet.edgesOrientation[i] = 0;
                break;
            }
            if (cubeface.block[edgeBlock[i][0]] == edgeColor[j][1] && cubeface.block[edgeBlock[i][1]] == edgeColor[j][0])
            {
                ccRet.edgesPermutation[i] = static_cast<Edge>(j);
                ccRet.edgesOrientation[i] = 1;
                break;
            }
        }
    }
    return ccRet;
}
