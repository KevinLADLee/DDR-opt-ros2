cmake_minimum_required(VERSION 3.8)
project(mpc_controller)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

get_filename_component(DDR_OPT_REPO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
include("${DDR_OPT_REPO_DIR}/cmake/UseBundledOsqp.cmake")

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ddr_opt_ros2_compat REQUIRED)
find_package(carstatemsgs REQUIRED)
find_package(back_end REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(OsqpEigen QUIET)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

if(OsqpEigen_FOUND)
  get_filename_component(DDR_OPT_OSQP_PREFIX "${OsqpEigen_DIR}/../.." ABSOLUTE)
else()
  ddr_opt_use_bundled_osqp(
    REPO_DIR "${DDR_OPT_REPO_DIR}"
    PREFIX_VAR DDR_OPT_OSQP_PREFIX
  )
endif()

add_executable(mpc src/mpc_node.cpp src/mpc.cpp)
target_compile_features(mpc PRIVATE cxx_std_17)
target_compile_options(mpc PRIVATE -O3 -Wall -g)
target_include_directories(mpc PRIVATE
  include
  ${PCL_INCLUDE_DIRS}
  "${DDR_OPT_OSQP_PREFIX}/include"
  "${DDR_OPT_OSQP_PREFIX}/include/osqp"
)
ament_target_dependencies(mpc
  rclcpp ddr_opt_ros2_compat carstatemsgs back_end geometry_msgs nav_msgs
  sensor_msgs std_msgs visualization_msgs tf2 tf2_geometry_msgs pcl_conversions
)
target_link_libraries(mpc OsqpEigen::OsqpEigen Eigen3::Eigen ${PCL_LIBRARIES})

if(EXISTS "${DDR_OPT_OSQP_PREFIX}/lib")
  set_target_properties(mpc PROPERTIES
    BUILD_RPATH "${DDR_OPT_OSQP_PREFIX}/lib"
    INSTALL_RPATH "${DDR_OPT_OSQP_PREFIX}/lib"
  )
endif()

install(TARGETS mpc DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_package()
