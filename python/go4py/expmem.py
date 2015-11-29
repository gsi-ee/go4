
import ROOT
from fnmatch import fnmatchcase


class ExpMem(object):

    def __init__(self, name, rootfolder="Analysis"):
        if not rootfolder or rootfolder in ["*", "/"]:
            rootfolder = ""
        self.file = f = ROOT.TFile(name)
        self.root = f.GetDirectory(rootfolder)

    def Objects(self, folder=None):
        if folder is None:
            folder = self.root
        elif isinstance(folder, basestring):
            folder = self.root.GetDirectory(folder)

        if not folder:
            return

        diriter = folder.GetListOfKeys().MakeIterator()
        while True:
            key = diriter.Next()
            if not key:
                break
            if key.GetClassName().startswith("TDirectory"):
                subfolder = key.ReadObj()
                for obj in self.Objects(subfolder):
                    yield obj
            else:
                yield key.ReadObj()


    def MatchingObjects(self, expr="*", folder=None):
        for obj in self.Objects(folder):
            name = obj.GetName()
            if fnmatchcase(name, expr):
                yield obj


