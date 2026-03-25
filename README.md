# 🖌️ Wall Painting Robot (ROS 2 + MoveIt 2)

## Simulation Video
[https://www.youtube.com/watch?v=1SAAv57ydL4](https://www.youtube.com/watch?v=h-pQkV4MC3g)

<image src=image.png/>

## 📌 Overview
This project focuses on designing and simulating a robotic wall-painting system using a **Franka Panda manipulator** in a **ROS 2 + MoveIt 2** environment.

The system generates optimized painting trajectories, executes smooth Cartesian motion, and evaluates performance based on wall coverage and execution time.

---

## 🚀 Features
- Coverage path planning using zig-zag pattern  
- Franka Panda robotic arm simulation  
- Waypoint generation algorithm  
- Cartesian trajectory execution using MoveIt 2  
- Performance evaluation (coverage % and execution time)  
- Optional depth camera + point cloud processing  

---

## 🏗️ System Architecture
1. Define wall parameters (width, height, center, offset)  
2. Generate zig-zag waypoints  
3. Send waypoints to MoveIt 2  
4. Execute Cartesian trajectory  
5. Log execution time and coverage  

---


## 📊 Results

| Test | Coverage | Planned Time | Actual Time |
|------|---------|-------------|------------|
| 1 | 53.09% | 27.44s | 27.48s |
| 2 | 26.04% | 25.79s | 25.84s |
| 3 | 33.22% | 54.43s | 54.49s |
| 4 | 45.21% | 50.58s | 50.64s |
| 5 | 45.44% | 15.79s | 15.84s |

---

## ⚠️ Limitations
- Robot reach limitations (~0.855 m) cause singularities  
- Assumes full wall reachability  
- No obstacle avoidance implemented  

---

## 🔮 Future Improvements
- Adaptive stripe spacing for better coverage  
- Obstacle-aware planning (doors, windows)  
- Real-time wall detection using depth camera  
- Parameter tuning for near 100% coverage  

---

## 🛠️ Tech Stack
- ROS 2 Jazzy  
- MoveIt 2  
- Gazebo  
- RViz  
- C++  
- Python  
- OpenCV  
- URDF  

---
