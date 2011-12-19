// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4MBSFILE_H
#define TGO4MBSFILE_H

#include "TGo4MbsSource.h"

class TGo4MbsFileParameter;
class TList;


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
    static const char* fgcNOTAGFILE;

    /** standard suffix for listmode data files */
    static const char* fgcLMDSUF;

    /** standard suffix for list of listmode files */
    static const char* fgcFILELISTSUF;

  private:

    void AddFileName(const char* name, const char* tagname = 0, bool isonly = kFALSE);

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

    /** file that contains the list of mbs filenames.
      * format should be: "filename.lmd tagfilename" for each line. */
    TList* fxMultiFile; //!

    /** This indicates if the current file is open or not */
    Bool_t fbFileOpen;

    /** Indicates if mbs file should show information about new open file */
    Bool_t fbShowInfo; //!

  ClassDef(TGo4MbsFile, 3)
};

#endif //TGO4MBSFILE_H
