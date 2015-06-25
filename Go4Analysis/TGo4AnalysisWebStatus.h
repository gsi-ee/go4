// $Id: TGo4AnalysisWebStatus.h 1396 2015-02-20 15:11:10Z adamczew $
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

#ifndef TGO4ANALYSISWEBSTATUS_H
#define TGO4ANALYSISWEBSTATUS_H

#include "TGo4AnalysisStatus.h"


/**
 * Subclass of TGo4AnalysisStatus that has UpdateFromURL method
 * the only reason for this subclass is to keep Qt4 GUI independent of full analysis lib
 * All direct  calls to reconfigure TGo4Analysis itself are put here
 * this is not needed by traditional GUI due to text command interface
 *
 * @since 3/2015  @authors JAM
 */
class TGo4AnalysisWebStatus : public TGo4AnalysisStatus {

  friend class TGo4Analysis;

  public:

    /** default ctor for streamer. */
    TGo4AnalysisWebStatus(): TGo4AnalysisStatus() {}

    TGo4AnalysisWebStatus(const char* name): TGo4AnalysisStatus(name) {}

    virtual ~TGo4AnalysisWebStatus() {}

    /** Method used by HTTP server to update some fields, specified in URL syntax */
    Bool_t UpdateFromUrl(const char* rest_url_opt);

    /** Methods used by Go4GUI via HTTP to obtain status of analysis */
    TGo4AnalysisStatus* CreateStatus();

    /** Method used by Go4GUI via HTTP to set status to analysis */
    Bool_t ApplyStatus(TGo4AnalysisStatus* status);

    Bool_t LoadStatus(const char* fname);

    Bool_t SaveStatus(const char* fname);

    Bool_t WriteAutoSave(const char* fname,
                         Bool_t overwrite,
                         Int_t complevel);

    Bool_t ExecuteLine(const char* cmd);

    /** web condition editor keywords used in UpdateFromUrl: */
    static TString fgxURL_STARTRUN;
    static TString fgxURL_ENABLESTEP;
    static TString fgxURL_ENABLESOURCE;
    static TString fgxURL_ENABLESTORE;

    static TString fgxURL_SOURCE_TYPE;
    static TString fgxURL_SOURCE_NAME;
    static TString fgxURL_SOURCE_PORT;
    static TString fgxURL_SOURCE_TIMEOUT;
    static TString fgxURL_SOURCE_RETRY;
    static TString fgxURL_SOURCE_FIRST;
    static TString fgxURL_SOURCE_LAST;
    static TString fgxURL_SOURCE_SKIP;
    static TString fgxURL_STORE_TYPE;
    static TString fgxURL_STORE_NAME;
    static TString fgxURL_STORE_SPLIT;
    static TString fgxURL_STORE_BUF;
    static TString fgxURL_STORE_COMP;
    static TString fgxURL_STORE_ASF;
    static TString fgxURL_STORE_OVERWRITE;

    static TString fgxURL_ASF_SAVE;
    static TString fgxURL_ASF_ENABLED;
    static TString fgxURL_ASF_NAME;
    static TString fgxURL_ASF_INTERVAL;
    static TString fgxURL_ASF_COMP;
    static TString fgxURL_ASF_OVERWRITE;

    static TString fgxURL_PREFS_NAME;
    static TString fgxURL_PREFS_LOAD;
    static TString fgxURL_PREFS_SAVE;
    static TString fgxURL_CLOSE;

  ClassDef(TGo4AnalysisWebStatus,1)

};

#endif //TGo4AnalysisWebStatus_H
