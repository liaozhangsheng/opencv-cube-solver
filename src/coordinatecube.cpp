#include "cache_manager.h"
#include "coordinatecube.h"

int twistMove[N_TWIST][N_MOVE];
int flipMove[N_FLIP][N_MOVE];
int parityMove[2][18] = {
    {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}};
int FRtoBR_Move[N_FRtoBR][N_MOVE];
int URFtoDLF_Move[N_URFtoDLF][N_MOVE] = {{0}};
int URtoDF_Move[N_URtoDF][N_MOVE] = {{0}};
int URtoUL_Move[N_URtoUL][N_MOVE] = {{0}};
int UBtoDF_Move[N_UBtoDF][N_MOVE] = {{0}};
int MergeURtoULandUBtoDF[336][336] = {{0}};
char Slice_URFtoDLF_Parity_Prun[N_SLICE2 * N_URFtoDLF * N_PARITY / 2] = {0};
char Slice_URtoDF_Parity_Prun[N_SLICE2 * N_URtoDF * N_PARITY / 2] = {0};
char Slice_Twist_Prun[N_SLICE1 * N_TWIST / 2 + 1] = {0};
char Slice_Flip_Prun[N_SLICE1 * N_FLIP / 2] = {0};

bool PRUNING_INITED = false;

void move(CoordCube &coordcube, int m, const std::filesystem::path &cache_dir)
{
    if (!PRUNING_INITED)
    {
        init_pruning(cache_dir);
    }
    coordcube.twist = twistMove[coordcube.twist][m];
    coordcube.flip = flipMove[coordcube.flip][m];
    coordcube.parity = parityMove[coordcube.parity][m];
    coordcube.FRtoBR = FRtoBR_Move[coordcube.FRtoBR][m];
    coordcube.URFtoDLF = URFtoDLF_Move[coordcube.URFtoDLF][m];
    coordcube.URtoUL = URtoUL_Move[coordcube.URtoUL][m];
    coordcube.UBtoDF = UBtoDF_Move[coordcube.UBtoDF][m];
    if (coordcube.URtoUL < 336 && coordcube.UBtoDF < 336) // updated only if UR,UF,UL,UB,DR,DF
        // are not in UD-slice
        coordcube.URtoDF = MergeURtoULandUBtoDF[coordcube.URtoUL][coordcube.UBtoDF];
}

CoordCube get_CoordCube(CubeState &cubestate)
{
    CoordCube result;

    result.twist = get_twist(cubestate);
    result.flip = get_flip(cubestate);
    result.parity = corner_parity(cubestate);
    result.FRtoBR = get_FR_to_BR(cubestate);
    result.URFtoDLF = get_URF_to_DLF(cubestate);
    result.URtoUL = get_UR_to_UL(cubestate);
    result.UBtoDF = get_UB_to_DF(cubestate);
    result.URtoDF = get_UR_to_DF(cubestate); // only needed in phase2

    return result;
}

void init_pruning(const std::filesystem::path &cache_dir)
{
    CubeState *moveCube = get_cubestate_after_move();

    if (!read_cache(cache_dir, "twistMove", (void *)twistMove, sizeof(twistMove)))
    {
        CubeState tmp;
        for (int i = 0; i < N_TWIST; i++)
        {
            set_twist(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    corner_multiply(tmp, moveCube[j]);
                    twistMove[i][3 * j + k] = get_twist(tmp);
                }
                corner_multiply(tmp, moveCube[j]); // 4. faceturn restores
            }
        }
        write_cache(cache_dir, "twistMove", (void *)twistMove, sizeof(twistMove));
    }

    if (!read_cache(cache_dir, "flipMove", (void *)flipMove, sizeof(flipMove)))
    {
        CubeState tmp;
        for (int i = 0; i < N_FLIP; i++)
        {
            set_flip(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    edge_multiply(tmp, moveCube[j]);
                    flipMove[i][3 * j + k] = get_flip(tmp);
                }
                edge_multiply(tmp, moveCube[j]);
            }
        }

        write_cache(cache_dir, "flipMove", (void *)flipMove, sizeof(flipMove));
    }

    if (!read_cache(cache_dir, "FRtoBR_Move", (void *)FRtoBR_Move, sizeof(FRtoBR_Move)))
    {
        CubeState tmp;
        for (int i = 0; i < N_FRtoBR; i++)
        {
            set_FR_to_BR(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    edge_multiply(tmp, moveCube[j]);
                    FRtoBR_Move[i][3 * j + k] = get_FR_to_BR(tmp);
                }
                edge_multiply(tmp, moveCube[j]);
            }
        }
        write_cache(cache_dir, "FRtoBR_Move", (void *)FRtoBR_Move, sizeof(FRtoBR_Move));
    }

    if (!read_cache(cache_dir, "URFtoDLF_Move", (void *)URFtoDLF_Move, sizeof(URFtoDLF_Move)))
    {
        CubeState tmp;
        for (int i = 0; i < N_URFtoDLF; i++)
        {
            set_URF_to_DLF(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    corner_multiply(tmp, moveCube[j]);
                    URFtoDLF_Move[i][3 * j + k] = get_URF_to_DLF(tmp);
                }
                corner_multiply(tmp, moveCube[j]);
            }
        }

        write_cache(cache_dir, "URFtoDLF_Move", (void *)URFtoDLF_Move, sizeof(URFtoDLF_Move));
    }

    if (!read_cache(cache_dir, "URtoDF_Move", (void *)URtoDF_Move, sizeof(URtoDF_Move)))
    {
        CubeState tmp;
        for (int i = 0; i < N_URtoDF; i++)
        {
            set_UR_to_DF(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    edge_multiply(tmp, moveCube[j]);
                    URtoDF_Move[i][3 * j + k] = get_UR_to_DF(tmp);
                    // Table values are only valid for phase 2 moves!
                    // For phase 1 moves, casting to int is not possible.
                }
                edge_multiply(tmp, moveCube[j]);
            }
        }

        write_cache(cache_dir, "URtoDF_Move", (void *)URtoDF_Move, sizeof(URtoDF_Move));
    }

    if (!read_cache(cache_dir, "URtoUL_Move", (void *)URtoUL_Move, sizeof(URtoUL_Move)))
    {
        CubeState tmp;
        for (int i = 0; i < N_URtoUL; i++)
        {
            set_UR_to_UL(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    edge_multiply(tmp, moveCube[j]);
                    URtoUL_Move[i][3 * j + k] = get_UR_to_UL(tmp);
                }
                edge_multiply(tmp, moveCube[j]);
            }
        }

        write_cache(cache_dir, "URtoUL_Move", (void *)URtoUL_Move, sizeof(URtoUL_Move));
    }

    if (!read_cache(cache_dir, "UBtoDF_Move", (void *)UBtoDF_Move, sizeof(UBtoDF_Move)))
    {
        CubeState tmp;
        for (int i = 0; i < N_UBtoDF; i++)
        {
            set_UB_to_DF(tmp, i);
            for (int j = 0; j < 6; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    edge_multiply(tmp, moveCube[j]);
                    UBtoDF_Move[i][3 * j + k] = get_UB_to_DF(tmp);
                }
                edge_multiply(tmp, moveCube[j]);
            }
        }

        write_cache(cache_dir, "UBtoDF_Move", (void *)UBtoDF_Move, sizeof(UBtoDF_Move));
    }

    if (!read_cache(cache_dir, "MergeURtoULandUBtoDF", (void *)MergeURtoULandUBtoDF, sizeof(MergeURtoULandUBtoDF)))
    {
        // for i, j <336 the six edges UR,UF,UL,UB,DR,DF are not in the
        // UD-slice and the index is <20160
        int uRtoUL, uBtoDF;
        for (uRtoUL = 0; uRtoUL < 336; uRtoUL++)
        {
            for (uBtoDF = 0; uBtoDF < 336; uBtoDF++)
            {
                MergeURtoULandUBtoDF[uRtoUL][uBtoDF] = get_UR_to_DF_standalone(uRtoUL, uBtoDF);
            }
        }
        write_cache(cache_dir, "MergeURtoULandUBtoDF", (void *)MergeURtoULandUBtoDF, sizeof(MergeURtoULandUBtoDF));
    }

    if (!read_cache(cache_dir, "Slice_URFtoDLF_Parity_Prun", (void *)Slice_URFtoDLF_Parity_Prun, sizeof(Slice_URFtoDLF_Parity_Prun)))
    {
        int depth = 0, done = 1;
        int i, j;
        for (i = 0; i < N_SLICE2 * N_URFtoDLF * N_PARITY / 2; i++)
            Slice_URFtoDLF_Parity_Prun[i] = -1;
        set_pruning(Slice_URFtoDLF_Parity_Prun, 0, 0);
        while (done != N_SLICE2 * N_URFtoDLF * N_PARITY)
        {
            // printf("%d %d %d\n", done, N_SLICE2 * N_URFtoDLF * N_PARITY, depth);
            for (i = 0; i < N_SLICE2 * N_URFtoDLF * N_PARITY; i++)
            {
                int parity = i % 2;
                int URFtoDLF = (i / 2) / N_SLICE2;
                int slice = (i / 2) % N_SLICE2;
                if (get_pruning(Slice_URFtoDLF_Parity_Prun, i) == depth)
                {
                    for (j = 0; j < 18; j++)
                    {
                        int newSlice;
                        int newURFtoDLF;
                        int newParity;
                        switch (j)
                        {
                        case 3:
                        case 5:
                        case 6:
                        case 8:
                        case 12:
                        case 14:
                        case 15:
                        case 17:
                            continue;
                        default:
                            newSlice = FRtoBR_Move[slice][j];
                            newURFtoDLF = URFtoDLF_Move[URFtoDLF][j];
                            newParity = parityMove[parity][j];
                            if (get_pruning(Slice_URFtoDLF_Parity_Prun, (N_SLICE2 * newURFtoDLF + newSlice) * 2 + newParity) == 0x0f)
                            {
                                set_pruning(Slice_URFtoDLF_Parity_Prun, (N_SLICE2 * newURFtoDLF + newSlice) * 2 + newParity,
                                            (signed char)(depth + 1));
                                done++;
                            }
                        }
                    }
                }
            }
            depth++;
        }
        write_cache(cache_dir, "Slice_URFtoDLF_Parity_Prun", (void *)Slice_URFtoDLF_Parity_Prun, sizeof(Slice_URFtoDLF_Parity_Prun));
    }

    if (!read_cache(cache_dir, "Slice_URtoDF_Parity_Prun", (void *)Slice_URtoDF_Parity_Prun, sizeof(Slice_URtoDF_Parity_Prun)))
    {
        int depth = 0, done = 1;
        int i, j;
        for (i = 0; i < N_SLICE2 * N_URtoDF * N_PARITY / 2; i++)
            Slice_URtoDF_Parity_Prun[i] = -1;
        set_pruning(Slice_URtoDF_Parity_Prun, 0, 0);
        while (done != N_SLICE2 * N_URtoDF * N_PARITY)
        {
            for (i = 0; i < N_SLICE2 * N_URtoDF * N_PARITY; i++)
            {
                int parity = i % 2;
                int URtoDF = (i / 2) / N_SLICE2;
                int slice = (i / 2) % N_SLICE2;
                if (get_pruning(Slice_URtoDF_Parity_Prun, i) == depth)
                {
                    for (j = 0; j < 18; j++)
                    {
                        int newSlice;
                        int newURtoDF;
                        int newParity;
                        switch (j)
                        {
                        case 3:
                        case 5:
                        case 6:
                        case 8:
                        case 12:
                        case 14:
                        case 15:
                        case 17:
                            continue;
                        default:
                            newSlice = FRtoBR_Move[slice][j];
                            newURtoDF = URtoDF_Move[URtoDF][j];
                            newParity = parityMove[parity][j];
                            if (get_pruning(Slice_URtoDF_Parity_Prun, (N_SLICE2 * newURtoDF + newSlice) * 2 + newParity) == 0x0f)
                            {
                                set_pruning(Slice_URtoDF_Parity_Prun, (N_SLICE2 * newURtoDF + newSlice) * 2 + newParity,
                                            (signed char)(depth + 1));
                                done++;
                            }
                        }
                    }
                }
            }
            depth++;
        }
        write_cache(cache_dir, "Slice_URtoDF_Parity_Prun", (void *)Slice_URtoDF_Parity_Prun, sizeof(Slice_URtoDF_Parity_Prun));
    }

    if (!read_cache(cache_dir, "Slice_Twist_Prun", (void *)Slice_Twist_Prun, sizeof(Slice_Twist_Prun)))
    {
        int depth = 0, done = 1;
        int i, j;
        for (i = 0; i < N_SLICE1 * N_TWIST / 2 + 1; i++)
            Slice_Twist_Prun[i] = -1;
        set_pruning(Slice_Twist_Prun, 0, 0);
        while (done != N_SLICE1 * N_TWIST)
        {
            for (i = 0; i < N_SLICE1 * N_TWIST; i++)
            {
                int twist = i / N_SLICE1, slice = i % N_SLICE1;
                if (get_pruning(Slice_Twist_Prun, i) == depth)
                {
                    for (j = 0; j < 18; j++)
                    {
                        int newSlice = FRtoBR_Move[slice * 24][j] / 24;
                        int newTwist = twistMove[twist][j];
                        if (get_pruning(Slice_Twist_Prun, N_SLICE1 * newTwist + newSlice) == 0x0f)
                        {
                            set_pruning(Slice_Twist_Prun, N_SLICE1 * newTwist + newSlice, (signed char)(depth + 1));
                            done++;
                        }
                    }
                }
            }
            depth++;
        }
        write_cache(cache_dir, "Slice_Twist_Prun", (void *)Slice_Twist_Prun, sizeof(Slice_Twist_Prun));
    }

    if (!read_cache(cache_dir, "Slice_Flip_Prun", (void *)Slice_Flip_Prun, sizeof(Slice_Flip_Prun)))
    {
        int depth = 0, done = 1;
        int i, j;
        for (i = 0; i < N_SLICE1 * N_FLIP / 2; i++)
            Slice_Flip_Prun[i] = -1;
        set_pruning(Slice_Flip_Prun, 0, 0);
        while (done != N_SLICE1 * N_FLIP)
        {
            for (i = 0; i < N_SLICE1 * N_FLIP; i++)
            {
                int flip = i / N_SLICE1, slice = i % N_SLICE1;
                if (get_pruning(Slice_Flip_Prun, i) == depth)
                {
                    for (j = 0; j < 18; j++)
                    {
                        int newSlice = FRtoBR_Move[slice * 24][j] / 24;
                        int newFlip = flipMove[flip][j];
                        if (get_pruning(Slice_Flip_Prun, N_SLICE1 * newFlip + newSlice) == 0x0f)
                        {
                            set_pruning(Slice_Flip_Prun, N_SLICE1 * newFlip + newSlice, (signed char)(depth + 1));
                            done++;
                        }
                    }
                }
            }
            depth++;
        }
        write_cache(cache_dir, "Slice_Flip_Prun", (void *)Slice_Flip_Prun, sizeof(Slice_Flip_Prun));
    }

    PRUNING_INITED = true;
}

void set_pruning(char *table, int index, char value)
{
    if ((index & 1) == 0)
        table[index >> 1] &= 0xf0 | value;
    else
        table[index >> 1] &= 0x0f | (value << 4);

}

// Extract pruning value
char get_pruning(char *table, int index)
{
    return (index & 1) == 0 ? (table[index >> 1] & 0x0f) : ((table[index >> 1] >> 4) & 0x0f);
}