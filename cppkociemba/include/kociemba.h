#ifndef __SEARCH_H__
#define __SEARCH_H__

#include <iostream>
#include <vector>
#include <filesystem>

/**
 * Parses the solution string and returns a vector of individual moves.
 *
 * @param steps The solution string to parse.
 * @return A vector of strings representing individual moves.
 */
std::vector<std::string> parse_steps(const std::string &steps);

/**
 * Computes the solver string for a given cube.
 *
 * @param cubeString
 *          is the cube definition string, see {@link Facelet} for the format.
 *
 * @param maxDepth
 *          defines the maximal allowed maneuver length. For random cubes, a maxDepth of 21 usually will return a
 *          solution in less than 0.5 seconds. With a maxDepth of 20 it takes a few seconds on average to find a
 *          solution, but it may take much longer for specific cubes.
 *
 *@param timeOut
 *          defines the maximum computing time of the method in seconds. If it does not return with a solution, it returns with
 *          an error code.
 *
 * @param useSeparator
 *          determines if a " . " separates the phase1 and phase2 parts of the solver string like in F' R B R L2 F .
 *          U2 U D for example.<br>
 * @return The solution string.
 */
std::string kociemba(const std::string &cubeString, int maxDepth = 24, int timeOut = 1000, const std::filesystem::path &cache_dir  = "./cache");

#endif
