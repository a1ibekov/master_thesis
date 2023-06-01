#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

class AltitudePublisher : public rclcpp::Node
{
public:
  AltitudePublisher() : Node("altitude_publisher_node")
  {
    m_publisher = this->create_publisher<std_msgs::msg::Float32>("altitude", 10);

    auto timer_callback = [this]() -> void {
      std_msgs::msg::Float32 message;
      message.data = m_altitude;
      m_publisher->publish(message);

      if (m_altitude == 100) {
        m_altitude = 0;
      } else {
        m_altitude += 10;
      }
    };

    m_timer = this->create_wall_timer(std::chrono::seconds(2), timer_callback);
  }

private:
  rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr m_publisher;
  rclcpp::TimerBase::SharedPtr m_timer;
  int m_altitude = 0;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<AltitudePublisher>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
