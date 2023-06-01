#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/compressed_image.hpp"
#include "opencv2/opencv.hpp"

class MySubscriber : public rclcpp::Node
{
public:
  MySubscriber() : Node("feature_extraction_node")
  {
    // Create a subscription to the "my_topic" topic
    subscription_ = this->create_subscription<sensor_msgs::msg::CompressedImage>(
      "/camera/image/compressed", 10, std::bind(&MySubscriber::callback, this, std::placeholders::_1));
  }

private:
  void callback(const sensor_msgs::msg::CompressedImage::SharedPtr msg)
  {
    cv::Mat input_image = cv::imdecode(cv::Mat(msg->data), cv::IMREAD_COLOR);
    cv::Mat gray_image;
    cv::cvtColor(input_image, gray_image, cv::COLOR_BGR2GRAY);

    // Initialize ORB feature detector and extractor
    cv::Ptr<cv::ORB> orb_detector = cv::ORB::create();

    // Detect and extract features from input image
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    orb_detector->detectAndCompute(gray_image, cv::noArray(), keypoints, descriptors);

    // Draw detected keypoints on input image
    cv::drawKeypoints(input_image, keypoints, input_image, cv::Scalar::all(-1));

    // Show results
    cv::imshow("Detected Keypoints", input_image);
    cv::waitKey(1);
  } 

  rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr subscription_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MySubscriber>();
  rclcpp::spin(node);
  cv::destroyAllWindows();
  rclcpp::shutdown();
  return 0;
}
