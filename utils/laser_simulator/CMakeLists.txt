cmake_minimum_required(VERSION 3.8)
project(laser_simulator)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ddr_opt_ros2_compat REQUIRED)
find_package(carstatemsgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL REQUIRED)

add_executable(laser_sim_node src/laser_sim_node.cpp)
target_compile_features(laser_sim_node PRIVATE cxx_std_17)
target_compile_options(laser_sim_node PRIVATE -O3 -Wall)
target_include_directories(laser_sim_node PRIVATE include ${PCL_INCLUDE_DIRS})
ament_target_dependencies(laser_sim_node
  rclcpp ddr_opt_ros2_compat carstatemsgs geometry_msgs nav_msgs sensor_msgs
  std_msgs visualization_msgs tf2 tf2_ros tf2_geometry_msgs pcl_conversions
)
target_link_libraries(laser_sim_node ${PCL_LIBRARIES})

add_executable(transmit_odom_node src/transmit_odom.cpp)
target_compile_features(transmit_odom_node PRIVATE cxx_std_17)
target_compile_options(transmit_odom_node PRIVATE -O3 -Wall)
ament_target_dependencies(transmit_odom_node
  rclcpp ddr_opt_ros2_compat carstatemsgs geometry_msgs nav_msgs sensor_msgs
  std_msgs visualization_msgs tf2 tf2_ros tf2_geometry_msgs pcl_conversions
)

install(TARGETS laser_sim_node transmit_odom_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_package()
