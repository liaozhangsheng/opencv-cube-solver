#include "include/cubestate.h"
#include "include/cubeface.h"

void initializeMove(CubeState &cube, const Corner cornersPermutation[8], const char cornersOrientation[8], const Edge edgesPermutation[12], const char edgesOrientation[12])
{
    memcpy(cube.cornersPermutation, cornersPermutation, sizeof(Corner) * 8);
    memcpy(cube.cornersOrientation, cornersOrientation, sizeof(char) * 8);
    memcpy(cube.edgesPermutation, edgesPermutation, sizeof(Edge) * 12);
    memcpy(cube.edgesOrientation, edgesOrientation, sizeof(char) * 12);
}

CubeState *get_cubestate_after_move()
{
    static CubeState moveCube[6];
    static bool moveCube_initialized = false;

    static const Corner cpU[8] = {UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB};
    static const char coU[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    static const Edge epU[12] = {UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR};
    static const char eoU[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static const Corner cpR[8] = {DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR};
    static const char coR[8] = {2, 0, 0, 1, 1, 0, 0, 2};
    static const Edge epR[12] = {FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR};
    static const char eoR[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static const Corner cpF[8] = {UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB};
    static const char coF[8] = {1, 2, 0, 0, 2, 1, 0, 0};
    static const Edge epF[12] = {UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR};
    static const char eoF[12] = {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0};
    static const Corner cpD[8] = {URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR};
    static const char coD[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    static const Edge epD[12] = {UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR};
    static const char eoD[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static const Corner cpL[8] = {URF, ULB, DBL, UBR, DFR, UFL, DLF, DRB};
    static const char coL[8] = {0, 1, 2, 0, 0, 2, 1, 0};
    static const Edge epL[12] = {UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR};
    static const char eoL[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static const Corner cpB[8] = {URF, UFL, UBR, DRB, DFR, DLF, ULB, DBL};
    static const char coB[8] = {0, 0, 1, 2, 0, 0, 2, 1};
    static const Edge epB[12] = {UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB};
    static const char eoB[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1};

    if (!moveCube_initialized)
    {
        initializeMove(moveCube[0], cpU, coU, epU, eoU);
        initializeMove(moveCube[1], cpR, coR, epR, eoR);
        initializeMove(moveCube[2], cpF, coF, epF, eoF);
        initializeMove(moveCube[3], cpD, coD, epD, eoD);
        initializeMove(moveCube[4], cpL, coL, epL, eoL);
        initializeMove(moveCube[5], cpB, coB, epB, eoB);
    }

    return moveCube;
}
int Cnk(int n, int k)
{
    int i, j, s;
    if (n < k)
        return 0;
    if (k > n / 2)
        k = n - k;
    for (s = 1, i = n, j = 1; i != n - k; i--, j++)
    {
        s *= i;
        s /= j;
    }
    return s;
}

template <typename T>
void rotate_left(T *arr, int l, int r)
{
    T temp = arr[l];
    for (int i = l; i < r; i++)
        arr[i] = arr[i + 1];
    arr[r] = temp;
}

template <typename T>
void rotate_right(T *arr, int l, int r)
{
    T temp = arr[r];
    for (int i = r; i > l; i--)
        arr[i] = arr[i - 1];
    arr[l] = temp;
}

CubeFace to_CubeFace(CubeState &cubestate)
{
    int i, j, n;
    char ori;
    CubeFace cfRet;

    for (i = 0; i < CORNER_COUNT; i++)
    {
        j = cubestate.cornersPermutation[i]; // cornercubie with index j is at
        // cornerposition with index i
        ori = cubestate.cornersOrientation[i]; // Orientation of this cubie
        for (n = 0; n < 3; n++)
            cfRet.block[cornerBlock[i][(n + ori) % 3]] = cornerColor[j][n];
    }
    for (i = 0; i < EDGE_COUNT; i++)
    {
        j = cubestate.edgesPermutation[i]; // edgecubie with index j is at edgeposition
        // with index i
        ori = cubestate.edgesOrientation[i]; // Orientation of this cubie
        for (n = 0; n < 2; n++)
            cfRet.block[edgeBlock[i][(n + ori) % 2]] = edgeColor[j][n];
    }
    return cfRet;
}

void corner_multiply(CubeState &lhs, CubeState &rhs)
{
    int corn;
    char oriA, oriB, ori;
    Corner cPerm[8];
    char cOri[8];
    for (corn = 0; corn < CORNER_COUNT; corn++)
    {
        cPerm[corn] = lhs.cornersPermutation[rhs.cornersPermutation[corn]];

        oriA = lhs.cornersOrientation[rhs.cornersPermutation[corn]];
        oriB = rhs.cornersOrientation[corn];
        ori = 0;

        if (oriA < 3 && oriB < 3) // if both cubes are regular cubes...
        {
            ori = oriA + oriB; // just do an addition modulo 3 here
            if (ori >= 3)
                ori -= 3; // the composition is a regular cube

            // +++++++++++++++++++++not used in this implementation +++++++++++++++++++++++++++++++++++
        }
        else if (oriA < 3 && oriB >= 3) // if cube b is in a mirrored
        // state...
        {
            ori = oriA + oriB;
            if (ori >= 6)
                ori -= 3; // the composition is a mirrored cube
        }
        else if (oriA >= 3 && oriB < 3) // if cube a is an a mirrored
        // state...
        {
            ori = oriA - oriB;
            if (ori < 3)
                ori += 3; // the composition is a mirrored cube
        }
        else if (oriA >= 3 && oriB >= 3) // if both cubes are in mirrored
        // states...
        {
            ori = oriA - oriB;
            if (ori < 0)
                ori += 3; // the composition is a regular cube
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        }
        cOri[corn] = ori;
    }
    for (corn = 0; corn < CORNER_COUNT; corn++)
    {
        lhs.cornersPermutation[corn] = cPerm[corn];
        lhs.cornersOrientation[corn] = cOri[corn];
    }
}

void edge_multiply(CubeState &lhs, CubeState &rhs)
{
    int edge;
    Edge ePerm[12];
    char eOri[12];

    for (edge = 0; edge < EDGE_COUNT; edge++)
    {
        ePerm[edge] = lhs.edgesPermutation[rhs.edgesPermutation[edge]];
        eOri[edge] = (rhs.edgesOrientation[edge] + lhs.edgesOrientation[rhs.edgesPermutation[edge]]) % 2;
    }
    for (edge = 0; edge < EDGE_COUNT; edge++)
    {
        lhs.edgesPermutation[edge] = ePerm[edge];
        lhs.edgesOrientation[edge] = eOri[edge];
    }
}

void multiply(CubeState &lhs, CubeState &rhs)
{
    corner_multiply(lhs, rhs);
    edge_multiply(lhs, rhs);
}

void inv_cubestate(CubeState &lhs, CubeState &rhs)
{
    for (int edge = 0; edge < EDGE_COUNT; edge++)
        rhs.edgesPermutation[lhs.edgesPermutation[edge]] = static_cast<Edge>(edge);
    for (int edge = 0; edge < EDGE_COUNT; edge++)
        rhs.edgesOrientation[edge] = lhs.edgesOrientation[rhs.edgesPermutation[edge]];
    for (int corn = 0; corn < CORNER_COUNT; corn++)
        rhs.cornersPermutation[lhs.cornersPermutation[corn]] = static_cast<Corner>(corn);
    for (int corn = 0; corn < CORNER_COUNT; corn++)
    {
        char ori = lhs.cornersOrientation[rhs.cornersPermutation[corn]];
        if (ori >= 3) // Just for completeness. We do not invert mirrored
            // cubes in the program.
            rhs.cornersOrientation[corn] = ori;
        else
        { // the standard case
            rhs.cornersOrientation[corn] = -ori;
            if (rhs.cornersOrientation[corn] < 0)
                rhs.cornersOrientation[corn] += 3;
        }
    }
}

int get_twist(CubeState &cubestate)
{
    int ret = 0;
    for (int i = URF; i < DRB; i++)
        ret = 3 * ret + cubestate.cornersOrientation[i];
    return ret;
}

void set_twist(CubeState &cubestate, int twist)
{
    int twistParity = 0;
    for (int i = DRB - 1; i >= URF; i--)
    {
        twistParity += cubestate.cornersOrientation[i] = (char)(twist % 3);
        twist /= 3;
    }
    cubestate.cornersOrientation[DRB] = (char)((3 - twistParity % 3) % 3);
}

int get_flip(CubeState &cubestate)
{
    int ret = 0;
    for (int i = UR; i < BR; i++)
        ret = 2 * ret + cubestate.edgesOrientation[i];
    return ret;
}

void set_flip(CubeState &cubestate, int flip)
{
    int flipParity = 0;
    for (int i = BR - 1; i >= UR; i--)
    {
        flipParity += cubestate.edgesOrientation[i] = (char)(flip % 2);
        flip /= 2;
    }
    cubestate.edgesOrientation[BR] = (char)((2 - flipParity % 2) % 2);
}

int corner_parity(CubeState &cubestate)
{
    int s = 0;
    for (int i = DRB; i >= URF + 1; i--)
        for (int j = i - 1; j >= URF; j--)
            if (cubestate.cornersPermutation[j] > cubestate.cornersPermutation[i])
                s++;
    return s % 2;
}

int edge_parity(CubeState &cubestate)
{
    int s = 0;
    for (int i = BR; i >= UR + 1; i--)
        for (int j = i - 1; j >= UR; j--)
            if (cubestate.edgesPermutation[j] > cubestate.edgesPermutation[i])
                s++;
    return s % 2;
}

int get_FR_to_BR(CubeState &cubestate)
{
    int a = 0, x = 0, j;
    int b = 0;
    Edge edge4[4];
    // compute the index a < (12 choose 4) and the permutation array perm.
    for (j = BR; j >= UR; j--)
        if (FR <= cubestate.edgesPermutation[j] && cubestate.edgesPermutation[j] <= BR)
        {
            a += Cnk(11 - j, x + 1);
            edge4[3 - x++] = cubestate.edgesPermutation[j];
        }

    for (j = 3; j > 0; j--) // compute the index b < 4! for the
    // permutation in perm
    {
        int k = 0;
        while (edge4[j] != j + 8)
        {
            rotate_left<Edge>(edge4, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return 24 * a + b;
}
void set_FR_to_BR(CubeState &cubestate, int idx)
{
    int x, j, k, e;
    Edge sliceEdge[4] = {FR, FL, BL, BR};
    Edge otherEdge[8] = {UR, UF, UL, UB, DR, DF, DL, DB};
    int b = idx % 24; // Permutation
    int a = idx / 24; // Combination
    for (e = 0; e < EDGE_COUNT; e++)
        cubestate.edgesPermutation[e] = DB; // Use UR to invalidate all edges

    for (j = 1; j < 4; j++) // generate permutation from index b
    {
        k = b % (j + 1);
        b /= j + 1;
        while (k-- > 0)
            rotate_right<Edge>(sliceEdge, 0, j);
    }

    x = 3; // generate combination and set slice edges
    for (j = UR; j <= BR; j++)
        if (a - Cnk(11 - j, x + 1) >= 0)
        {
            cubestate.edgesPermutation[j] = sliceEdge[3 - x];
            a -= Cnk(11 - j, x-- + 1);
        }
    x = 0; // set the remaining edges UR..DB
    for (j = UR; j <= BR; j++)
        if (cubestate.edgesPermutation[j] == DB)
            cubestate.edgesPermutation[j] = otherEdge[x++];
}

int get_URF_to_DLF(CubeState &cubestate)
{
    int a = 0, x = 0, j, b = 0;
    Corner corner6[6];
    // compute the index a < (8 choose 6) and the corner permutation.
    for (j = URF; j <= DRB; j++)
        if (cubestate.cornersPermutation[j] <= DLF)
        {
            a += Cnk(j, x + 1);
            corner6[x++] = cubestate.cornersPermutation[j];
        }

    for (j = 5; j > 0; j--) // compute the index b < 6! for the
    // permutation in corner6
    {
        int k = 0;
        while (corner6[j] != j)
        {
            rotate_left<Corner>(corner6, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return 720 * a + b;
}

void set_URF_to_DLF(CubeState &cubestate, int idx)
{
    int x;
    Corner corner6[6] = {URF, UFL, ULB, UBR, DFR, DLF};
    Corner otherCorner[2] = {DBL, DRB};
    int b = idx % 720; // Permutation
    int a = idx / 720; // Combination
    int c, j, k;
    for (c = 0; c < CORNER_COUNT; c++)
        cubestate.cornersPermutation[c] = DRB; // Use DRB to invalidate all corners

    for (j = 1; j < 6; j++) // generate permutation from index b
    {
        k = b % (j + 1);
        b /= j + 1;
        while (k-- > 0)
            rotate_right<Corner>(corner6, 0, j);
    }
    x = 5; // generate combination and set corners
    for (j = DRB; j >= 0; j--)
        if (a - Cnk(j, x + 1) >= 0)
        {
            cubestate.cornersPermutation[j] = corner6[x];
            a -= Cnk(j, x-- + 1);
        }
    x = 0;
    for (j = URF; j <= DRB; j++)
        if (cubestate.cornersPermutation[j] == DRB)
            cubestate.cornersPermutation[j] = otherCorner[x++];
}

int get_UR_to_DF(CubeState &cubestate)
{
    int a = 0, x = 0;
    int b = 0, j;
    Edge edge6[6];
    // compute the index a < (12 choose 6) and the edge permutation.
    for (j = UR; j <= BR; j++)
        if (cubestate.edgesPermutation[j] <= DF)
        {
            a += Cnk(j, x + 1);
            edge6[x++] = cubestate.edgesPermutation[j];
        }

    for (j = 5; j > 0; j--) // compute the index b < 6! for the
    // permutation in edge6
    {
        int k = 0;
        while (edge6[j] != j)
        {
            rotate_left<Edge>(edge6, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return 720 * a + b;
}

void set_UR_to_DF(CubeState &cubestate, int idx)
{
    int x, e, j, k;
    Edge edge6[6] = {UR, UF, UL, UB, DR, DF};
    Edge otherEdge[6] = {DL, DB, FR, FL, BL, BR};
    int b = idx % 720; // Permutation
    int a = idx / 720; // Combination

    for (e = 0; e < EDGE_COUNT; e++)
        cubestate.edgesPermutation[e] = BR; // Use BR to invalidate all edges

    for (j = 1; j < 6; j++) // generate permutation from index b
    {
        k = b % (j + 1);
        b /= j + 1;
        while (k-- > 0)
            rotate_right<Edge>(edge6, 0, j);
    }
    x = 5; // generate combination and set edges
    for (j = BR; j >= 0; j--)
        if (a - Cnk(j, x + 1) >= 0)
        {
            cubestate.edgesPermutation[j] = edge6[x];
            a -= Cnk(j, x-- + 1);
        }
    x = 0; // set the remaining edges DL..BR
    for (j = UR; j <= BR; j++)
        if (cubestate.edgesPermutation[j] == BR)
            cubestate.edgesPermutation[j] = otherEdge[x++];
}

int get_UR_to_UL(CubeState &cubestate)
{
    int a = 0, b = 0, x = 0, j;
    Edge edge3[3];
    // compute the index a < (12 choose 3) and the edge permutation.
    for (j = UR; j <= BR; j++)
        if (cubestate.edgesPermutation[j] <= UL)
        {
            a += Cnk(j, x + 1);
            edge3[x++] = cubestate.edgesPermutation[j];
        }

    for (j = 2; j > 0; j--) // compute the index b < 3! for the
    // permutation in edge3
    {
        int k = 0;
        while (edge3[j] != j)
        {
            rotate_left<Edge>(edge3, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return 6 * a + b;
}

void set_UR_to_UL(CubeState &cubestate, int idx)
{
    int x, e, j, k;
    Edge edge3[3] = {UR, UF, UL};
    int b = idx % 6; // Permutation
    int a = idx / 6; // Combination
    for (e = 0; e < EDGE_COUNT; e++)
    {
        cubestate.edgesPermutation[e] = BR; // Use BR to invalidate all edges
    }

    for (j = 1; j < 3; j++)
    { // generate permutation from index b
        k = b % (j + 1);
        b /= j + 1;
        while (k-- > 0)
            rotate_right<Edge>(edge3, 0, j);
    }
    x = 2; // generate combination and set edges
    for (j = BR; j >= 0; j--)
    {
        if (a - Cnk(j, x + 1) >= 0)
        {
            cubestate.edgesPermutation[j] = edge3[x];
            a -= Cnk(j, x-- + 1);
        }
    }
}

int get_UB_to_DF(CubeState &cubestate)
{
    int a = 0, x = 0, b = 0, j;
    Edge edge3[3];
    // compute the index a < (12 choose 3) and the edge permutation.
    for (j = UR; j <= BR; j++)
        if (UB <= cubestate.edgesPermutation[j] && cubestate.edgesPermutation[j] <= DF)
        {
            a += Cnk(j, x + 1);
            edge3[x++] = cubestate.edgesPermutation[j];
        }

    for (j = 2; j > 0; j--) // compute the index b < 3! for the
    // permutation in edge3
    {
        int k = 0;
        while (edge3[j] != UB + j)
        {
            rotate_left<Edge>(edge3, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return 6 * a + b;
}

void set_UB_to_DF(CubeState &cubestate, int idx)
{
    int x, e, j, k;
    Edge edge3[3] = {UB, DR, DF};
    int b = idx % 6; // Permutation
    int a = idx / 6; // Combination
    for (e = 0; e < EDGE_COUNT; e++)
        cubestate.edgesPermutation[e] = BR; // Use BR to invalidate all edges

    for (j = 1; j < 3; j++) // generate permutation from index b
    {
        k = b % (j + 1);
        b /= j + 1;
        while (k-- > 0)
            rotate_right<Edge>(edge3, 0, j);
    }
    x = 2; // generate combination and set edges
    for (j = BR; j >= 0; j--)
        if (a - Cnk(j, x + 1) >= 0)
        {
            cubestate.edgesPermutation[j] = edge3[x];
            a -= Cnk(j, x-- + 1);
        }
}

int get_URF_to_DLB(CubeState &cubestate)
{
    Corner perm[8];
    int b = 0, i, j;
    for (i = 0; i < 8; i++)
        perm[i] = cubestate.cornersPermutation[i];
    for (j = 7; j > 0; j--) // compute the index b < 8! for the permutation in perm
    {
        int k = 0;
        while (perm[j] != j)
        {
            rotate_left<Corner>(perm, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return b;
}

void set_URF_to_DLB(CubeState &cubestate, int idx)
{
    Corner perm[8] = {URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB};
    int k, j;
    int x = 7; // set corners
    for (j = 1; j < 8; j++)
    {
        k = idx % (j + 1);
        idx /= j + 1;
        while (k-- > 0)
            rotate_right<Corner>(perm, 0, j);
    }

    for (j = 7; j >= 0; j--)
        cubestate.cornersPermutation[j] = perm[x--];
}

int get_UR_to_BR(CubeState &cubestate)
{
    Edge perm[12];
    int b = 0, i, j;
    for (i = 0; i < 12; i++)
        perm[i] = cubestate.edgesPermutation[i];
    for (j = 11; j > 0; j--) // compute the index b < 12! for the permutation in perm
    {
        int k = 0;
        while (perm[j] != j)
        {
            rotate_left<Edge>(perm, 0, j);
            k++;
        }
        b = (j + 1) * b + k;
    }
    return b;
}

void set_UR_to_BR(CubeState &cubestate, int idx)
{
    Edge perm[12] = {UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR};
    int k, j;
    int x = 11; // set edges
    for (j = 1; j < 12; j++)
    {
        k = idx % (j + 1);
        idx /= j + 1;
        while (k-- > 0)
            rotate_right<Edge>(perm, 0, j);
    }
    for (j = 11; j >= 0; j--)
        cubestate.edgesPermutation[j] = perm[x--];
}

int verify(CubeState &cubestate)
{
    int sum = 0, e, i, c;
    int edgeCount[12] = {0};
    int cornerCount[8] = {0};

    for (e = 0; e < EDGE_COUNT; e++)
        edgeCount[cubestate.edgesPermutation[e]]++;
    for (i = 0; i < 12; i++)
        if (edgeCount[i] != 1)
            return -2;

    for (i = 0; i < 12; i++)
        sum += cubestate.edgesOrientation[i];
    if (sum % 2 != 0)
        return -3;

    for (c = 0; c < CORNER_COUNT; c++)
        cornerCount[cubestate.cornersPermutation[c]]++;
    for (i = 0; i < 8; i++)
        if (cornerCount[i] != 1)
            return -4; // missing corners

    sum = 0;
    for (i = 0; i < 8; i++)
        sum += cubestate.cornersOrientation[i];
    if (sum % 3 != 0)
        return -5; // twisted corner

    if ((edge_parity(cubestate) ^ corner_parity(cubestate)) != 0)
        return -6; // parity error

    return 0; // cube ok
}

int get_UR_to_DF_standalone(int idx1, int idx2)
{
    CubeState a, b;
    set_UR_to_UL(a, idx1);
    set_UB_to_DF(b, idx2);
    for (int i = 0; i < 8; i++)
    {
        if (a.edgesPermutation[i] != BR)
        {
            if (b.edgesPermutation[i] != BR)
            { // collision
                return -1;
            }
            else
            {
                b.edgesPermutation[i] = a.edgesPermutation[i];
            }
        }
    }

    return get_UR_to_DF(b);
}
