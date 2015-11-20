
"""
Add PyROOT-bound go4 object to builtins
this allows it to be accessed inside imported modules
"""

import __builtin__
__builtin__.go4 = go4


