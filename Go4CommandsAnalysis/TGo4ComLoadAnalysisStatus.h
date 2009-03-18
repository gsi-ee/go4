#ifndef TGO4COMLOADANALYSISSTATUS_H
#define TGO4COMLOADANALYSISSTATUS_H

#include "TGo4AnalysisCommand.h"
#include "TString.h"

/**
 * Loads analysis settings from file specified by name.
 */

class TGo4ComLoadAnalysisStatus : public TGo4AnalysisCommand {
  public:

    TGo4ComLoadAnalysisStatus();

    TGo4ComLoadAnalysisStatus(const char* filename);

    virtual ~TGo4ComLoadAnalysisStatus();

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
     TString fxFileName;

   ClassDef(TGo4ComLoadAnalysisStatus,1)
};

#endif //TGO4COMLOADANALYSISSTATUS_H
