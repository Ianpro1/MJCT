# MJCT (MuJoCo Tasks)
***status: incomplete***

C/C++ reinforcement learning tasks using MuJoCo physics engine with Python API

******

INSTALLATION (FOR WINDOWS ONLY):

```
git clone --recursive https://github.com/Ianpro1/MJCT.git
pip install ./MJCT
```
NOTE: the module is named MuJoCoTasks, therefore to uninstall simply run:
```
pip uninstall mujocotasks
```
You can test the module by creating a Tosser environment object:
>import mjct
>
>env = mjct.Tosser(True)
 
You can also test the rendering capabilities:
>env.render()

 &nbsp;
 
_Dependencies and Versions: mujoco.dll, MuJoCo 2.3.3, glfw 3.3.8_
