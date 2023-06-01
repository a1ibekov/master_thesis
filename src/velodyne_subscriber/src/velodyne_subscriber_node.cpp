#include "rclcpp/rclcpp.hpp"
#include "velodyne_msgs/msg/velodyne_scan.hpp"

void velodyneCallback(const velodyne_msgs::msg::VelodyneScan::SharedPtr msg)
{
    RCLCPP_INFO(rclcpp::get_logger("velodyne_subscriber"), "Received VelodyneScan message");
    RCLCPP_INFO(rclcpp::get_logger("velodyne_subscriber"), "Number of packets: %zu", msg->packets.size());
    RCLCPP_INFO(rclcpp::get_logger("velodyne_subscriber"), "1 packet: %zu", msg->packets[0]);

    


}

int main(int argc, char** argv)
{
    // Initialize the ROS 2 node
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("velodyne_subscriber");

    // Create a subscriber for the velodyne_packets topic
    auto subscription = node->create_subscription<velodyne_msgs::msg::VelodyneScan>(
        "velodyne_packets", 10, velodyneCallback);

    // Spin the node and process callbacks
    rclcpp::spin(node);

    // Clean up
    rclcpp::shutdown();

    return 0;
}
