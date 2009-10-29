// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4ANALYSISOBJECTNAMES_H
#define TGO4ANALYSISOBJECTNAMES_H

#include "TGo4Status.h"

class TList;
class TFolder;

/**
 * Contains the name (key) list of all objects in the analysis scope. This object is sent via status channel to the gui server on command to update the displayed information.
 * @since 6/2001
 * @author J. Adamczewski
 */

class TGo4AnalysisObjectNames : public TGo4Status {

  friend class TGo4Analysis;
  friend class TGo4AnalysisObjectManager;

  public:

    /** default ctor for streamer. */
   TGo4AnalysisObjectNames();

   virtual ~TGo4AnalysisObjectNames();

    void SetTopFolder(TFolder* f) { fxTopFolder = f; }

    /** Access to the list of the names folder structure. */
    TList * GetFolderList();

    /** Access to the folder containing the names objects. */
    TFolder * GetNamesFolder(Bool_t chown=kFALSE);

    /** basic method to printout status information
     * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

    /** Print names of the objects in given nameslist folder into buffer. */
    Int_t PrintFolder(TFolder* fold, Text_t * buf, Int_t buflen=0);

  private:

    /** Top Level Go4 Folder with all subfolders and the analysis folder
      * contents as TObjString instances. */
    TFolder * fxTopFolder;

    /** private ctor can only invoked by Go4Analysis class, which is friend. */
    TGo4AnalysisObjectNames(const char* name);

  ClassDef(TGo4AnalysisObjectNames,1)
};

#endif //TGO4ANALYSISOBJECTNAMES_H
