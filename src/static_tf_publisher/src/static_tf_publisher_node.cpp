#include <rclcpp/rclcpp.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

class StaticTfPublisher : public rclcpp::Node
{
public:
  StaticTfPublisher()
  : Node("static_tf_publisher")
  {
    // Set up the static TF broadcaster
    tf_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

    // Create the transformation message
    geometry_msgs::msg::TransformStamped transform;
    transform.header.frame_id = "base_link";
    transform.child_frame_id = "lidar_link";

    // Set the translation values (adjust according to your robot and lidar positions)
    transform.transform.translation.x = 0.5;  // Example translation along the X-axis
    transform.transform.translation.y = 0.0;  // Example translation along the Y-axis
    transform.transform.translation.z = 0.2;  // Example translation along the Z-axis

    // Set the rotation values (adjust according to your robot and lidar orientations)
    tf2::Quaternion quaternion;
    quaternion.setRPY(0.0, 0.0, 0.0);  // Example: no rotation
    transform.transform.rotation.x = quaternion.x();
    transform.transform.rotation.y = quaternion.y();
    transform.transform.rotation.z = quaternion.z();
    transform.transform.rotation.w = quaternion.w();

    // Publish the static transformation
    tf_broadcaster_->sendTransform(transform);
  }

private:
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<StaticTfPublisher>());
  rclcpp::shutdown();
  return 0;
}
