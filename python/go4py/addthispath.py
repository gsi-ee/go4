
import sys, os, inspect


def addthispath():
    """
    Adds the path of the script calling this function to the Python path
    """
    tmp = inspect.currentframe()
    tmp = inspect.getouterframes(tmp)[1][1]
    tmp = os.path.realpath(tmp)
    tmp = os.path.dirname(tmp)
    sys.path.append(tmp)


# getouterframes(currentframe) returns a list with information for each
# frame, starting from the current one with [0] and stepping outward.
# We want to step out one frame to the calling script, hence [1].
# Each entry is a tuple with [1] being the filename.

# An alternative would be inspect.getsourcefile(f) with an f defined in 
# the calling script, e.g., f = lambda:0



