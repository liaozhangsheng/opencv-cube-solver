#include <iostream>
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>

#include "search.h"
#include "cube.h"

using namespace cube;

extern std::map<cube::Side, std::vector<cube::Color>> state;

std::map<cube::Color, char> sign_conv = {
    {cube::Color::green, 'F'},
    {cube::Color::blue, 'B'},
    {cube::Color::red, 'R'},
    {cube::Color::orange, 'L'},
    {cube::Color::yellow, 'D'},
    {cube::Color::white, 'U'},
};

std::map<cube::Side, std::vector<cube::Color>> state = {
    {up, {white, white, white, white, white, white, white, white, white}},
    {right, {white, white, white, white, white, white, white, white, white}},
    {front, {white, white, white, white, white, white, white, white, white}},
    {down, {white, white, white, white, white, white, white, white, white}},
    {left, {white, white, white, white, white, white, white, white, white}},
    {back, {white, white, white, white, white, white, white, white, white}}};

std::map<cube::Color, cv::Scalar> color = {
    {red, cv::Scalar(0, 0, 255)},
    {orange, cv::Scalar(0, 128, 255)},
    {blue, cv::Scalar(255, 0, 0)},
    {green, cv::Scalar(0, 255, 0)},
    {white, cv::Scalar(255, 255, 255)},
    {yellow, cv::Scalar(0, 255, 255)},
};

std::map<cube::Side, std::vector<cv::Point>> stickers = {
    {cube::Side::main, {{200, 120}, {300, 120}, {400, 120}, {200, 220}, {300, 220}, {400, 220}, {200, 320}, {300, 320}, {400, 320}}},
    {current, {{20, 20}, {54, 20}, {88, 20}, {20, 54}, {54, 54}, {88, 54}, {20, 88}, {54, 88}, {88, 88}}},
    {preview, {{20, 130}, {54, 130}, {88, 130}, {20, 164}, {54, 164}, {88, 164}, {20, 198}, {54, 198}, {88, 198}}},
    {left, {{50, 280}, {94, 280}, {138, 280}, {50, 324}, {94, 324}, {138, 324}, {50, 368}, {94, 368}, {138, 368}}},
    {front, {{188, 280}, {232, 280}, {276, 280}, {188, 324}, {232, 324}, {276, 324}, {188, 368}, {232, 368}, {276, 368}}},
    {right, {{326, 280}, {370, 280}, {414, 280}, {326, 324}, {370, 324}, {414, 324}, {326, 368}, {370, 368}, {414, 368}}},
    {up, {{188, 128}, {232, 128}, {276, 128}, {188, 172}, {232, 172}, {276, 172}, {188, 216}, {232, 216}, {276, 216}}},
    {down, {{188, 434}, {232, 434}, {276, 434}, {188, 478}, {232, 478}, {276, 478}, {188, 522}, {232, 522}, {276, 522}}},
    {back, {{464, 280}, {508, 280}, {552, 280}, {464, 324}, {508, 324}, {552, 324}, {464, 368}, {508, 368}, {552, 368}}},
};

std::map<cube::Side, std::pair<std::pair<char, cv::Point>, std::tuple<char, cv::Scalar, cv::Point>>> textPoints = {
    {up, {{'U', {242, 202}}, {'W', {255, 255, 255}, {260, 208}}}},
    {right, {{'R', {380, 354}}, {'R', {0, 0, 255}, {398, 360}}}},
    {front, {{'F', {242, 354}}, {'G', {0, 255, 0}, {260, 360}}}},
    {down, {{'D', {242, 508}}, {'Y', {0, 255, 255}, {260, 514}}}},
    {left, {{'L', {104, 354}}, {'O', {0, 165, 255}, {122, 360}}}},
    {back, {{'B', {518, 354}}, {'B', {255, 0, 0}, {536, 360}}}},
};

Color color_detect(cv::Vec3b hsv)
{
    int h = hsv[0], s = hsv[1], v = hsv[2];
    if (h > 0 && h < 20 && s > 64 && s < 255 && v > 0 && v < 255)
        return Color::orange;
    else if (h > 20 && h < 70 && s > 20 && s < 255 && v > 0 && v < 255)
        return Color::yellow;
    else if (h > 0 && h < 40 && s > 0 && s < 60 && v > 140 && v < 255)
        return Color::white;
    else if (h > 110 && h < 180 && s > 70 && s < 255 && v > 0 && v < 180)
        return Color::red;
    else if (h > 40 && h < 100 && s > 40 && s < 255 && v > 0 && v < 255)
        return Color::green;
    else if (h > 90 && h < 170 && s > 60 && s < 255 && v > 0 && v < 255)
        return Color::blue;
    else
        return Color::white;
}

void draw_stickers(cv::Mat &frame, std::map<Side, std::vector<cv::Point>> &stickers, Side name)
{
    for (auto &point : stickers[name])
    {
        cv::rectangle(frame, cv::Rect(point.x, point.y, 30, 30), cv::Scalar(255, 255, 255), 2);
    }
}

void draw_preview_stickers(cv::Mat &frame, std::map<Side, std::vector<cv::Point>> &stickers)
{
    auto stick = {Side::front, Side::back, Side::left, Side::right, Side::up, Side::down};
    for (auto &name : stick)
    {
        for (auto &point : stickers[name])
        {
            cv::rectangle(frame, cv::Rect(point.x, point.y, 40, 40), cv::Scalar(255, 255, 255), 2);
        }
    }
}

void text_on_preview_stickers(cv::Mat &frame)
{
    auto stick = {Side::front, Side::back, Side::left, Side::right, Side::up, Side::down};

    for (auto &face : stick)
    {
        for (int i = 0; i < 9; ++i)
        {
            cv::putText(frame, {textPoints[face].first.first}, textPoints[face].first.second, cv::FONT_HERSHEY_SIMPLEX, 1, {0, 0, 0}, 1, cv::LINE_AA);
            cv::putText(frame, {std::get<0>(textPoints[face].second)}, std::get<2>(textPoints[face].second), cv::FONT_HERSHEY_SIMPLEX, 0.5, std::get<1>(textPoints[face].second), 1, cv::LINE_AA);
        }
    }
}

void fill_stickers(cv::Mat &frame, std::map<Side, std::vector<cv::Point>> &stickers, std::map<Side, std::vector<Color>> &sides)
{
    for (auto &side : sides)
    {
        int num = 0;
        for (auto &point : stickers[side.first])
        {
            cv::rectangle(frame, cv::Rect(point.x, point.y, 40, 40), color[side.second[num]], -1);
            num++;
        }
    }
}

void precess_frame(cv::Mat &img, cv::Mat &preview, std::vector<Color> &current_state)
{
    std::vector<cv::Vec3b> hsv(9);
    cv::Mat frame;

    cv::cvtColor(img, frame, cv::COLOR_BGR2HSV);

    draw_stickers(img, stickers, Side::main);
    draw_stickers(img, stickers, Side::current);
    draw_preview_stickers(preview, stickers);
    fill_stickers(preview, stickers, state);
    text_on_preview_stickers(preview);

    for (int i = 0; i < 9; ++i)
    {
        cv::Vec3i sum = cv::Vec3i(0, 0, 0);
        int spacing = 6;
        for (int dx = spacing; dx < 30; dx += spacing)
        {
            for (int dy = spacing; dy < 30; dy += spacing)
            {
                sum += cv::Vec3i(frame.at<cv::Vec3b>(stickers[Side::main][i] + cv::Point{dx, dy}));
            }
        }
        hsv[i] = cv::Vec3b(sum / std::pow(((30 / spacing) - 1), 2));
    }

    int a = 0;
    for (const auto &point : stickers[Side::current])
    {
        Color color_name = color_detect(hsv[a]);
        cv::rectangle(img, cv::Rect(point.x, point.y, 30, 30), color[color_name], -1);
        current_state.at(a++) = color_name;
    }
}

std::vector<cv::Mat> load_images(const std::vector<std::string> &steps)
{
    std::vector<cv::Mat> images;
    for (const auto &step : steps)
    {
        std::string imagePath = "../imgs/" + step + ".jpg";
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

int main(int argc, char **argv)
{
    int cameraIndex = 0;
    if (argc > 1)
    {
        cameraIndex = std::stoi(argv[1]);
    }
    cv::VideoCapture cap(cameraIndex);
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
            std::string cubeString;
            for (auto &side : state)
            {
                for (auto &color : side.second)
                {
                    cubeString += sign_conv[color];
                }
            }

            auto start = std::chrono::high_resolution_clock::now();
            auto solution = kociemba(cubeString, 24, 1000, "./cache");
            auto end = std::chrono::high_resolution_clock::now();

            if (solution != "")
            {
                cv::destroyWindow("frame");
                cv::destroyWindow("preview");
                std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
                std::vector<std::string> parsedSteps = parse_steps(solution);
                std::vector<cv::Mat> images = load_images(parsedSteps);
                cv::Mat result = concatenate_images(images);
                cv::imshow("Solution", result);
                cv::waitKey(0);
                break;
            }
            else
            {
                std::cout << "Unsolvable cube! Please carefully check your rubik's cube" << std::endl;
            }
        }
    }

    cv::destroyAllWindows();
    return 0;
}
