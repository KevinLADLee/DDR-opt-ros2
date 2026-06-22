cmake_minimum_required(VERSION 3.8)
project(simulator)

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
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(simulator src/simulator.cpp)
target_compile_features(simulator PRIVATE cxx_std_17)
target_compile_options(simulator PRIVATE -O3 -Wall -fPIC)
target_include_directories(simulator PRIVATE include ${PCL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
ament_target_dependencies(simulator
  rclcpp ddr_opt_ros2_compat carstatemsgs geometry_msgs nav_msgs sensor_msgs
  std_msgs visualization_msgs tf2 tf2_geometry_msgs pcl_conversions
)
target_link_libraries(simulator Eigen3::Eigen ${PCL_LIBRARIES})

add_executable(global_map_node src/global_map.cpp)
target_compile_features(global_map_node PRIVATE cxx_std_17)
target_compile_options(global_map_node PRIVATE -O3 -Wall -fPIC)
target_include_directories(global_map_node PRIVATE include ${PCL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
ament_target_dependencies(global_map_node
  rclcpp ddr_opt_ros2_compat carstatemsgs geometry_msgs nav_msgs sensor_msgs
  std_msgs visualization_msgs tf2 tf2_geometry_msgs pcl_conversions
)
target_link_libraries(global_map_node Eigen3::Eigen ${PCL_LIBRARIES} ${OpenCV_LIBS})

install(TARGETS simulator global_map_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY urdf/ DESTINATION share/${PROJECT_NAME}/urdf)

ament_package()
