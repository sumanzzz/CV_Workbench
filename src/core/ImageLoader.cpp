#include "ImageLoader.h"

cv::Mat ImageLoader::load(const std::string& filePath)
{
	return cv::imread(filePath);
}