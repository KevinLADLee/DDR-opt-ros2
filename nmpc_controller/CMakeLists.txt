cmake_minimum_required(VERSION 3.8)
project(nmpc_controller)

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

add_executable(nmpc
  src/mpc_node.cpp
  src/mpc.cpp
  src/mpc_wrapper.cpp
  externals/qpoases/SRC/Bounds.cpp
  externals/qpoases/SRC/Constraints.cpp
  externals/qpoases/SRC/CyclingManager.cpp
  externals/qpoases/SRC/Indexlist.cpp
  externals/qpoases/SRC/MessageHandling.cpp
  externals/qpoases/SRC/QProblem.cpp
  externals/qpoases/SRC/QProblemB.cpp
  externals/qpoases/SRC/SubjectTo.cpp
  externals/qpoases/SRC/Utils.cpp
  externals/qpoases/SRC/EXTRAS/SolutionAnalysis.cpp
  UAV_CAR_model/build/quadrotor_mpc_codegen/acado_qpoases_interface.cpp
  UAV_CAR_model/build/quadrotor_mpc_codegen/acado_integrator.c
  UAV_CAR_model/build/quadrotor_mpc_codegen/acado_solver.c
  UAV_CAR_model/build/quadrotor_mpc_codegen/acado_auxiliary_functions.c
)
target_compile_features(nmpc PRIVATE cxx_std_17)
target_compile_options(nmpc PRIVATE -O3 -Wall -g)
target_include_directories(nmpc PRIVATE
  include
  include/nmpc_controller
  ${PCL_INCLUDE_DIRS}
  "${DDR_OPT_OSQP_PREFIX}/include"
  "${DDR_OPT_OSQP_PREFIX}/include/osqp"
  UAV_CAR_model/build/quadrotor_mpc_codegen
  externals/qpoases
  externals/qpoases/INCLUDE
  externals/qpoases/SRC
)
ament_target_dependencies(nmpc
  rclcpp ddr_opt_ros2_compat carstatemsgs back_end geometry_msgs nav_msgs
  sensor_msgs std_msgs visualization_msgs tf2 tf2_geometry_msgs pcl_conversions
)
target_link_libraries(nmpc OsqpEigen::OsqpEigen Eigen3::Eigen ${PCL_LIBRARIES})

if(EXISTS "${DDR_OPT_OSQP_PREFIX}/lib")
  set_target_properties(nmpc PROPERTIES
    BUILD_RPATH "${DDR_OPT_OSQP_PREFIX}/lib"
    INSTALL_RPATH "${DDR_OPT_OSQP_PREFIX}/lib"
  )
endif()

install(TARGETS nmpc DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_package()
