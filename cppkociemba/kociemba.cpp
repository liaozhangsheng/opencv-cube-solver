#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

#include "include/kociemba.h"
#include "include/cubeface.h"
#include "include/coordinatecube.h"

std::map<int, char> ax_map = {{0, 'U'}, {1, 'R'}, {2, 'F'}, {3, 'D'}, {4, 'L'}, {5, 'B'}};
std::map<int, std::string> po_map = {{1, " "}, {2, "2 "}, {3, "' "}};

struct Search
{
    int ax[31];   // The axis of the move
    int po[31];   // The power of the move
    int flip[31]; // phase1 coordinates
    int twist[31];
    int slice[31];
    int parity[31]; // phase2 coordinates
    int URFtoDLF[31];
    int FRtoBR[31];
    int URtoUL[31];
    int UBtoDF[31];
    int URtoDF[31];
    int minDistPhase1[31]; // IDA* distance do goal estimations
    int minDistPhase2[31];
};

std::vector<std::string> parse_steps(const std::string &steps)
{
    std::vector<std::string> result;
    std::istringstream iss(steps);
    std::string step;

    while (std::getline(iss, step, ' '))
    {
        result.push_back(step);
    }

    return result;
}

// generate the solution string from the array data including a separator between phase1 and phase2 moves
std::string solution_to_string(Search &search, int length)
{
    std::string s;
    for (int i = 0; i < length; i++)
    {
        s.append(1, ax_map[search.ax[i]]);
        s.append(po_map[search.po[i]]);
    }
    return s;
}

// Apply phase2 of algorithm and return the combined phase1 and phase2 depth. In phase2, only the moves
// U,D,R2,F2,L2 and B2 are allowed.
int total_depth(Search &search, int depthPhase1, int maxDepth)
{
    int mv = 0, d1 = 0, d2 = 0, i;
    int maxDepthPhase2 = std::min(10, maxDepth - depthPhase1); // Allow only max 10 moves in phase2
    int depthPhase2;
    int n;
    bool busy = false;
    for (i = 0; i < depthPhase1; i++)
    {
        mv = 3 * search.ax[i] + search.po[i] - 1;
        // System.out.format("%d %d %d %d\n", i, mv, ax[i], po[i]);
        search.URFtoDLF[i + 1] = URFtoDLF_Move[search.URFtoDLF[i]][mv];
        search.FRtoBR[i + 1] = FRtoBR_Move[search.FRtoBR[i]][mv];
        search.parity[i + 1] = parityMove[search.parity[i]][mv];
    }

    if ((d1 = get_pruning(Slice_URFtoDLF_Parity_Prun,
                          (N_SLICE2 * search.URFtoDLF[depthPhase1] + search.FRtoBR[depthPhase1]) * 2 + search.parity[depthPhase1])) > maxDepthPhase2)
        return -1;

    for (i = 0; i < depthPhase1; i++)
    {
        mv = 3 * search.ax[i] + search.po[i] - 1;
        search.URtoUL[i + 1] = URtoUL_Move[search.URtoUL[i]][mv];
        search.UBtoDF[i + 1] = UBtoDF_Move[search.UBtoDF[i]][mv];
    }
    search.URtoDF[depthPhase1] = MergeURtoULandUBtoDF[search.URtoUL[depthPhase1]][search.UBtoDF[depthPhase1]];

    if ((d2 = get_pruning(Slice_URtoDF_Parity_Prun,
                          (N_SLICE2 * search.URtoDF[depthPhase1] + search.FRtoBR[depthPhase1]) * 2 + search.parity[depthPhase1])) > maxDepthPhase2)
        return -1;

    if ((search.minDistPhase2[depthPhase1] = std::max(d1, d2)) == 0) // already solved
        return depthPhase1;

    // now set up search

    depthPhase2 = 1;
    n = depthPhase1;
    search.po[depthPhase1] = 0;
    search.ax[depthPhase1] = 0;
    search.minDistPhase2[n + 1] = 1; // else failure for depthPhase2=1, n=0
    // +++++++++++++++++++ end initialization +++++++++++++++++++++++++++++++++
    do
    {
        do
        {
            if ((depthPhase1 + depthPhase2 - n > search.minDistPhase2[n + 1]) && !busy)
            {

                if (search.ax[n] == 0 || search.ax[n] == 3) // Initialize next move
                {
                    search.ax[++n] = 1;
                    search.po[n] = 2;
                }
                else
                {
                    search.ax[++n] = 0;
                    search.po[n] = 1;
                }
            }
            else if ((search.ax[n] == 0 || search.ax[n] == 3) ? (++search.po[n] > 3) : ((search.po[n] = search.po[n] + 2) > 3))
            {
                do
                { // increment axis
                    if (++search.ax[n] > 5)
                    {
                        if (n == depthPhase1)
                        {
                            if (depthPhase2 >= maxDepthPhase2)
                                return -1;
                            else
                            {
                                depthPhase2++;
                                search.ax[n] = 0;
                                search.po[n] = 1;
                                busy = false;
                                break;
                            }
                        }
                        else
                        {
                            n--;
                            busy = true;
                            break;
                        }
                    }
                    else
                    {
                        if (search.ax[n] == 0 || search.ax[n] == 3)
                            search.po[n] = 1;
                        else
                            search.po[n] = 2;
                        busy = false;
                    }
                } while (n != depthPhase1 && (search.ax[n - 1] == search.ax[n] || search.ax[n - 1] - 3 == search.ax[n]));
            }
            else
                busy = false;
        } while (busy);
        // +++++++++++++ compute new coordinates and new minDist ++++++++++
        mv = 3 * search.ax[n] + search.po[n] - 1;

        search.URFtoDLF[n + 1] = URFtoDLF_Move[search.URFtoDLF[n]][mv];
        search.FRtoBR[n + 1] = FRtoBR_Move[search.FRtoBR[n]][mv];
        search.parity[n + 1] = parityMove[search.parity[n]][mv];
        search.URtoDF[n + 1] = URtoDF_Move[search.URtoDF[n]][mv];

        search.minDistPhase2[n + 1] = std::max(get_pruning(Slice_URtoDF_Parity_Prun, (N_SLICE2 * search.URtoDF[n + 1] + search.FRtoBR[n + 1]) * 2 + search.parity[n + 1]),
                                               get_pruning(Slice_URFtoDLF_Parity_Prun, (N_SLICE2 * search.URFtoDLF[n + 1] + search.FRtoBR[n + 1]) * 2 + search.parity[n + 1]));
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    } while (search.minDistPhase2[n + 1] != 0);

    return depthPhase1 + depthPhase2;
}

std::string kociemba(const std::string &cubeString, int maxDepth, int timeOut, const std::filesystem::path &cache_dir)
{
    if (cubeString.size() != 54)
    {
        std::cerr << "Error: wrong input, maybe some blocks are missing or surplus\n";
        return "";
    }
    else if (cubeString == "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB")
    {
        return "";
    }
    else if (cubeString[4] != 'U' || cubeString[13] != 'R' || cubeString[22] != 'F' || cubeString[31] != 'D' || cubeString[40] != 'L' || cubeString[49] != 'B')
    {
        std::cerr << "Error: wrong input, please check the center block\n";
        return "";
    }

    Search search;
    CubeFace cf;
    CubeState cs;
    CoordCube cc;

    int s;
    int mv = 0, n = 0;
    bool busy = false;
    int depthPhase1 = 1;
    // +++++++++++++++++++++check for wrong input +++++++++++++++++++++++++++++

    if (!PRUNING_INITED)
    {
        init_pruning(cache_dir);
    }

    cf = string_to_cubeface(cubeString);
    cs = to_CubeState(cf);
    if ((s = verify(cs)) != 0)
    {
        std::cerr << "Error: wrong input\n";
        return "";
    }

    // +++++++++++++++++++++++ initialization +++++++++++++++++++++++++++++++++
    cc = get_CoordCube(cs);

    search.po[0] = 0;
    search.ax[0] = 0;
    search.flip[0] = cc.flip;
    search.twist[0] = cc.twist;
    search.parity[0] = cc.parity;
    search.slice[0] = cc.FRtoBR / 24;
    search.URFtoDLF[0] = cc.URFtoDLF;
    search.FRtoBR[0] = cc.FRtoBR;
    search.URtoUL[0] = cc.URtoUL;
    search.UBtoDF[0] = cc.UBtoDF;

    search.minDistPhase1[1] = 1; // else failure for depth=1, n=0

    auto tStart = std::chrono::system_clock::now();

    // +++++++++++++++++++ Main loop ++++++++++++++++++++++++++++++++++++++++++
    do
    {
        do
        {
            if ((depthPhase1 - n > search.minDistPhase1[n + 1]) && !busy)
            {

                if (search.ax[n] == 0 || search.ax[n] == 3) // Initialize next move
                    search.ax[++n] = 1;
                else
                    search.ax[++n] = 0;
                search.po[n] = 1;
            }
            else if (++search.po[n] > 3)
            {
                do
                { // increment axis
                    if (++search.ax[n] > 5)
                    {
                        if (n == 0)
                        {
                            if (depthPhase1 >= maxDepth)
                            {
                                std::cerr << "Error: maxDepth reached\n";
                                return "";
                            }
                            else
                            {
                                depthPhase1++;
                                search.ax[n] = 0;
                                search.po[n] = 1;
                                busy = false;
                                break;
                            }
                        }
                        else
                        {
                            n--;
                            busy = true;
                            break;
                        }
                    }
                    else
                    {
                        search.po[n] = 1;
                        busy = false;
                    }
                } while (n != 0 && (search.ax[n - 1] == search.ax[n] || search.ax[n - 1] - 3 == search.ax[n]));
            }
            else
                busy = false;

        } while (busy);

        // +++++++++++++ compute new coordinates and new minDistPhase1 ++++++++++
        // if minDistPhase1 =0, the H subgroup is reached
        mv = 3 * search.ax[n] + search.po[n] - 1;
        search.flip[n + 1] = flipMove[search.flip[n]][mv];
        search.twist[n + 1] = twistMove[search.twist[n]][mv];
        search.slice[n + 1] = FRtoBR_Move[search.slice[n] * 24][mv] / 24;
        search.minDistPhase1[n + 1] = std::max(
            get_pruning(Slice_Flip_Prun, N_SLICE1 * search.flip[n + 1] + search.slice[n + 1]),
            get_pruning(Slice_Twist_Prun, N_SLICE1 * search.twist[n + 1] + search.slice[n + 1]));
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if (search.minDistPhase1[n + 1] == 0 && n >= depthPhase1 - 5)
        {
            search.minDistPhase1[n + 1] = 10; // instead of 10 any value >5 is possible
            if (n == depthPhase1 - 1 && (s = total_depth(search, depthPhase1, maxDepth)) >= 0)
            {
                if (s == depthPhase1 || (search.ax[depthPhase1 - 1] != search.ax[depthPhase1] && search.ax[depthPhase1 - 1] != search.ax[depthPhase1] + 3))
                {
                    return solution_to_string(search, s);
                }
            }
        }

        // time out
        if ((std::chrono::system_clock::now() - tStart) > std::chrono::milliseconds(timeOut))
        {
            std::cerr << "Error: time out\n";
            return "";
        }
    } while (1);
}
