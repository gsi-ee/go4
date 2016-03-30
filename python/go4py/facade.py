
import sys


class Facade(object):

    def __init__(self, modulename):
        """
        Replaces a module with an instance of a types.ModuleType-based class
        modulename is probably given by __name__ in that module

        Example:
            @Facade(__name__)
            class AnInteriorClass(ModuleType):
                pass
        """
        self.modulename = modulename

    def __call__(self, Interior):
        """
        Replace current module with an instance of Interior in sys.modules
        Update the globals of this instance from the globals of the module
        cf. https://mail.python.org/pipermail/python-ideas/2012-May/014969.html
        """
        n = self.modulename
        w = Interior(n)
        w.__dict__.update(sys.modules[n].__dict__)
        sys.modules[n] = w



