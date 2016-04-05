
import ROOT
from array import array


def list2hist(xs, ys, name="anonymous", title=""):
    """
    Create a ROOT.TH1I with xs as bins (low edges) and ys as their content
    name and title will be used for the new histogram
    """
    h = ROOT.TH1I(name, title, len(xs), array('d', xs))
    for i, y in enumerate(ys):
        h.SetBinContent(i+1, y)
    return h


def hist2list(h):
    """
    Read points from histogram h into two lists (x and y coordinates)
    Format is chosen such that matplotlib.pyplot.step() can be used for plotting
    hence (x, y) == (low edges, contents)
    """
    tmp = ((h.GetBinLowEdge(i+1), h.GetBinContent(i+1)) for i in range(h.GetNbinsX()))
    return zip(*tmp)



def eval_func(xs, tf):
    """
    Evaluate the TF1 tf for all values in xs, return list of y values
    """
    return [tf.Eval(x) for x in xs]



