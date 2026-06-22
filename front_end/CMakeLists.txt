cmake_minimum_required(VERSION 3.8)
project(front_end)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ddr_opt_ros2_compat REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(plan_env REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

add_library(front_end
  src/jps_planner/graph_search.cpp
  src/jps_planner/jps_planner.cpp
)
target_compile_features(front_end PUBLIC cxx_std_17)
target_compile_options(front_end PRIVATE -O3 -Wall -fPIC)
target_include_directories(front_end PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${PCL_INCLUDE_DIRS}
)
ament_target_dependencies(front_end
  rclcpp cv_bridge geometry_msgs nav_msgs sensor_msgs std_msgs
  visualization_msgs tf2 tf2_geometry_msgs plan_env ddr_opt_ros2_compat
)
target_link_libraries(front_end Eigen3::Eigen ${PCL_LIBRARIES})

install(TARGETS front_end
  EXPORT export_front_end
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_export_targets(export_front_end HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rclcpp cv_bridge geometry_msgs nav_msgs sensor_msgs std_msgs
  visualization_msgs tf2 tf2_geometry_msgs plan_env ddr_opt_ros2_compat Eigen3
)
ament_package()
