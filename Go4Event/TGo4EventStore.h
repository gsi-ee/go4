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

#ifndef TGO4EVENTSTORE_H
#define TGO4EVENTSTORE_H

#include "TNamed.h"

class TGo4EventElement;
class TGo4Parameter;
class TGo4Condition;
class TGo4Fitter;
class TFolder;
class TTree;


/** The abstract interface class for the raw event store.
 * Raw events are delivered from the raw event source, may be stored in a raw event store, and are processed
 * (unpacked) by a subelement factory to construct the user event (the
 * user detector data, respectively)
 * @interface
 * @author J. Adamczewski
 * @since 12/2000; added some Store() methods 10/2003 */

class TGo4EventStore : public TNamed {

  public:
    TGo4EventStore();

    TGo4EventStore(const char* name);

    virtual ~TGo4EventStore();

    /** Stores eventelement event into the storage implementation.
      * May be a file, a root tree, a socket to a viewer... */
    virtual Int_t Store(TGo4EventElement* event) = 0;

    /** Saves a parameter (e.g. calibration object) correllated with the current event into the storage.
      * This method is optional, i.e. it needs not to be implemented for all subclasses. */
    virtual Int_t Store(TGo4Parameter* cali);

    /** Saves a condition correllated with the current event into the storage.
      * This method is optional, i.e. it needs not to be implemented for all subclasses. */
    virtual Int_t Store(TGo4Condition* conny);

    /** Saves a fitter correllated with the current event into the storage.
      * This method is optional, i.e. it needs not to be implemented for all subclasses. */
    virtual Int_t Store(TGo4Fitter* fitter);

    /** Saves a folder correllated with the current event into the storage.
      * This method is optional, i.e. it needs not to be implemented for all subclasses. */
    virtual Int_t Store(TFolder* fold);

    /** Access to a tree that might belong to this storage implementation.
      * To be overridden. */
    virtual TTree* GetTree() { return 0; }

    /** We overwrite the default TNamed::Clear that would
      *  erase our name and title!
      * Implement this method in your parameter class
      * if you would like to reset any values with the
      * eraser button in the gui remote browser*/
    virtual void Clear(Option_t* opt="");

  protected:
     /** text length */

     enum { fguTXTLEN = 256 };

   ClassDef(TGo4EventStore,1)
};

#endif //TGO4EVENTSTORE_H

