
# opencv
set(OpenCV_DIR "D:/CppThirdPartyLibrary/OpenCV")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main main.cpp)

target_link_libraries(main 
PRIVATE
    ${OpenCV_LIBS}
    cppkociemba
)