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

#ifndef TGO4HDF5STORE_H
#define TGO4HDF5STORE_H

#include "TGo4EventStore.h"
#include "TGo4HDF5Adapter.h"

class TFile;
class TTree;

class TGo4EventCalibration;
class TGo4EventElement;
class TGo4HDF5StoreParameter;

/**
 * Event store which fills entries to a dataset in a hdf5 formatted file
 *  * @author J. Adamczewski-Musch
 * @since 12/2018
 */
class TGo4HDF5Store : public TGo4EventStore, public TGo4HDF5Adapter {
  public:

    TGo4HDF5Store();

    TGo4HDF5Store(const char *name, UInt_t flags);

    TGo4HDF5Store(TGo4HDF5StoreParameter* par);

    virtual ~TGo4HDF5Store();

    /** Stores eventelement event into the storage implementation.
      * May be a file, a root tree, a socket to a viewer... */
    Int_t Store(TGo4EventElement *event) override;

    /** Saves a calibration object correlated with the current event into the storage. */
    Int_t Store(TGo4Parameter *cali) override;

    /** Saves a condition correlated with the current event into the storage. */
    Int_t Store(TGo4Condition *conny) override;

    /** Saves a fitter correlated with the current event into the storage. */
    Int_t Store(TGo4Fitter* fitter) override;

    /** Saves a complete objects folder correlated with the current event into the storage. */
    Int_t Store(TFolder *fold) override;

  protected:

    /** opens the hdf5 file depending on the setup */
    void OpenFile(const char *name = nullptr) override;

    /** opens the hdf5 file depending on the setup */
//    virtual void CloseFile();

    /** used by all Store methods to write with referencing event number in name */
    void WriteToStore(TNamed *ob);

    /** initialize dataset from event structure*/
    void BuildDataSet(TGo4EventElement *event) override;


  ClassDefOverride(TGo4HDF5Store,1)
};

#endif //TGO4FILESTORE_H
