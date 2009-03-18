#ifndef TGO4COMSAVEANALYSISSTATUS_H
#define TGO4COMSAVEANALYSISSTATUS_H

#include "TGo4AnalysisCommand.h"
#include "TString.h"

/**
 * Save current  analysis settings to a file specified by name.
 */

class TGo4ComSaveAnalysisStatus : public TGo4AnalysisCommand {
  public:

    TGo4ComSaveAnalysisStatus();

    TGo4ComSaveAnalysisStatus(const char* filename);

    virtual ~TGo4ComSaveAnalysisStatus();

    Int_t ExeCom();

    /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    virtual void Set(TGo4RemoteCommand* remcon);

    /** Access to the object name under command. */
    const char* GetFileName() const { return fxFileName.Data(); }

    /** Set the object name under command. */
    void SetFileName(const char* name) { fxFileName = name; }

  private:

    /** Name of the settings file. */
    TString fxFileName ;

  ClassDef(TGo4ComSaveAnalysisStatus,1)
};

#endif //TGO4COMSAVEANALYSISSTATUS_H
