import os
__all__ = ['make']


def make(ENV_ID, render=False, timestep=0.02):
    filepath = os.path.dirname(__file__)
    from . import mujocotasks
    if ENV_ID == "Tosser":
        return mujocotasks.Tosser(filepath+'/models/tosser.xml', render, timestep)
    else:
        raise "env id: " + ENV_ID + " does not exists!\n"