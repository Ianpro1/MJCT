# MJCT (MuJoCo Tasks)
***status: incomplete***

C/C++ reinforcement learning tasks using MuJoCo physics engine with Python API

******

INSTALLATION (FOR WINDOWS ONLY):

```
git clone --recursive https://github.com/Ianpro1/MJCT.git
pip install ./MJCT
```
The installed module is actually named MuJoCoTasks, therefore to uninstall simply run:
```
pip uninstall mujocotasks
```
To create an environment simply import mjct and much like openai's gym, use the make function with the desired environment's id:
```
import mjct
env = mjct.make("Tosser")
```
NOTE: If the make function throws an error on its first use after installation, simply restart your interpreter. 
&nbsp;
***ENV_ID LIST***
>Tosser
>
&nbsp;
 
_Dependencies and Versions: mujoco.dll, MuJoCo 2.3.3, glfw 3.3.8_
