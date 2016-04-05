
import sys


# Try to get a reload function:
try:
    # Built-in (Python 2) or imported before
    # allow it to be imported from here by assigning it locally
    reload = reload
except NameError:
    # Python 3
    try:
        # New in 3.1, not in 2.7
        from importlib import reload
    except ImportError:
        # Deprecated since 3.4
        from imp import reload



def reimport(modulename):
    """
    Reloads modulename if it was imported before, imports it otherwise.

    Workaround for some implications of the otherwise very useful fact
    that TPython's interpreter state is preserved in between calls:
    Assume a main script and a library module. Once the script has 
    imported the library, changes to the library are not considered 
    anymore. A cached version is used instead. To see changes a reload 
    is necessary. This function should simply do the right thing...
    """
    try:
        m = sys.modules[modulename]
    except KeyError:
        m = __import__(modulename)
    else:
        reload(m)
    finally:
        return m



# Alternatively the module can be deleted to trigger a clean import:

#if 'myModule' in sys.modules:  
#    del sys.modules["myModule"]

#for mod in sys.modules.keys():
#    if mod.startswith('myModule.'):
#        del sys.modules[mod]



