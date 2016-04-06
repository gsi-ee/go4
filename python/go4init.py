
"""
Add PyROOT-bound go4 object to builtins
this allows it to be accessed inside imported modules, cf. go4py.go4.go4Wrapper
"""


try:
    go4
except NameError as e:
    msg = "Are you importing go4init outside of Go4?"
    a = list(e.args)
    a[0] += "\n" + " " * len("NameError: ") + msg
    e.args = tuple(a)
    raise


import __builtin__
__builtin__.go4 = go4



