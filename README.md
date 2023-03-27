# MJCT (MuJoCo Tasks)
***status: incomplete***

C/C++ reinforcement learning tasks using MuJoCo physics engine with Python API

******

INSTALLATION (FOR WINDOWS ONLY):

```
git clone --recursive https://github.com/Ianpro1/MJCT.git
pip install ./MJCT
```
NOTE: The installed module is actually named MuJoCoTasks, therefore to uninstall simply run:
```
pip uninstall mujocotasks
```
To create an environment simply import mjct and invoke make with the environment's id:
```
import mjct
env = mjct.make("Tosser")
```
&nbsp;
***ENV_ID LIST***
>Tosser
>
&nbsp;
 
_Dependencies and Versions: mujoco.dll, MuJoCo 2.3.3, glfw 3.3.8_
