#ifndef TGO4EXPORTMANAGER_H
#define TGO4EXPORTMANAGER_H

#include "TNamed.h"
#include "TString.h"

class TH1;
class TH2;
class TCollection;
class TFolder;
class TDirectory;
class TGraph;


enum Go4Export_t
{
   GO4EX_ASCII,
   GO4EX_ASCII_CHANNELS,
   GO4EX_RADWARE,
   GO4EX_ROOT,
   GO4EX_XML
};

/**
 * This class manages all export and import conversions of
 * objects to/from different file formats like ascii, radware, etc.
 *
 * @author J. Adamczewski
 * @since 05-Apr-2004
 */
class TGo4ExportManager : public TNamed {
  public:

    TGo4ExportManager();

    TGo4ExportManager(const char* name);

    virtual ~TGo4ExportManager();

    /**  Conversion of object into selected file format in working dir. If object
      * is a collection, folder or root directory, all contents of these are
      * converted recursively. The substructure hierarchy is expressed by means of
      * subdirectories of the file system, or within the output root file, respectively*/
    void Export(TObject* ob, Go4Export_t format);

    /**  Treat all general objects. Downcast types for overloaded export methods.
      * Conversion format is to be specified with SetFilter method first.*/
    void Export(TObject* ob);

    /**  Conversion of histogram into filter format file */
    void Export(TH1* histo);

    /**  Conversion of graph into filter file*/
    void Export(TGraph* graph);

    /**  Recursive conversion of all objects in fold specified filter files.
      * folder structure will be represented by subdirs on file system.*/
    void Export(TFolder* fold);

    /**  Recursive conversion of all objects in directory into filter format.
      * folder structure will be represented by subdirs on file system.*/
    void Export(TDirectory* dir);

    /**  Recursive conversion of all objects in collection into ascii files.
      * folder structure will be represented by subdirs on file system.*/
    void Export(TCollection* dir);

    void SetFilter(Go4Export_t format);

    void SetCurrentDir(const char* dir=0);

    void SetStartDir(const char* dir=0);

    void SetOutFile(const char* filename=0)
        { fxOutFile = (filename!=0) ? filename : "Go4Export"; }

    void SetOutFileComment(const char* comment=0)
        { fxOutFileComment=(comment!=0) ?  comment : "Saved from Go4"; }

  private:

    /**  Conversion of histogram into ascii file.
      * If channels is true, x/y/z coordinates are in absolute
      * bin channel numbers. Otherwise, export value of bincenters,
      * with respect to axis scale. */
    void ExportASCII(TH1* histo, Bool_t channels=kTRUE);

    /**  Conversion of graph into ascii file*/
    void ExportASCII(TGraph* graph);

    /**  Conversion of histogram into radware file */
    void ExportRadware(TH1* histo);

    /**  Conversion of 2d histogram into radware file */
    void ExportRadware(TH2* histo);

    /**  Conversion of graph into radware file*/
    void ExportRadware(TGraph* graph);

    /**  Store object into root file. Filename can be set by SetOutFile()*/
    void ExportRoot(TObject* ob);

    /**  Store object into root xml file. Filename derived from object name*/
    void ExportXML(TObject* ob);

    /** name of the start (top level) directory. If keys from a directory are
     *  read, it is required that this start directory is current dir.*/
    TString fxStartDir;

    /** name of the current working directory. */
    TString fxCurrentDir;

    /** Name of the output file. Optionally, for ascii export the files are named
      * according to object names. */
    TString fxOutFile;

    /** Comment (title) of the output file. Optionally, for ascii export the files are named
      * according to object names. For root export, this will be TFile t*/
    TString fxOutFileComment;

    /** Active filter format. */
    Go4Export_t fiFilter;
};

#endif //TGO4EXPORTMANAGER_H
