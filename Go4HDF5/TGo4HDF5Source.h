// $Id: $
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

#include "TString.h"

#include "TGo4HDF5SourceParameter.h"

#ifndef __CINT__
#include "H5Cpp.h"
#endif

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


  protected:


    /** opens the hdf5 file of given name for reading */
       void OpenFile(const char* fname);

       /** opens the hdf5 file depending on the setup */
       void CloseFile();

       /** initialize dataset from event structure*/
       void BuildDataSet(TGo4EventElement* event, size_t parentoffset=0);

       /** delete dataset resource*/
       void DeleteDataSet();

       /** evaluate total memory size of event object regarding composite subevents*/
       size_t ScanEventSize(TGo4EventElement* event);


  private:

#ifndef __CINT__

    H5::H5File* fxFile; //!

       H5::DataSet fxDataSet; //!

       H5::CompType* fxType; //!

       H5::DataSpace* fxMemorySpace; //!

       H5::DataSpace fxFileSpace; //!

#endif

       /** True if branch already exists. Used for automatic creation
         * of new event branch within Store method. */
       Bool_t fbDataSetExists;

       /** Points to event structure to be filled into dataset. */
       TGo4EventElement * fxEvent; //!

       /** read buffer for hdf5*/
       Char_t* fxReadBuffer; //!

       /** size of event structure in bytes, for redefining output dataset*/
       size_t fiEventSize;

       /** begin of real eventdata payload after event object pointer**/
       size_t fiReadOffset;


#ifndef __CINT__
       /** counter of filled events. */
       hsize_t fiFillCount; //!
#endif

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
