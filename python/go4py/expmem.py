
import ROOT
from fnmatch import fnmatchcase


class ExpMem(object):
    """
    Wraps exported Go4 memory .root-files
    """

    def __init__(self, filename, rootfolder="Analysis"):
        """
        Open the TFile filename as file and
        get the TDirectory rootfolder as root
        For rootfolder, "Workspace" might be desired besides the default
        An unset rootfolder, "*", and "/" all mean the actual root of the file
        """
        if not rootfolder or rootfolder in ["*", "/"]:
            rootfolder = ""

        self.file = f = ROOT.TFile(filename)
        self.root = f.GetDirectory(rootfolder)

        if not self.root:
            self.root = self.file


    def Objects(self, folder=None):
        """
        Iterate over all objects in the opened .root-file
        folder can be a string (relative to rootfolder) or a TDirectory
        """
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
        """
        Iterate over Objects() that match expr
        """
        for obj in self.Objects(folder):
            name = obj.GetName()
            if fnmatchcase(name, expr):
                yield obj


    def tree(self, folder=None, indent=1):
        """
        Recursively print the object/folder tree of the opened .root-file
        """
        if folder is None:
            folder = self.root
            print folder.GetName()

        diriter = folder.GetListOfKeys().MakeIterator()
        while True:
            key = diriter.Next()
            if not key:
                break
            if key.GetClassName().startswith("TDirectory"):
                subfolder = key.ReadObj()
                sfname = subfolder.GetName()
                print "-"*indent, sfname
                self.tree(subfolder, indent+1)
            else:
                print "="*indent, key.ReadObj().GetName()



