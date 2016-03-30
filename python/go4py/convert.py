
def hist2list(h):
    """
    Read points from histogram h into two lists (x and y coordinates)
    Format is chosen such that matplotlib.pyplot.step() can be used for plotting
    hence (x, y) == (low edge, content)
    """
    xs = []
    ys = []
    for i in range(h.GetNbinsX()):
        x = h.GetBinLowEdge(i+1)
        y = h.GetBinContent(i+1)
        xs.append(x)
        ys.append(y)
    return xs, ys


def eval_func(xs, tf):
    """
    Evaluate the TF1 tf for all values in xs, return list of y values
    """
    ys = []
    for x in xs:
        y = tf.Eval(x)
        ys.append(y)
    return ys



