#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

enum class BlurType
{
	Gaussian,
	Median,
	Box
};
class Image
{
public:
	Image(const cv::Mat& image);

	const cv::Mat& getImage() const;

	// BLUR 
	cv::Mat applyBlur(BlurType type, int kernelSize);

	cv::Mat gaussianBlur(int kernelSize);
	cv::Mat medianBlur(int kernelSize);
	cv::Mat boxBlur(int kernelSize);

	std::vector<cv::KeyPoint> detectKeypoints();
	cv::Mat drawKeyPoints(const std::vector<cv::KeyPoint>& keyPoints);

private:
	cv::Mat m_image;
};