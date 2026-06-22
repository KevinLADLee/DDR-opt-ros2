#ifndef DDR_OPT_ROS2_COMPAT_TF_TRANSFORM_DATATYPES_H_
#define DDR_OPT_ROS2_COMPAT_TF_TRANSFORM_DATATYPES_H_

#include <geometry_msgs/msg/quaternion.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2/utils.hpp>

namespace tf
{
using Quaternion = tf2::Quaternion;

inline double getYaw(const geometry_msgs::msg::Quaternion & q)
{
  return tf2::getYaw(q);
}

inline geometry_msgs::msg::Quaternion createQuaternionMsgFromYaw(double yaw)
{
  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, yaw);
  return tf2::toMsg(q);
}

inline Quaternion createQuaternionFromYaw(double yaw)
{
  Quaternion q;
  q.setRPY(0.0, 0.0, yaw);
  return q;
}

inline Quaternion createQuaternionFromRPY(double roll, double pitch, double yaw)
{
  Quaternion q;
  q.setRPY(roll, pitch, yaw);
  return q;
}
}  // namespace tf

#endif  // DDR_OPT_ROS2_COMPAT_TF_TRANSFORM_DATATYPES_H_
