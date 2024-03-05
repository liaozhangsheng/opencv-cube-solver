#ifndef __CUBESTATE_H__
#define __CUBESTATE_H__

#include <iostream>

#include "cube.h"

using namespace cube;

struct CubeState
{
    Corner cornersPermutation[8] = {URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB};
    char cornersOrientation[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    Edge edgesPermutation[12] = {UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR};
    char edgesOrientation[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

// forward declaration
struct CubeFace;

CubeState *get_cubestate_after_move();

int Cnk(int n, int k);

template <typename T>
void rotate_left(T* arr, int l, int r);

template <typename T>
void rotate_right(T* arr, int l, int r);

CubeFace to_CubeFace(CubeState &state);

void corner_multiply(CubeState &lhs, CubeState &rhs);
void edge_multiply(CubeState &lhs, CubeState &rhs);
void multiply(CubeState &lhs, CubeState &rhs);

void inv_cubestate(CubeState &state, CubeState &c);

int get_twist(CubeState &state);
void set_twist(CubeState &state, int twist);

int get_flip(CubeState &state);
void set_flip(CubeState &state, int flip);

int corner_parity(CubeState &state);
int edge_parity(CubeState &state);

int get_FR_to_BR(CubeState &state);
void set_FR_to_BR(CubeState &state, int idx);

int get_URF_to_DLF(CubeState &state);
void set_URF_to_DLF(CubeState &state, int idx);

int get_UR_to_DF(CubeState &state);
void set_UR_to_DF(CubeState &state, int idx);

int get_UR_to_UL(CubeState &cubiecube);
void set_UR_to_UL(CubeState &cubiecube, int idx);
int get_UB_to_DF(CubeState &cubiecube);
void set_UB_to_DF(CubeState &cubiecube, int idx);
int get_URF_to_DLB(CubeState &cubiecube);
void set_URF_to_DLB(CubeState &cubiecube, int idx);
int get_UR_to_BR(CubeState &cubiecube);
void set_UR_to_BR(CubeState &cubiecube, int idx);

int verify(CubeState &state);

int get_UR_to_DF_standalone(int idx1, int idx2);

#endif