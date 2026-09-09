#include <opencv2/opencv.hpp>

class Image
{
public:
	Image(const cv::Mat& image);

	// BLUR
	cv::Mat gaussianBlur(int KernelSize);
	cv::Mat medianBlur(int KernelSize);
	cv::Mat boxBlur(int KernelSize);

private:
	cv::Mat m_image;
};