cmake_minimum_required(VERSION 3.8)
project(icrekf)

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
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(icrekf src/icrekf_node.cpp src/icrekf.cpp)
target_compile_features(icrekf PRIVATE cxx_std_17)
target_compile_options(icrekf PRIVATE -O3 -Wall -fPIC)
target_include_directories(icrekf PRIVATE include)
ament_target_dependencies(icrekf
  rclcpp carstatemsgs geometry_msgs nav_msgs std_msgs visualization_msgs
  tf2 tf2_geometry_msgs ddr_opt_ros2_compat
)
target_link_libraries(icrekf Eigen3::Eigen)

install(TARGETS icrekf DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

ament_package()
