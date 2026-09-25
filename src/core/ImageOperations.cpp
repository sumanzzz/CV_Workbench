#include "ImageOperations.h"

#include <cmath>
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

cv::Rect Image::keyPointRegion(const cv::KeyPoint& keyPoint) const
{
	if (m_image.empty() || keyPoint.size <= 0.0f)
	{
		return {};
	}

	const double halfSize = static_cast<double>(keyPoint.size) / 2.0;
	const int left = static_cast<int>(std::floor(keyPoint.pt.x - halfSize));
	const int top = static_cast<int>(std::floor(keyPoint.pt.y - halfSize));
	const int right = static_cast<int>(std::ceil(keyPoint.pt.x + halfSize));
	const int bottom = static_cast<int>(std::ceil(keyPoint.pt.y + halfSize));
	const cv::Rect keyPointBounds(left, top, right - left, bottom - top);
	const cv::Rect imageBounds(0, 0, m_image.cols, m_image.rows);

	return keyPointBounds & imageBounds;
}
