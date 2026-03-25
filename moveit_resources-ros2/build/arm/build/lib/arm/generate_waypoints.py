from geometry_msgs.msg import Pose

def generate_painting_waypoints(wall_center, wall_width, wall_height, 
                                     offset=0.05, stripe_spacing=0.1):

        waypoints = []
        
        start_y = wall_center[1] - wall_width/2
        end_y = wall_center[1] + wall_width/2
        start_z = wall_center[2] + wall_height/2
        
        num_stripes = int(wall_height / stripe_spacing) + 1
        
        for i in range(num_stripes):
            z_pos = start_z - i * stripe_spacing
            
        
            if z_pos < wall_center[2] - wall_height/2:
                break
            
            if i % 2 == 0:  
                y_start, y_end = start_y, end_y
            else:  
                y_start, y_end = end_y, start_y
            
        
            pose_start = Pose()
            pose_start.position.z = wall_center[0] - offset
            pose_start.position.y = y_start
            pose_start.position.x = z_pos
    
            pose_start.orientation.x = 0.0
            pose_start.orientation.y = 0.707
            pose_start.orientation.z = 0.0
            pose_start.orientation.w = 0.707
            waypoints.append(pose_start)
            
        
            pose_end = Pose()
            pose_end.position.z = wall_center[0] - offset
            pose_end.position.y = y_end
            pose_end.position.x = z_pos
            pose_end.orientation.x = 0.0
            pose_end.orientation.y = 0.707
            pose_end.orientation.z = 0.0
            pose_end.orientation.w = 0.707
            waypoints.append(pose_end)
        print(waypoints)
        return waypoints
def main():
     wall_center = [0.4,0.0,0.25]
     wall_width = 0.8
     wall_height = 0.5
     generate_painting_waypoints(wall_center,wall_width,wall_height)
    
main()