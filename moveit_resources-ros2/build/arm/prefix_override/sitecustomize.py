import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/laxmi-arz-i006/moveit_resources-ros2/install/arm'
