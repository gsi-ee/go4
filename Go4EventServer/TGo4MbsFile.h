#ifndef TGO4MBSFILE_H
#define TGO4MBSFILE_H

#include "TGo4MbsSource.h"

class TGo4MbsFileParameter;

/**
 * Implements the gsi mbs stream server into the
 * go4 classes. This is an alias for the MbsSource
 * Class with parameter GETEVT__FILE
 * @author J. Adamczewski
 * @since 1/2001
 */

class TGo4MbsFile : public TGo4MbsSource {
  public:

    TGo4MbsFile(const char* name);

    TGo4MbsFile();

    TGo4MbsFile(TGo4MbsFileParameter* par);

    ~TGo4MbsFile();

    /** Close the file or connection. */
    virtual Int_t Close();

    /** Open the file or connection. */
    virtual Int_t Open();

    /** Requests the next event from mbs source. Returns event status value. */
    virtual Int_t NextEvent();

    /** Name of the Tagfile */
    const char* GetTagName() const { return fxTagFile.Data(); }

    const char* GetCurrentTagName() const { return fxCurrentTag.Data(); }

    const char* GetCurrentFileName() const { return fxCurrentFile.Data(); }

    /** interface from eventsource base class to give active file*/
    virtual const char* GetActiveName();

    /** string to indicate that no lmd tagfile is chosen */
    static const Text_t fgcNOTAGFILE[];

    /** Filename of default multi input file from wildcard evaluation */
    static const Text_t fgcWILDFILE[];

    /** standard suffix for listmode data files */
    static const Text_t fgcLMDSUF[];

    /** standard suffix for list of listmode files */
    static const Text_t fgcFILELISTSUF[];

    Bool_t fbFirstEvent;

  private:

    /** For multiple input mode: Open next file in list. */
    Int_t NextFile();

    /** Open current file/tagfile. */
    Int_t OpenFile();

    /** Close current file/tagfile. */
    Int_t CloseFile();

    /** Called whenever new file is opened in multiple mode.
      * For ctor, argument dosave should be false
      * (no autosave possible inside InitEventClasses()) */
    Int_t NewFileAction(Bool_t dosave=kTRUE);

    /** Name of the tagfile associated with the listmode event file. */
    TString fxTagFile;

    /** Currently processed file (for multiple input mode) */
    TString fxCurrentFile;

    /** Currently processed tagfile (for multiple input mode) */
    TString fxCurrentTag;

    /** Currently processed multiple input filename (for multiple input mode) */
    TString fxMultiName;

    /** kTRUE if multiple input mode is active */
    Bool_t fbMultipleMode;

    /** kTRUE if wildcard input mode is active */
    Bool_t fbWildcardMode;

    /** file that contains the list of mbs filenames.
      * format should be: "filename.lmd tagfilename" for each line. */
    std::ifstream* fxMultiFile; //!

    /** This indicates if the current file is open or not */
    Bool_t fbFileOpen;

  ClassDef(TGo4MbsFile,1)
};

#endif //TGO4MBSFILE_H
