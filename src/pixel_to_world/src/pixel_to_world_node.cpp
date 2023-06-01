#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

class PixelToWorldConverter : public rclcpp::Node
{
public:
  PixelToWorldConverter() : Node("pixel_to_world_node")
  {
    // Subscribe to the "/centroids" topic
    centroid_subscriber_ = this->create_subscription<geometry_msgs::msg::Point>(
      "/centroids", 10, std::bind(&PixelToWorldConverter::centroidCallback, this, std::placeholders::_1));

    // Subscribe to the "/odom" topic
    odom_subscriber_ = this->create_subscription<nav_msgs::msg::Odometry>(
      "/odom", 10, std::bind(&PixelToWorldConverter::odomCallback, this, std::placeholders::_1));

    // Initialize camera intrinsic parameters
    // These values are specific to the Logitech C910 camera
    // Adjust them based on your camera's intrinsic parameters
    fx_ = 1400.0;  // focal length in the x-direction
    fy_ = 1400.0;  // focal length in the y-direction
    cx_ = 960.0;   // optical center x-coordinate
    cy_ = 540.0;   // optical center y-coordinate

    // Create the publisher for the converted point
    world_point_publisher_ = this->create_publisher<geometry_msgs::msg::PointStamped>(
      "/centroid_world", 10);
  }

private:
  void centroidCallback(const geometry_msgs::msg::Point::SharedPtr msg)
  {
    // Convert the centroid coordinates from the pixel frame to the world frame
    double u = msg->x;
    double v = msg->y;

    double x = (u - cx_) * depth_ / fx_;
    double y = (v - cy_) * depth_ / fy_;
    double z = 300;

    // Publish the converted centroid in the world frame
    auto world_point = std::make_shared<geometry_msgs::msg::PointStamped>();
    world_point->header.stamp = this->get_clock()->now();
    world_point->header.frame_id = "world";
    world_point->point.x = x;
    world_point->point.y = y;
    world_point->point.z = z;

    world_point_publisher_->publish(*world_point);
  }

  void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
  {
    // Get the depth of the camera (distance from camera to cube)
    depth_ = msg->pose.pose.position.z - 0.6; // Assuming the camera is 0.6 meters down the z-axis from the cube

    // Update the timestamp of the depth
    depth_timestamp_ = msg->header.stamp;
  }

  double fx_;  // focal length in the x-direction
  double fy_;  // focal length in the y-direction
  double cx_;  // optical center x-coordinate
  double cy_;  // optical center y-coordinate
  double depth_;  // depth of the camera (distance from camera to cube)
  rclcpp::Time depth_timestamp_;

  rclcpp::Subscription<geometry_msgs::msg::Point>::SharedPtr centroid_subscriber_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscriber_;
  rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr world_point_publisher_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PixelToWorldConverter>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
