add_library(cppkociemba)

target_sources(cppkociemba
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/cache_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/coordinatecube.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cubeface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cubestate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/kociemba.cpp
)

target_include_directories(cppkociemba
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)