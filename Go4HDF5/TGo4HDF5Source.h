// $Id: TGo4HDF5Source.h 2130 2018-08-14 10:13:11Z linev $
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

#ifndef TGO4HDF5SOURCE_H
#define TGO4HDF5SOURCE_H



#include "TGo4EventSource.h"

#include "TString.h"

class TFile;
class TTree;
class TList;
class TGo4HDF5SourceParameter;
class TGo4EventElement;

/**
 * Raw event source which reads entries from a root TTree in a TFile.
 * File is opened by ctor; method BuildEvent() iterates over tree. Needs pointer to external
 * reference to event structure class which must match the structure of the tree entry
 * New since 10/2004: Name of event element defines tree subbranch
 * to be activated as partial input
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4HDF5Source : public TGo4EventSource {

  public:

    TGo4HDF5Source();

    TGo4HDF5Source(TGo4HDF5SourceParameter* par);

    TGo4HDF5Source(const char* name);

    virtual ~TGo4HDF5Source();

    /** Fill the destination event dest from the tree.
      * If end of tree is reached, the current event counter
      * is reset and a kFALSE value is returned. */
    virtual Bool_t BuildEvent(TGo4EventElement* dest);

    static TList* ProducesFilesList(const char* mask);

  private:

    TFile* fxFile; //!

    TTree * fxTree; //!

    /** Number of events stored in the Tree. Used to check if
      * the complete tree is already read. */
    Long64_t fiMaxEvents;

    /** Event number in current tree. */
    Long64_t fiCurrentEvent;

    /** Global event number, starting from the first tree. */
    long int fiGlobalEvent;   //!

    /** This flag is used for lazy init of tree in Eventbuilding methods. */
    Bool_t fbActivated; //!

    /** pointer to top branch event */
    TGo4EventElement* fxTopEvent; //!

    /** list of files names */
    TList* fxFilesNames; //!

    /** current name of the file */
    TString fxCurrentFileName;  //!

    /** Open next file from the files list. */
    Bool_t OpenNextFile();

    /** Close currently open file. */
    Bool_t CloseCurrentFile();

  ClassDef(TGo4HDF5Source,2)
};


#endif //TGO4FILESOURCE_H
