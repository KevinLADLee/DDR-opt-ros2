cmake_minimum_required(VERSION 3.8)
project(ddr_opt_ros2_compat)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(carstatemsgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME} INTERFACE
  rclcpp ament_index_cpp builtin_interfaces std_msgs geometry_msgs nav_msgs
  sensor_msgs visualization_msgs carstatemsgs tf2 tf2_geometry_msgs
)

install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

ament_export_targets(export_${PROJECT_NAME})
ament_export_dependencies(
  rclcpp ament_index_cpp builtin_interfaces std_msgs geometry_msgs nav_msgs
  sensor_msgs visualization_msgs carstatemsgs tf2 tf2_geometry_msgs
)
ament_package()
