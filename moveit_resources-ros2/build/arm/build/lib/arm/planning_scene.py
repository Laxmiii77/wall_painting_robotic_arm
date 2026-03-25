#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from moveit_msgs.msg import CollisionObject, PlanningScene
from shape_msgs.msg import SolidPrimitive
from geometry_msgs.msg import Pose
import time


class CylinderUpdater(Node):
    def __init__(self):
        super().__init__('cylinder_updater')
        
        # Create publisher for planning scene
        self.scene_pub = self.create_publisher(
            PlanningScene, 
            '/planning_scene', 
            10
        )
        
        # Wait for publisher to be ready
        self.get_logger().info('Waiting for planning scene publisher...')
        time.sleep(2.0)
        
        # Update the cylinder
        self.update_cylinder()
        
    def update_cylinder(self):
        """Remove old box and add new one with updated dimensions."""
        
        # CHANGE THESE VALUES
        box_name = "box"  # Your cylinder's name
        position = [0.8, 0.0, 0.3]  # x, y, z position
        new_length = 0.8         # New height in meters
        new_breadth = 0.05
        new_height = 0.5         # New radius in meters
        
        # Step 1: Remove the old cylinder
        self.remove_object(box_name)
        time.sleep(1.0)
        
        # Step 2: Add cylinder with new dimensions
        self.add_box(box_name, position, new_length, new_breadth,new_height)
        time.sleep(1.0)
        
        self.get_logger().info('wall updated successfully!')
        
    def remove_object(self, name):
        """Remove an object from the planning scene."""
        scene = PlanningScene()
        scene.is_diff = True
        
        collision_object = CollisionObject()
        collision_object.id = name
        collision_object.operation = CollisionObject.REMOVE
        
        scene.world.collision_objects.append(collision_object)
        self.scene_pub.publish(scene)
        
    def add_box(self, name, position, length,breadth,height):
        """Add a cylinder to the planning scene."""
        scene = PlanningScene()
        scene.is_diff = True
        
        collision_object = CollisionObject()
        collision_object.header.frame_id = "world"
        collision_object.header.stamp = self.get_clock().now().to_msg()
        collision_object.id = name
        
        # Create cylinder primitive
        box = SolidPrimitive()
        box.type = SolidPrimitive.BOX
        box.dimensions =  [length,breadth,height]
        
        # Set pose
        pose = Pose()
        pose.position.x = position[0]
        pose.position.y = position[1]
        pose.position.z = position[2]
        pose.orientation._z = 1.00
        
        collision_object.primitives.append(box)
        collision_object.primitive_poses.append(pose)
        collision_object.operation = CollisionObject.ADD
        
        scene.world.collision_objects.append(collision_object)
        self.scene_pub.publish(scene)


def main(args=None):
    rclpy.init(args=args)
    
    updater = CylinderUpdater()
    
    # Keep node alive briefly to ensure updates complete
    rclpy.spin_once(updater, timeout_sec=3.0)
    
    updater.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()