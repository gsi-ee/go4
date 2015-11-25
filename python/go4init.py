
"""
Add PyROOT-bound go4 object to builtins
this allows it to be accessed inside imported modules, cf. go4py.go4.Wrapper
"""

import __builtin__
__builtin__.go4 = go4


