#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/compressed_image.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <algorithm>

class MySubscriber : public rclcpp::Node
{
public:
  MySubscriber() : Node("safe_landing_node")
  {
    // Create a subscription to the "my_topic" topic
    subscription_ = this->create_subscription<sensor_msgs::msg::CompressedImage>(
      "/camera/image/compressed", 10, std::bind(&MySubscriber::callback, this, std::placeholders::_1));

    // Create a publisher for centroid points
    centroid_publisher_ = this->create_publisher<geometry_msgs::msg::Point>("/centroids", 10);
  }

private:

void callback(const sensor_msgs::msg::CompressedImage::SharedPtr msg)
{
  cv::Mat input_image = cv::imdecode(cv::Mat(msg->data), cv::IMREAD_COLOR);
  cv::Mat gray_image;
  cv::cvtColor(input_image, gray_image, cv::COLOR_BGR2GRAY);

  // Calculate the gradient and magnitude
  cv::Mat gradient_x, gradient_y;
  cv::Sobel(gray_image, gradient_x, CV_32F, 1, 0);
  cv::Sobel(gray_image, gradient_y, CV_32F, 0, 1);

  cv::Mat magnitude, direction;
  cv::cartToPolar(gradient_x, gradient_y, magnitude, direction);

  const float threshold = 5.0;

  // Find connected regions in the magnitude array
  cv::Mat labels;
  int num_regions = cv::connectedComponents(magnitude < threshold, labels);

  // Calculate the area and centroid of each region
  std::vector<int> region_areas(num_regions, 0);
  std::vector<cv::Point2d> centroids(num_regions, cv::Point2d(0, 0));
  for (int i = 0; i < labels.rows; i++) {
    for (int j = 0; j < labels.cols; j++) {
      int label = labels.at<int>(i, j);
      if (label > 0) {
        region_areas[label]++;
        centroids[label] += cv::Point2d(j, i);  // Accumulate x and y coordinates for centroid calculation
      }
    }
  }

  // Normalize centroids and collect top 5 regions
  std::vector<std::pair<int, double>> top_regions;
  for (int i = 1; i < num_regions; i++) {  // Start from 1 to skip background region (label 0)
    if (region_areas[i] > 0) {
      centroids[i] /= region_areas[i];  // Calculate centroid by dividing accumulated coordinates by area
      top_regions.push_back(std::make_pair(i, region_areas[i]));
    }
  }

  // Sort regions based on area in descending order
  std::sort(top_regions.begin(), top_regions.end(),
            [](const std::pair<int, double>& a, const std::pair<int, double>& b) { return a.second > b.second; });

  // Publish centroid of the region with the highest area
  if (!top_regions.empty()) {
    int region_label = top_regions[0].first;
    cv::Point2d centroid = centroids[region_label];

    // Publish centroid
    geometry_msgs::msg::Point centroid_msg;
    centroid_msg.x = centroid.x;
    centroid_msg.y = centroid.y;
    centroid_publisher_->publish(centroid_msg);

    // Find contours of the region
    cv::Mat mask = (labels == region_label);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Draw contours on the input image
    cv::Scalar color_centroid = cv::Scalar(0, 0, 255);
    cv::Scalar color_contour = cv::Scalar(0, 255, 255);
    cv::drawContours(input_image, contours, -1, color_contour, 2);

    // Draw circle on the input image
    cv::circle(input_image, cv::Point(centroid.x, centroid.y), 5, color_centroid, -1);
  }

  // Display the input image
  cv::imshow("Output", input_image);
  cv::waitKey(1);
}

  rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr subscription_;
  rclcpp::Publisher<geometry_msgs::msg::Point>::SharedPtr centroid_publisher_;
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
