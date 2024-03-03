#include <iostream>

#include "search.h"
#include "cube_recognize.h"

extern std::map<cube::Side, std::vector<cube::Color>> state;

std::map<cube::Color, char> sign_conv = {
    {cube::Color::green, 'F'},
    {cube::Color::blue, 'B'},
    {cube::Color::red, 'R'},
    {cube::Color::orange, 'L'},
    {cube::Color::yellow, 'D'},
    {cube::Color::white, 'U'},
};

std::vector<cv::Mat> load_images(const std::vector<std::string> &steps)
{
    std::vector<cv::Mat> images;
    for (const auto &step : steps)
    {
        std::string imagePath = "./imgs/" + step + ".jpg";
        cv::Mat img = cv::imread(imagePath);
        if (img.empty())
        {
            std::cout << "Could not read the image: " << imagePath << std::endl;
            continue;
        }
        images.push_back(img);
    }
    return images;
}

cv::Mat concatenate_images(const std::vector<cv::Mat> &images)
{
    std::vector<cv::Mat> rows;
    for (size_t i = 0; i < images.size(); i += 6)
    {
        std::vector<cv::Mat> rowImages(images.begin() + i, images.begin() + std::min(i + 6, images.size()));
        while (rowImages.size() < 6)
        {
            rowImages.push_back(cv::Mat::zeros(images[0].size(), images[0].type()));
        }
        cv::Mat row;
        cv::hconcat(rowImages, row);
        rows.push_back(row);
    }
    cv::Mat result;
    cv::vconcat(rows, result);
    return result;
}

int main()
{
    cv::VideoCapture cap(1);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Unable to open camera" << std::endl;
        return 1;
    }
    cv::Mat img, frame, preview{cv::Mat::zeros(700, 700, CV_8UC3)};
    int check_state{0};

    while (true)
    {
        std::vector<cube::Color> current_state(9);

        cap >> img;
        if (img.empty())
        {
            std::cerr << "Error: Unable to capture frame" << std::endl;
            break;
        }

        precess_frame(img, preview, current_state);

        cv::imshow("frame", img);
        cv::imshow("preview", preview);

        int k = cv::waitKey(1);
        if (k == 27)
        {
            break;
        }
        else if (k == 'u')
        {
            state[cube::Side::up] = current_state;
            check_state |= 0b1;
        }
        else if (k == 'r')
        {
            state[cube::Side::right] = current_state;
            check_state |= 0b1 << 1;
        }
        else if (k == 'l')
        {
            state[cube::Side::left] = current_state;
            check_state |= 0b1 << 2;
        }
        else if (k == 'd')
        {
            state[cube::Side::down] = current_state;
            check_state |= 0b1 << 3;
        }
        else if (k == 'f')
        {
            state[cube::Side::front] = current_state;
            check_state |= 0b1 << 4;
        }
        else if (k == 'b')
        {
            state[cube::Side::back] = current_state;
            check_state |= 0b1 << 5;
        }
        else if (k == '\r' && check_state == 0b111111)
        {
            std::string result;
            for (auto &side : state)
            {
                for (auto &color : side.second)
                {
                    result += sign_conv[color];
                }
            }
            auto solution = kociemba(result, 24, 1000, "./cache");
            if (solution != "")
            {
                std::vector<std::string> parsedSteps = parse_steps(solution);
                std::vector<cv::Mat> images = load_images(parsedSteps);
                cv::Mat result = concatenate_images(images);
                cv::imshow("Solution", result);
            }
            else
            {
                std::cout << "Unsolvable cube!" << std::endl;
            }
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
