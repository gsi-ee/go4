
import go4


def MatchingObjects(expr="*", folder="*"):
    """Pythonic version of NextMatchingObject"""
    if folder in ["*", "/"]:
        folder = None
    yield go4.NextMatchingObject(expr, folder, True)
    while True:
        obj = go4.NextMatchingObject(expr, folder, False)
        if obj is None:
            break
        yield obj


