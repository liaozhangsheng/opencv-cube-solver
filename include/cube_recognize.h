#ifndef __CUBE_RECOGNIZE_H__
#define __CUBE_RECOGNIZE_H__

#include <opencv2/opencv.hpp>

#include "cube.h"

void precess_frame(cv::Mat &img, cv::Mat &preview, std::vector<cube::Color> &current_state);

#endif