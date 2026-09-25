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

cv::Mat Image::applyBlurToRegion(
	const cv::Rect& region,
	BlurType type,
	int kernelSize,
	const cv::Mat& existingProcessedImage
)
{
	if (m_image.empty())
	{
		return {};
	}

	cv::Mat result = m_image.clone();
	if (!existingProcessedImage.empty() &&
		existingProcessedImage.size() == m_image.size() &&
		existingProcessedImage.type() == m_image.type())
	{
		result = existingProcessedImage.clone();
	}
	if (kernelSize <= 0)
	{
		return result;
	}

	const cv::Rect imageBounds(0, 0, m_image.cols, m_image.rows);
	const cv::Rect clippedRegion = region & imageBounds;
	if (clippedRegion.empty())
	{
		return result;
	}

	const cv::Mat blurredImage = applyBlur(type, kernelSize);
	if (blurredImage.empty())
	{
		return result;
	}

	blurredImage(clippedRegion).copyTo(result(clippedRegion));
	return result;
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
	return detectKeypoints(m_image);
}

std::vector<cv::KeyPoint> Image::detectKeypoints(const cv::Mat& image) const
{
	cv::Ptr<cv::ORB> orb = cv::ORB::create();

	std::vector<cv::KeyPoint> keyPoints;

	if (!image.empty())
	{
		orb->detect(image, keyPoints);
	}

	return keyPoints;
}

cv::Mat Image::drawKeyPoints(const std::vector<cv::KeyPoint>& keyPoints)
{
	return drawKeyPoints(m_image, keyPoints);
}

cv::Mat Image::drawKeyPoints(
	const cv::Mat& image,
	const std::vector<cv::KeyPoint>& keyPoints
) const
{
	cv::Mat result;
	cv::drawKeypoints(image, keyPoints, result);
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
