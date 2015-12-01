
import sys
from types import ModuleType


class Wrapper(ModuleType):
    """
    Wraps the PyROOT-bound go4 object (transferred here via __builtin__)
    to provide some safety measures
    """
    try:
        analysis = go4
    except NameError:
        analysis = None

    def __getattr__(self, name):
        """Forward missing attributes to the internal go4 object"""
        return getattr(self.analysis, name)

    def NextMatchingObject(self, expr="*", folder=None, reset=False):
        """Safer proxy for the eponymous go4 method"""
        if not folder:
            folder = "Go4"
        obj = self.analysis.NextMatchingObject(expr, folder, reset)
        return self.realNone(obj)


def realNone(obj):
    """
    Replace PyROOT null pointer (== None) with a real Python None (is None)
    """
    return None if obj == None else obj


def init():
    """
    Replace this module with an instance of the Wrapper class in sys.modules
    Update the globals of this instance from the globals of the module
    cf. https://mail.python.org/pipermail/python-ideas/2012-May/014969.html
    """
    w = Wrapper(__name__)
    w.__dict__.update(globals())
    sys.modules[__name__] = w



init()



