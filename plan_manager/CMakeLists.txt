cmake_minimum_required(VERSION 3.8)
project(plan_manager)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ddr_opt_ros2_compat REQUIRED)
find_package(carstatemsgs REQUIRED)
find_package(plan_env REQUIRED)
find_package(front_end REQUIRED)
find_package(back_end REQUIRED)
find_package(mpc_controller REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

add_executable(global_planning src/global_planning.cpp)
target_compile_features(global_planning PRIVATE cxx_std_17)
target_compile_options(global_planning PRIVATE -O3 -Wall -fPIC)
target_include_directories(global_planning PRIVATE include ${PCL_INCLUDE_DIRS})
ament_target_dependencies(global_planning
  rclcpp ddr_opt_ros2_compat carstatemsgs plan_env front_end back_end
  mpc_controller geometry_msgs nav_msgs sensor_msgs std_msgs visualization_msgs
  tf2 tf2_geometry_msgs
)
target_link_libraries(global_planning Eigen3::Eigen ${PCL_LIBRARIES})

install(TARGETS global_planning DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)
install(DIRECTORY rviz/ DESTINATION share/${PROJECT_NAME}/rviz)
install(DIRECTORY plotjuggler/ DESTINATION share/${PROJECT_NAME}/plotjuggler)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

ament_package()
