
from facade import Facade
from types import ModuleType


def realNone(obj):
    """
    Replace PyROOT null pointer (== None) with a real Python None (is None)
    """
    return None if obj == None else obj



@Facade(__name__)
class go4Wrapper(ModuleType):
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



