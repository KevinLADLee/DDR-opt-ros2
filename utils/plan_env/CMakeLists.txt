cmake_minimum_required(VERSION 3.8)
project(plan_env)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ddr_opt_ros2_compat REQUIRED)
find_package(carstatemsgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_sensor_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

add_library(plan_env
  src/sdf_map.cpp
  src/raycast.cpp
)
target_compile_features(plan_env PUBLIC cxx_std_17)
target_compile_options(plan_env PRIVATE -O3 -Wall -fPIC)
target_include_directories(plan_env PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${PCL_INCLUDE_DIRS}
)
ament_target_dependencies(plan_env
  rclcpp carstatemsgs geometry_msgs sensor_msgs std_msgs visualization_msgs
  tf2 tf2_ros tf2_sensor_msgs tf2_geometry_msgs pcl_conversions ddr_opt_ros2_compat
)
target_link_libraries(plan_env Eigen3::Eigen ${PCL_LIBRARIES})

add_executable(exam_node src/sdf_map_exam_node.cpp)
target_compile_features(exam_node PRIVATE cxx_std_17)
ament_target_dependencies(exam_node rclcpp ddr_opt_ros2_compat)
target_link_libraries(exam_node plan_env)

install(TARGETS plan_env
  EXPORT export_plan_env
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(TARGETS exam_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_export_targets(export_plan_env HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rclcpp carstatemsgs geometry_msgs sensor_msgs std_msgs visualization_msgs
  tf2 tf2_ros tf2_sensor_msgs tf2_geometry_msgs pcl_conversions ddr_opt_ros2_compat Eigen3
)
ament_package()
