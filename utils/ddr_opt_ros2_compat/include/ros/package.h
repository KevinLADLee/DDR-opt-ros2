#ifndef DDR_OPT_ROS2_COMPAT_ROS_PACKAGE_H_
#define DDR_OPT_ROS2_COMPAT_ROS_PACKAGE_H_

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <string>

namespace ros
{
namespace package
{
inline std::string getPath(const std::string & package_name)
{
  return ament_index_cpp::get_package_share_directory(package_name);
}
}  // namespace package
}  // namespace ros

#endif  // DDR_OPT_ROS2_COMPAT_ROS_PACKAGE_H_
