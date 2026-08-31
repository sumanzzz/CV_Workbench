#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

class ImageLoader
{
public:
	cv::Mat load(const std::string& filePath);
};