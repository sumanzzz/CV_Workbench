#include "ImageLoader.h"

cv::Mat ImageLoader::load(const std::string& filePath)
{
	cv::Mat image = cv::imread(filePath);
	std::cout <<"Image Loaded :" << image.rows << "x" << image.cols << std::endl;

	if (image.empty())
	{
		std::cout << "Failed to Load Image" << std::endl;
		exit(0);
	}
	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
	return image;
}