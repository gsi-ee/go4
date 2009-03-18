#ifndef TGO4ANALYSISOBJECTCOMMAND_H
#define TGO4ANALYSISOBJECTCOMMAND_H

#include "TGo4AnalysisCommand.h"
#include "TString.h"

/**
 * Intermediate class for all commands accessing an analysis object by name.
 * @author J. Adamczewski
 * @since 06-Jun-2001, added folder name 08-Apr-2003
 */
class TGo4AnalysisObjectCommand : public TGo4AnalysisCommand {

  public:

    TGo4AnalysisObjectCommand();

    TGo4AnalysisObjectCommand(const char* comname,
                              const char* description,
                              const char* obname);

    virtual ~TGo4AnalysisObjectCommand();

    /** access to the object name under command. */
    const char* GetObjectName() const { return fxObjectName.Data(); }

    /** set the object name under command. */
    void SetObjectName(const char* name) { fxObjectName = name; }

    const char* GetFolderName() const { return fxFolderName.Data(); }

    void SetFolderName(const char* name) { fxFolderName = name; }

   /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    virtual void Set(TGo4RemoteCommand* remcon);


  private:

    /** Name of the object under command. */
    TString fxObjectName;

    /** Optional subfolder name where the object is located. */
    TString fxFolderName;

  ClassDef(TGo4AnalysisObjectCommand,1)
};

#endif //TGO4ANALYSISOBJECTCOMMAND_H
