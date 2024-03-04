cmake_minimum_required(VERSION 3.10)

# set the project name
project(cppkociemba)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add compile-time flags
# add_compile_options(-Wall -Wextra -g)

set(CMAKE_BUILD_TYPE release)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# specify the directories which contain the source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# specify the directories which contain the header files
include_directories(include)

# specify the directories which contain the libraries
link_directories(lib)

# opencv
set(OpenCV_DIR "D:/CppThirdPartyLibrary/OpenCV")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# link the libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})