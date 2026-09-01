#include "ImageOperations.h"

Image::Image(const cv::Mat& image) : m_image(image){}

void Image::blurring()
{
	cv::Mat blur_image = m_image;
	//cv::GaussianBlur(m_image, blur_image);
}