cmake_minimum_required(VERSION 3.8)
project(back_end)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ddr_opt_ros2_compat REQUIRED)
find_package(front_end REQUIRED)
find_package(plan_env REQUIRED)
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

add_library(back_end src/optimizer.cpp)
target_compile_features(back_end PUBLIC cxx_std_17)
target_compile_options(back_end PRIVATE -O3 -Wall -fPIC)
target_include_directories(back_end PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${PCL_INCLUDE_DIRS}
)
ament_target_dependencies(back_end
  rclcpp front_end plan_env geometry_msgs nav_msgs sensor_msgs std_msgs
  visualization_msgs tf2 tf2_geometry_msgs pcl_conversions ddr_opt_ros2_compat
)
target_link_libraries(back_end Eigen3::Eigen ${PCL_LIBRARIES})

install(TARGETS back_end
  EXPORT export_back_end
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_export_targets(export_back_end HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rclcpp front_end plan_env geometry_msgs nav_msgs sensor_msgs std_msgs
  visualization_msgs tf2 tf2_geometry_msgs pcl_conversions ddr_opt_ros2_compat Eigen3
)
ament_package()
