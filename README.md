# MJCT (Example of Mujoco Tasks Python Bindings)

C/C++ Reinforcement Learning Tasks using MuJoCo Physics Engine with Python Bindings

******

INSTALLATION (FOR WINDOWS ONLY):

```
git clone --recursive https://github.com/Ianpro1/MJCT.git
pip install ./MJCT
```
The installed module is actually named MuJoCoTasks, to uninstall simply run:
```
pip uninstall mujocotasks
```
To create an environment simply import mjct (a more userfriendly module) and much like openai gym, use the make function with the desired environment id:
```
import mjct
env = mjct.make("Tosser")
```
You can also set gym to True if you wish to receive a gymnasium or gym derived environment:
```
env = mjct.make("Tosser", gym=True)
```

&nbsp;

***ENV_ID LIST***
>Tosser
>
>TosserCPP
>
&nbsp;
 
_Dependencies and Versions: mujoco.dll, MuJoCo 2.3.3, glfw 3.3.8_

_More information about MuJoCo can be found on Deepmind's GitHub: https://github.com/deepmind/mujoco_
