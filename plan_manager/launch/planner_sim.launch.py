from pathlib import Path

import yaml
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def share(package_name):
    return Path(get_package_share_directory(package_name))


def flatten(prefix, value, output):
    if isinstance(value, dict):
        for key, item in value.items():
            flatten(f"{prefix}.{key}" if prefix else str(key), item, output)
    else:
        output[prefix] = value
    return output


def load_params(path):
    with open(path, "r", encoding="utf-8") as stream:
        data = yaml.safe_load(stream) or {}
    return flatten("", data, {})


def generate_launch_description():
    start_x = -7.75
    start_y = -0.5
    start_yaw = 0.0
    final_x = 3.7
    final_y = 0.3
    final_yaw = 3.14
    gridmap_interval = 0.1
    detection_range = 27.0
    hrz_limited = False
    hrz_laser_range_dgr = 90.0
    icr_yr = -0.3
    icr_yl = 0.3
    icr_xv = 0.2

    front_end_share = share("front_end")
    plan_manager_share = share("plan_manager")
    back_end_share = share("back_end")
    plan_env_share = share("plan_env")
    mpc_share = share("mpc_controller")
    laser_share = share("laser_simulator")

    car_params = load_params(plan_manager_share / "config" / "car3ms.yaml")
    planner_params = {}
    planner_params.update(load_params(front_end_share / "config" / "jps3ms.yaml"))
    planner_params.update(car_params)
    planner_params.update(load_params(back_end_share / "config" / "global_planning3ms.yaml"))
    planner_params.update(load_params(plan_env_share / "config" / "mapsim.yaml"))
    planner_params.update({
        "start_x": start_x,
        "start_y": start_y,
        "start_yaw": start_yaw,
        "if_fix_final": False,
        "final_x": final_x,
        "final_y": final_y,
        "final_yaw": final_yaw,
        "replan_time": 5000.0,
        "max_replan_time": 0.05,
        "gridmap_interval": gridmap_interval,
        "path_gridmap_interval": 0.05,
        "detection_range": detection_range,
        "global_x_lower": -10.0,
        "global_x_upper": 10.0,
        "global_y_lower": -10.0,
        "global_y_upper": 10.0,
        "if_perspective": True,
        "if_cirSupRaycast": False,
        "hrz_limited": hrz_limited,
        "hrz_laser_range_dgr": hrz_laser_range_dgr,
        "ICR_yr": icr_yr,
        "ICR_yl": icr_yl,
        "ICR_xv": icr_xv,
        "if_standard_diff": True,
    })

    simulator_params = dict(car_params)
    simulator_params.update({
        "start_x": start_x,
        "start_y": start_y,
        "start_yaw": start_yaw,
        "length": 0.62,
        "width": 0.58,
        "height": 0.25,
        "wheel_base": 0.45,
        "tread": 0.49,
        "front_suspension": 0.085,
        "rear_suspension": 0.085,
        "if_add_noise": True,
        "noise_stddev": 0.01,
        "State_Propa_rate": 500,
        "Pose_pub_rate": 100,
        "Pose_odom_pub_rate": 10,
        "detection_range": detection_range,
        "hrz_limited": hrz_limited,
        "hrz_laser_range_dgr": hrz_laser_range_dgr,
        "ICR_yr": icr_yr,
        "ICR_yl": icr_yl,
        "ICR_xv": icr_xv,
        "if_standard_diff": True,
    })

    mpc_params = {}
    mpc_params.update(load_params(mpc_share / "config" / "mpc3ms.yaml"))
    mpc_params.update(car_params)
    mpc_params["if_mpc"] = True

    map_params = load_params(plan_env_share / "config" / "obs_exam3.yaml")
    map_params.update({
        "Map_input_method": 2,
        "if_boundary_wall": True,
        "gridmap_interval": gridmap_interval,
        "laser_gridmap_interval": gridmap_interval,
        "start_x": start_x,
        "start_y": start_y,
        "start_yaw": start_yaw,
        "srand_num": -1,
        "Random.map_range.x_min": -10,
        "Random.map_range.x_max": 10,
        "Random.map_range.y_min": -10,
        "Random.map_range.y_max": 10,
        "Random.obstacle_box.num": 100,
        "Random.obstacle_box.length": 1.0,
        "Random.obstacle_box.safe_dis": 3.0,
        "Png.file_path": str(plan_env_share / "config" / "obs.png"),
        "Png.x_lower": -5.0,
        "Png.y_lower": -5.0,
    })

    laser_params = load_params(laser_share / "config" / "perspective_laser.yaml")
    laser_params.update({
        "if_perspective": True,
        "laser_pcd_topic": "/laser_simulator/local_pointcloud",
        "pc_resolution": gridmap_interval,
        "sensing_horizon": detection_range,
    })

    icrekf_params = {
        "Pose_sub_Reduce_frequency_": 0,
        "state_pub_frequency": 100.0,
        "Q_x": 0.5,
        "Q_y": 0.5,
        "Q_psi": 1.14,
        "Q_yr": 0.1,
        "Q_yl": 0.1,
        "Q_xv": 0.1,
        "R_x": 0.1,
        "R_y": 0.1,
        "R_psi": 0.1,
        "init_x_yr": -0.25,
        "init_x_yl": 0.25,
        "init_x_xv": 0.1,
        "yr_standard": icr_yr,
        "yl_standard": icr_yl,
        "xv_standard": icr_xv,
    }

    return LaunchDescription([
        DeclareLaunchArgument("use_rviz", default_value="true"),
        Node(
            package="plan_manager",
            executable="global_planning",
            name="global_planning",
            output="screen",
            parameters=[planner_params],
            remappings=[
                ("local_pointcloud", "/laser_simulator/local_pointcloud"),
                ("odom", "/ICREKF/EKF_XYTheta"),
                ("traj", "/planner/traj_poly"),
            ],
        ),
        Node(
            package="simulator",
            executable="simulator",
            name="simulator",
            output="screen",
            parameters=[simulator_params],
        ),
        Node(
            package="mpc_controller",
            executable="mpc",
            name="mpc",
            output="screen",
            parameters=[mpc_params],
            remappings=[
                ("cmd", "/simulation/PoseSub"),
                ("odom", "/ICREKF/EKF_XYTheta"),
                ("traj", "/planner/traj_poly"),
            ],
        ),
        Node(
            package="simulator",
            executable="global_map_node",
            name="global_map_node",
            output="screen",
            parameters=[map_params],
        ),
        Node(
            package="laser_simulator",
            executable="laser_sim_node",
            name="laser_simulator",
            output="screen",
            parameters=[laser_params],
        ),
        Node(
            package="icrekf",
            executable="icrekf",
            name="icrekf",
            output="screen",
            parameters=[icrekf_params],
            remappings=[
                ("ref_pose", "/simulation/SimulatedCarState"),
                ("odom", "/simulation/PoseOdomPub"),
                ("control", "/simulation/ControlPub"),
                ("EKF_XYTheta", "/ICREKF/EKF_XYTheta"),
                ("EKF_ICR", "/ICREKF/EKF_ICR"),
                ("Simple_EKF_ICR", "/ICREKF/Simple_EKF_ICR"),
            ],
        ),
        Node(
            package="rviz2",
            executable="rviz2",
            name="rviz2",
            arguments=["-d", str(plan_manager_share / "rviz" / "planner_sim.rviz")],
            condition=IfCondition(LaunchConfiguration("use_rviz")),
        ),
    ])
