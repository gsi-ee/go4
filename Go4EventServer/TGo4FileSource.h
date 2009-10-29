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

#ifndef TGO4FILESOURCE_H
#define TGO4FILESOURCE_H

#include "TGo4EventSource.h"

#include "TString.h"

class TFile;
class TTree;
class TGo4FileSourceParameter;
class TGo4EventElement;
class TGo4CompositeEvent;

/**
 * Raw event source which reads entries from a root TTree in a TFile.
 * File is opened by ctor; method BuildEvent() iterates over tree. Needs pointer to external
 * reference to event structure class which must match the structure of the tree entry
 * New since 10/2004: Name of event element defines tree subbranch
 * to be activated as partial input
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4FileSource : public TGo4EventSource {

  public:

    TGo4FileSource();

    TGo4FileSource(TGo4FileSourceParameter* par);

    TGo4FileSource(const char* name);

    virtual ~TGo4FileSource();

    /** Fill the destination event dest from the tree.
      * If end of tree is reached, the current event counter
      * is reset and a kFALSE value is returned. */
    Bool_t BuildEvent(TGo4EventElement* dest);

    /** Use this method instead of BuildEvent
      * to correctly sync and read Go4 composite event */
    Bool_t BuildCompositeEvent(TGo4CompositeEvent *dest);

  private:

    TFile* fxFile; //!

    TTree * fxTree; //!

    /** Number of events stored in the Tree. Used to check if
      * the complete tree is already read. */
    Int_t fiMaxEvents;

    /** Number of events stored in the Tree. Used to check if
      * the complete tree is already read. */
    Int_t fiCurrentEvent;

    /** name of tree branch to be activated */
    TString fxBranchName;

    /** This flag is used for lazy init of tree in Eventbuilding methods. */
    Bool_t fbActivated; //!

    /** pointer to top branch event */
    TObject* fxTopEvent; //!

    /** Open file. To be used by all constructors. */
    Int_t Open();

  ClassDef(TGo4FileSource,1)
};

#endif //TGO4FILESOURCE_H
