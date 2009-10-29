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

#ifndef TGO4TREESTRUCTURE_H
#define TGO4TREESTRUCTURE_H

#include "TGo4Status.h"

#include "TFolder.h"

/**
 * Contains the branchelement structure  of a certain TTree on the
 * analysis side. Name of this status object is the same as tree name.
 * @since 6/2001
 * @author J. Adamczewski
 */
class TGo4TreeStructure : public TGo4Status {

  friend class TGo4Analysis;
  friend class TGo4AnalysisObjectManager;

  public:

    /** default ctor for streamer. */
    TGo4TreeStructure();

    virtual ~TGo4TreeStructure();

    /** Access to the list of the names folder structure. */
    TList * GetFolderList();

    /** Access to the folder containing the names objects. */
    TFolder * GetNamesFolder(Bool_t chown=kFALSE);

  private:

    /** Folder containing the tree structure */
    TFolder * fxTopFolder;

    /** private ctor can only invoked by Go4Analysis class, which is friend. */
    TGo4TreeStructure(const char* name);

  ClassDef(TGo4TreeStructure,1)
};

#endif //TGO4TREESTRUCTURE_H
