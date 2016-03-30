
import sys


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



