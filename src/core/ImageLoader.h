#include <opencv2/opencv.hpp>

class ImageLoader
{
public:
	cv::Mat load(const std::string& filePath);
};