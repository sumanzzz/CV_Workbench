#include <opencv2/opencv.hpp>

class Image
{
public:
	Image(const cv::Mat& image);
	void blurring();

private:
	cv::Mat m_image;
};