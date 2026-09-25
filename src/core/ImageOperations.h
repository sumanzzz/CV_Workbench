#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

enum class BlurType
{
	Gaussian,
	Median,
	Box
};

enum class ThresholdType
{
	Binary,
	BinaryInverse,
	Trunc,
	ToZero,
	ToZeroInverse
};

class Image
{
public:
	Image(const cv::Mat& image);

	const cv::Mat& getImage() const;

	// BLUR 
	cv::Mat applyBlur(BlurType type, int kernelSize);
	cv::Mat applyThreshold(ThresholdType type, double thresholdValue, double maxValue = 255.0);
	cv::Mat applyBlurToRegion(
		const cv::Rect& region,
		BlurType type,
		int kernelSize,
		const cv::Mat& existingProcessedImage
	);

	cv::Mat gaussianBlur(int kernelSize);
	cv::Mat medianBlur(int kernelSize);
	cv::Mat boxBlur(int kernelSize);

	std::vector<cv::KeyPoint> detectKeypoints();
	std::vector<cv::KeyPoint> detectKeypoints(const cv::Mat& image) const;
	cv::Mat drawKeyPoints(const std::vector<cv::KeyPoint>& keyPoints);
	cv::Mat drawKeyPoints(const cv::Mat& image, const std::vector<cv::KeyPoint>& keyPoints) const;
	cv::Rect keyPointRegion(const cv::KeyPoint& keyPoint) const;

private:
	cv::Mat m_image;
};
