
import sys
major, minor = sys.version_info[0:2]

if major == 2:
    from go4py import *
    import go4
    import convert
    from internals.addthispath import addthispath
    from internals.expmem import ExpMem
    from internals.reimport import reimport, reload
else:
    from .go4py import MatchingObjects
    from .internals.addthispath import addthispath
    from .internals.expmem import ExpMem
    from .internals.reimport import reimport, reload
