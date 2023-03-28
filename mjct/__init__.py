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


def make(ENV_ID, render=False, timestep=0.02, gym=False):
    filepath = os.path.dirname(__file__)
    from . import mujocotasks

    #args = obs_shape, action_shape
    match ENV_ID:
        
        case "Tosser":        
            env = mujocotasks.Tosser(filepath+'/models/tosser.xml', render, timestep)
            args =[(10,), (2,)]
            
        case "TosserCPP":        
            env = mujocotasks.TosserCPP(filepath+'/models/tosser.xml', render, timestep)
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