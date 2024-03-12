
# kociemba
This project implements the solution for a Rubik's Cube using C++ and OpenCV. It may be used in certain Rubik's Cube robots in the future. The algorithm used for solving the Rubik's Cube is the Kociemba two-phase algorithm. You can find more details about it on the official website: http://kociemba.org.

## Usage

[here](https://www.bilibili.com/video/BV1sS421P78z) is a short demonstration video. Once cmake is compiled, you can run the project.

```shell
mkdir build && cd build
cmake ..
make
./src/main 1  # The parameter is the camera number, which is 0 by default
```

After the camera recognizes the color stability, you can press the corresponding button to save the state of the Rubik's Cube, for example, press `U` to save the color of the upper layer, and press `F` to save the color of the previous part. When the colors of the six faces have been saved, press `Enter` to solve the Rubik's Cube.

The `cppkociemba` folder contains the program for the algorithm, which can be used independently. Here is an example:


```cpp
#include <iostream>
#include "kociemba.h"

int main() {

    std::string cube = "DRURUFBBDBDFRRLUFDRULLFUURLLDFUDLRFRLBUDLBDLBRFFDBBFUB";

    //kociemba(std::string cube, int maxDepth = 24, int timeOut = 1000, std::filesystem::path cachePath = "./cache)
    std::string solution = kociemba(cube, 24, 1000, "./cache");
    
    //output：R L2 F B U' F' D F' U B2 L' U2 B2 U D' B2 U2 L2 D' R2 D2 
    std::cout << solution << std::endl;

    return 0;
}
```

After the program is run, it will generate some file storage pruning lookup table (about 7 Mb) in the cache directory, and read the cache file directly to speed up the solution of the Rubik's cube next time.

## Cube string notation
The names of the facelet positions of the cube (letters stand for Up, Left, Front, Right, Back, and Down):
```
             |************|
             |*U1**U2**U3*|
             |************|
             |*U4**U5**U6*|
             |************|
             |*U7**U8**U9*|
             |************|
 ************|************|************|************
 *L1**L2**L3*|*F1**F2**F3*|*R1**R2**R3*|*B1**B2**B3*
 ************|************|************|************
 *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*
 ************|************|************|************
 *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*
 ************|************|************|************
             |************|
             |*D1**D2**D3*|
             |************|
             |*D4**D5**D6*|
             |************|
             |*D7**D8**D9*|
             |************|
```

A cube definition string "UBL..." means that in position U1 we have the U-color, in position U2 we have the
B-color, in position U3 we have the L color etc. according to the order `U1`, `U2`, `U3`, `U4`, `U5`, `U6`, `U7`, `U8`, `U9`, `R1`, `R2`,
`R3`, `R4`, `R5`, `R6`, `R7`, `R8`, `R9`, `F1`, `F2`, `F3`, `F4`, `F5`, `F6`, `F7`, `F8`, `F9`, `D1`, `D2`, `D3`, `D4`, `D5`, `D6`, `D7`, `D8`, `D9`, `L1`, `L2`, `L3`, `L4`,
`L5`, `L6`, `L7`, `L8`, `L9`, `B1`, `B2`, `B3`, `B4`, `B5`, `B6`, `B7`, `B8`, `B9`.

So, for example, a definition of a solved cube would be `UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB`

Solution string consists of space-separated parts, each of them represents a single move:
* A single letter by itself means to turn that face clockwise 90 degrees.
* A letter followed by an apostrophe means to turn that face counterclockwise 90 degrees.
* A letter with the number 2 after it means to turn that face 180 degrees.

e.g. **`R U R' U R U2 R' U`**

## Performance

No systematic tests have been done, but solving a common Rubik's cube takes about 10 milliseconds.

## Testing

## Reference library

Here are some of the libraries used in this project:

- [muodov/kociemba](https://github.com/muodov/kociemba/) - A pure Python and pure C ports of Kociemba's algorithm for solving Rubik's cube
- [nicpatel963/CubeSolvingScript](https://github.com/nicpatel963/CubeSolvingScript) - In this script I have implemented a code for detection of cube and solving it.(also GUI based solution)