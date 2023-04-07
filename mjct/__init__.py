import os
import importlib
import numpy as np


__all__ = ['make']


if importlib.util.find_spec("gymnasium") is not None:
    import gymnasium as gym
    disable_gym=False
elif importlib.util.find_spec("gym") is not None:
    import gym
    disable_gym=False
else:
    disable_gym=True

if disable_gym == False:
    class gymEnv(gym.Env):
        def __init__(self, env, obs_shape, action_shape):
            self.env = env
            self.action_space = gym.spaces.Box(low=-1., high=1., shape=action_shape)
            self.observation_space = gym.spaces.Box(low=-np.inf, high=np.inf, shape=obs_shape)
        
        def reset(self):
            return self.env.reset()
        
        def step(self, action):
            return self.env.step(action)
        
        def render(self):
            return self.env.render()


#ERROR TYPES
def render_TypeError(render_type):
    message ="'" + render_type + "' is not a valid rendering type! Here's a list of possible renders: ('', 'rgb_array' and 'glwindow')."
    return Exception(message)



def make(ENV_ID, render="", timestep=0.002, apirate=360, gym=False):
    assert(render, str)
    
    filepath = os.path.dirname(__file__)
    from . import mujocotasks

    if render == "":
        render_type = 0
    elif render == "rgb_array":
        render_type = 1
    elif render == "glwindow":
        render_type = 2
    elif render == "autogl":
        render_type = 3
    else:
        raise render_TypeError(render)

    #args = obs_shape, action_shape
    match ENV_ID:
        
        case "Tosser":
            if render_type == 0:
                brender = False
            elif render_type == 1:
                brender=True
            else:
                raise Exception("The only rendering mode on 'Tosser' is 'rgb_array'!")
            
            env = mujocotasks.Tosser(filepath+'/models/tosser.xml', brender, timestep, apirate)
            args =[(10,), (2,)]
            
        case "TosserCPP":
            env = mujocotasks.TosserCPP(filepath+'/models/tosser.xml', render_type, timestep, apirate)
            args =[(10,), (2,)]

        case _:
            error = "'" + str(ENV_ID) + "' does not exist! A list of environment ids can be found on: https://github.com/Ianpro1/MJCT \n"
            raise ValueError(error)
    
    if gym:
        if disable_gym:
            print("WARNING: Couldn't find gymnasium or gym installation on system, returned mjct environment instead.")
            return env
        else:
            env = gymEnv(env, args[0], args[1])
            return env
    else:
        return env