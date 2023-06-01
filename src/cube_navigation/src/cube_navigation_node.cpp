#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

struct Position {
    double x;
    double y;
    double z;
};

Position initialPosition;
Position finalPosition;
int positionCounter = 0;

void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
    if (positionCounter == 0) {
        initialPosition.x = msg->pose.pose.position.x;
        initialPosition.y = msg->pose.pose.position.y;
        initialPosition.z = msg->pose.pose.position.z;
        // Assuming initial velocity is provided in /odom topic as well
        double initialVx = msg->twist.twist.linear.x;
        double initialVy = msg->twist.twist.linear.y;
        double initialVz = msg->twist.twist.linear.z;

        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Initial Position: (%.2f, %.2f, %.2f)", initialPosition.x, initialPosition.y, initialPosition.z);
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Initial Velocity: (%.2f, %.2f, %.2f)", initialVx, initialVy, initialVz);
        positionCounter++;
    }
}

void centroidCallback(const geometry_msgs::msg::PointStamped::SharedPtr msg) {
    if (positionCounter == 1) {
        finalPosition.x = msg->point.x;
        finalPosition.y = msg->point.y;
        finalPosition.z = msg->point.z;

        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Final Position: (%.2f, %.2f, %.2f)", finalPosition.x, finalPosition.y, finalPosition.z);
        positionCounter++;
    }
}

geometry_msgs::msg::Point computeWaypoint(double t, double tf) {
    double x0 = initialPosition.x;
    double y0 = initialPosition.y;
    double z0 = initialPosition.z;
    double xf = finalPosition.x;
    double yf = finalPosition.y;
    double zf = finalPosition.z;
    double v0x = 0.0;  // Assuming final velocity is zero
    double v0y = 0.0;  // Assuming final velocity is zero
    double v0z = 0.0;  // Assuming final velocity is zero
    double afx = 0.0;  // Assuming zero acceleration except for z-coordinate
    double afy = 0.0;  // Assuming zero acceleration
    double afz = -9.8;  // Assuming zero acceleration except for z-coordinate
    double gx = 0.0;
    double gy = 0.0;
    double gz = -9.8;

    double a0x = (12.0 / (tf * tf)) * (xf - x0 - v0x * tf + (tf * tf * gx) / 2.0) - (6.0 / tf) * (0.0 - v0x + gx * tf) + afx;
    double a0y = (12.0 / (tf * tf)) * (yf - y0 - v0y * tf + (tf * tf * gy) / 2.0) - (6.0 / tf) * (0.0 - v0y + gy * tf) + afy;
    double a0z = (12.0 / (tf * tf)) * (zf - z0 - v0z * tf + (tf * tf * gz) / 2.0) - (6.0 / tf) * (0.0 - v0z + gz * tf) + afz;

    double a1x = (-48.0 / (tf * tf * tf)) * (xf - x0 - v0x * tf + (tf * tf * gx) / 2.0) + (30.0 / (tf * tf)) * (0.0 - v0x + gx * tf) - (6.0 / tf) * afx;
    double a1y = (-48.0 / (tf * tf * tf)) * (yf - y0 - v0y * tf + (tf * tf * gy) / 2.0) + (30.0 / (tf * tf)) * (0.0 - v0y + gy * tf) - (6.0 / tf) * afy;
    double a1z = (-48.0 / (tf * tf * tf)) * (zf - z0 - v0z * tf + (tf * tf * gz) / 2.0) + (30.0 / (tf * tf)) * (0.0 - v0z + gz * tf) - (6.0 / tf) * afz;

    double a2x = (36.0 / (tf * tf * tf * tf)) * (xf - x0 - v0x * tf + (tf * tf * gx) / 2.0) - (24.0 / (tf * tf * tf)) * (0.0 - v0x + gx * tf) + (6.0 / (tf * tf)) * afx;
    double a2y = (36.0 / (tf * tf * tf * tf)) * (yf - y0 - v0y * tf + (tf * tf * gy) / 2.0) - (24.0 / (tf * tf * tf)) * (0.0 - v0y + gy * tf) + (6.0 / (tf * tf)) * afy;
    double a2z = (36.0 / (tf * tf * tf * tf)) * (zf - z0 - v0z * tf + (tf * tf * gz) / 2.0) - (24.0 / (tf * tf * tf)) * (0.0 - v0z + gz * tf) + (6.0 / (tf * tf)) * afz;

    double positionX = x0 + v0x * t + (a0x - gx) * (t * t) / 2.0 + (a1x * t * t * t) / 6.0 + (a2x * t * t * t * t) / 12.0;
    double positionY = y0 + v0y * t + (a0y - gy) * (t * t) / 2.0 + (a1y * t * t * t) / 6.0 + (a2y * t * t * t * t) / 12.0;
    double positionZ = z0 + v0z * t + (a0z - gz) * (t * t) / 2.0 + (a1z * t * t * t) / 6.0 + (a2z * t * t * t * t) / 12.0;

    geometry_msgs::msg::Point waypoint;
    waypoint.x = positionX;
    waypoint.y = positionY;
    waypoint.z = positionZ;

    return waypoint;
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("waypoint_computation");

    auto odomSubscriber = node->create_subscription<nav_msgs::msg::Odometry>(
        "/odom", 10, odomCallback);

    auto centroidSubscriber = node->create_subscription<geometry_msgs::msg::PointStamped>(
        "/centroid_world", 10, centroidCallback);

    double computationRate = 1.0;  // 1 Hz
    rclcpp::Rate loopRate(computationRate);

    rclcpp::Publisher<geometry_msgs::msg::Point>::SharedPtr publisher = node->create_publisher<geometry_msgs::msg::Point>("/waypoints", 10);

    double incrementTime = 1.0;  // Increment time in seconds
    double tf = 600.0;  // Total time frame in seconds
    double t = 0.0;
    while (rclcpp::ok() && positionCounter < 2) {
        rclcpp::spin_some(node);
        loopRate.sleep();
    }

    while (rclcpp::ok() && t <= tf) {
        geometry_msgs::msg::Point waypoint = computeWaypoint(t, tf);
        publisher->publish(waypoint);

        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Published Waypoint: (%.2f, %.2f, %.2f)", waypoint.x, waypoint.y, waypoint.z);

        t += incrementTime;

        rclcpp::spin_some(node);
        loopRate.sleep();
    }

    rclcpp::shutdown();
    return 0;
}
