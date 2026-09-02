#include "ImageOperations.h"

Image::Image(const cv::Mat& image) : m_image(image){}

cv::Mat Image::gaussianBlur(int kernelSize)
{
	cv::Mat gauss_blur_image;
	cv::GaussianBlur(m_image, gauss_blur_image, cv::Size(kernelSize, kernelSize), 0);
	return gauss_blur_image;
}