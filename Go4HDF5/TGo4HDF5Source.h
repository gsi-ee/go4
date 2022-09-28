// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4HDF5SOURCE_H
#define TGO4HDF5SOURCE_H

#include "TGo4EventSource.h"
#include "TGo4HDF5Adapter.h"

#include "TString.h"

class TFile;
class TList;
class TGo4HDF5SourceParameter;
class TGo4EventElement;

/**
 *
 * Event source which reads entries from a dataset in a hdf5 formatted file
 * names of elements in hdf5 data must match the members of the input event class
 *  * @author J. Adamczewski-Musch
 * @since 01/2019
 */
class TGo4HDF5Source : public TGo4EventSource, public TGo4HDF5Adapter {

  friend class TGo4HDF5DataHandle;
  friend class TGo4HDF5BasicDataHandle;

  public:

    TGo4HDF5Source();

    TGo4HDF5Source(TGo4HDF5SourceParameter* par);

    TGo4HDF5Source(const char *name);

    virtual ~TGo4HDF5Source();

    /** Fill the destination event dest from the tree.
      * If end of tree is reached, the current event counter
      * is reset and a kFALSE value is returned. */
    Bool_t BuildEvent(TGo4EventElement* dest) override;

    static TList* ProducesFilesList(const char* mask);


  protected:

    /** opens the hdf5 file of given name for reading */
    void OpenFile(const char *fname) override;

    /** initialize dataset from event structure*/
    void BuildDataSet(TGo4EventElement* event) override;

    /** delete dataset resource*/
    void DeleteDataSet() override;

  private:

    /** read buffer for hdf5*/
    Char_t* fxReadBuffer{nullptr}; //!

    /** begin of real eventdata payload after event object pointer**/
    size_t fiReadOffset{0};

    /** list of files names */
    TList* fxFilesNames{nullptr}; //!

    /** current name of the file */
    TString fxCurrentFileName;  //!

    /** Open next file from the files list. */
    Bool_t OpenNextFile();

    /** Close currently open file. */
    Bool_t CloseCurrentFile();

  ClassDefOverride(TGo4HDF5Source,2)
};


#endif //TGO4HDF5SOURCE_H
