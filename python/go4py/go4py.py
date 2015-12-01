
import go4


def MatchingObjects(expr="*", folder="*"):
    """
    Pythonic version of NextMatchingObject
    Iterate over objects matching expr starting from folder
    For "*" and "/" as folder, the root folder will be used
    """
    if folder in ["*", "/"]:
        folder = None
    yield go4.NextMatchingObject(expr, folder, True)
    while True:
        obj = go4.NextMatchingObject(expr, folder, False)
        if obj is None:
            break
        yield obj



