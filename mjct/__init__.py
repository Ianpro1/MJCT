import os
__all__ = ['make']


def make(ENV_ID, render=False, timestep=0.02):
    filepath = os.path.dirname(__file__)
    from . import mujocotasks
    if ENV_ID == "Tosser":
        return mujocotasks.Tosser(filepath+'/models/tosser.xml', render, timestep)
    else:
        error = "'" + str(ENV_ID) + "' does not exist! A list of environment ids can be found on: https://github.com/Ianpro1/MJCT \n"
        raise ValueError(error)