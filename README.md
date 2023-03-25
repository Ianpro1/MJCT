# MJCT
c/c++ MuJoCo tasks with python api

Dependencies and Versions: mujoco.dll, MuJoCo 2.3.3, glfw 3.3.8

INSTALLATION:
CASE 1:
 1-create a build directory from the main directory
 2-move build.bat to the build directory
 3-run the bat file and it should have built a .pyd file to build/debug (if not go to CASE 2)
 4-move mujoco.dll (located in extern/mujoco/bin after the bat installation) to the debug folder
 5-at this point, inside the debug folder, you have all the required dependencies to import mjct from python
 
 CASE 2:
 1-if from CASE 1 you got USERWARNINGS from the command prompt, simply run the bat file once more
 2-at this point you should be good to go simply follow the steps 4 and 5 from CASE 1
