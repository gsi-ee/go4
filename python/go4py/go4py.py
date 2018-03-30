
import go4


def MatchingObjects(expr="*", folder="*"):
    """
    Pythonic version of NextMatchingObject
    Iterate over objects matching expr starting from folder
    For "*" and "/" as folder, the root folder will be used
    """
    if folder in ["*", "/"]:
        folder = None
    reset = True
    while True:
        obj = go4.NextMatchingObject(expr, folder, reset)
        reset = False
        if obj is None:
            break
        yield obj



