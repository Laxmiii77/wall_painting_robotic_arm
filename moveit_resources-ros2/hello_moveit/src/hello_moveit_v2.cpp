/*********************************************************************
 * Wall Painting Demo using MoveIt
 * 
 * This program generates zigzag waypoints to paint a wall and executes
 * them using Cartesian path planning in MoveIt.
 *********************************************************************/

#include <moveit/move_group_interface/move_group_interface.hpp>
#include <moveit/planning_scene_interface/planning_scene_interface.hpp>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <geometry_msgs/msg/pose.hpp>
#include <vector>
#include <chrono>
#include <thread>
static const rclcpp::Logger LOGGER = rclcpp::get_logger("wall_painting_demo");

std::vector<geometry_msgs::msg::Pose> generate_painting_waypoints(
    const std::vector<double>& wall_center, 
    double wall_width, 
    double wall_height,
    double offset = 0.05, 
    double stripe_spacing = 0.1)
{
    std::vector<geometry_msgs::msg::Pose> waypoints;
    
    double start_y = wall_center[1] - wall_width / 2.0;
    double end_y = wall_center[1] + wall_width / 2.0;
    double start_z = wall_center[2] + wall_height / 2.0;
    
    int num_stripes = static_cast<int>(wall_height / stripe_spacing) + 1;
    
    RCLCPP_INFO(LOGGER, "Generating %d stripes for painting", num_stripes);
    
    for (int i = 0; i < num_stripes; ++i)
    {
        double z_pos = start_z - i * stripe_spacing;
        
        if (z_pos < wall_center[2] - wall_height / 2.0)
            break;
        
        double y_start, y_end;
        if (i % 2 == 0)  // Even rows: left to right
        {
            y_start = start_y;
            y_end = end_y;
        }
        else  // Odd rows: right to left
        {
            y_start = end_y;
            y_end = start_y;
        }
        
        // Create start pose for this stripe
        geometry_msgs::msg::Pose pose_start;
        pose_start.position.x = wall_center[0] - offset;  // Distance from wall
        pose_start.position.y = y_start;
        pose_start.position.z = z_pos;
        
        pose_start.orientation.x = 0.0;
        pose_start.orientation.y = 0.707;
        pose_start.orientation.z = 0.0;
        pose_start.orientation.w = 0.707;
        
        waypoints.push_back(pose_start);
        
        // Create end pose for this stripe
        geometry_msgs::msg::Pose pose_end;
        pose_end.position.x = wall_center[0] - offset;
        pose_end.position.y = y_end;
        pose_end.position.z = z_pos;
        pose_end.orientation.x = 0.0;
        pose_end.orientation.y = 0.707;
        pose_end.orientation.z = 0.0;
        pose_end.orientation.w = 0.707;
        
        waypoints.push_back(pose_end);
    }
    
    RCLCPP_INFO(LOGGER, "Generated %zu waypoints", waypoints.size());
    return waypoints;
}
double get_trajectory_duration(const moveit_msgs::msg::RobotTrajectory& trajectory)
{
    if (trajectory.joint_trajectory.points.empty())
    {
        return 0.0;
    }
    
    // The last point's time_from_start gives us the total duration
    auto last_point = trajectory.joint_trajectory.points.back();
    double duration = last_point.time_from_start.sec + 
                     last_point.time_from_start.nanosec * 1e-9;
    
    return duration;
}
int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::NodeOptions node_options;
    node_options.automatically_declare_parameters_from_overrides(true);
    auto move_group_node = rclcpp::Node::make_shared("wall_painting_demo", node_options);

    // Spin up executor
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(move_group_node);
    std::thread([&executor]() { executor.spin(); }).detach();

    // Setup MoveIt
    static const std::string PLANNING_GROUP = "panda_arm";
    moveit::planning_interface::MoveGroupInterface move_group(move_group_node, PLANNING_GROUP);
    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

    const moveit::core::JointModelGroup* joint_model_group =
        move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);

    namespace rvt = rviz_visual_tools;
    moveit_visual_tools::MoveItVisualTools visual_tools(move_group_node, "panda_link0", 
                                                        "wall_painting_demo",
                                                        move_group.getRobotModel());
    visual_tools.deleteAllMarkers();
    visual_tools.loadRemoteControl();

    Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
    text_pose.translation().z() = 1.0;
    visual_tools.publishText(text_pose, "Wall_Painting_Demo", rvt::WHITE, rvt::XLARGE);
    visual_tools.trigger();

    // Display basic information
    RCLCPP_INFO(LOGGER, "Planning frame: %s", move_group.getPlanningFrame().c_str());
    RCLCPP_INFO(LOGGER, "End effector link: %s", move_group.getEndEffectorLink().c_str());

    visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to start wall painting");

    // Wall parameters
    std::vector<double> wall_center = {0.4, 0.0, 0.25};
    double wall_width = 0.8;
    double wall_height = 0.5;
    double offset = 0.05;  // Distance from wall (safety margin)
    double stripe_spacing = 0.10;  // Spacing between horizontal stripes

    std::vector<geometry_msgs::msg::Pose> waypoints = 
        generate_painting_waypoints(wall_center, wall_width, wall_height, offset, stripe_spacing);

    RCLCPP_INFO(LOGGER, "Visualizing waypoints...");
     for (auto& p : waypoints)
    {
        visual_tools.publishSphere(p, rvt::RED, 0.02);
        visual_tools.publishAxis(p, 0.05);
    }
    visual_tools.trigger();
    visual_tools.prompt("Press 'next' to plan the Cartesian path");


    double eef_step = 0.01;  // 1 cm resolution
    double jump_threshold = 0.0;  // Disable jump threshold (set to 0.0)

    moveit_msgs::msg::RobotTrajectory trajectory;
    double fraction = move_group.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);

    RCLCPP_INFO(LOGGER, "Cartesian path (%.2f%% achieved)", fraction * 100.0);
    double planned_duration = get_trajectory_duration(trajectory);
    visual_tools.publishPath(waypoints, rvt::LIME_GREEN, rvt::SMALL);
    visual_tools.publishTrajectoryLine(trajectory, joint_model_group);
    visual_tools.trigger();

    if (fraction < 0.9)
    {
        RCLCPP_WARN(LOGGER, "Cartesian path planning achieved only %.2f%% of the path", 
                    fraction * 100.0);
        visual_tools.prompt("Path incomplete. Press 'next' to try executing anyway or exit");
    }
    else
    {
        visual_tools.prompt("Press 'next' to execute the painting motion");
    }
    auto start_time = std::chrono::high_resolution_clock::now();
       move_group.execute(trajectory);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto execution_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    double execution_seconds = execution_duration.count() / 1000.0;
    
    RCLCPP_INFO(LOGGER, "Planned duration:   %.2f seconds", planned_duration);
    RCLCPP_INFO(LOGGER, "Actual execution:   %.2f seconds", execution_seconds);
    RCLCPP_INFO(LOGGER, "Difference:         %.2f seconds", 
                std::abs(execution_seconds - planned_duration));
   


    rclcpp::shutdown();
    return 0;
}