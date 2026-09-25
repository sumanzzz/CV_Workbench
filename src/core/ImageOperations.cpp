#include "ImageOperations.h"
/*
	BOX BLUR : SIMPLE AVERAGE 
		Takes average of all pixels
		Fast , Simple , Blurs edges quite heavily
		Not great for preserving details

	GAUSSIAN BLUR : WEIGHTED AVERGAGE
		
*/
Image::Image(const cv::Mat& image) : m_image(image){}

const cv::Mat& Image::getImage()const
{
	return m_image;
}
cv::Mat Image::applyBlur(BlurType type, int kernelSize)
{
	switch(type)
	{
	case BlurType::Gaussian:
	{
		return gaussianBlur(kernelSize);
	}
	case BlurType::Median:
	{
		return medianBlur(kernelSize);
	}
	case BlurType::Box:
	{
		return boxBlur(kernelSize);
	}

	}
}
cv::Mat Image::gaussianBlur(int kernelSize)
{
	cv::Mat gauss_blur_image;
	cv::GaussianBlur(m_image, gauss_blur_image, cv::Size(kernelSize, kernelSize), 0);
	return gauss_blur_image;
}
cv::Mat Image::medianBlur(int kernelSize)
{
	cv::Mat median_blur_image;
	cv::medianBlur(m_image, median_blur_image, kernelSize);
	return median_blur_image;
}
cv::Mat Image::boxBlur(int kernelSize)
{
	cv::Mat box_blur_image;
	cv::blur(m_image, box_blur_image, cv::Size(kernelSize, kernelSize));
	return box_blur_image;
}

std::vector<cv::KeyPoint> Image::detectKeypoints()
{
	cv::Ptr<cv::ORB> orb = cv::ORB::create();

	std::vector<cv::KeyPoint> keyPoints;

	orb->detect(m_image, keyPoints);

	return keyPoints;
}

cv::Mat Image::drawKeyPoints(const std::vector<cv::KeyPoint>& keyPoints)
{
	cv::Mat result;
	
	cv::drawKeypoints(m_image, keyPoints, result);

	return result;
}
