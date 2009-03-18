#ifndef TGO4COMMAND_H
#define TGO4COMMAND_H

/**
 * @author M. Hemberger, J. Adamczewski
 */

#include "TNamed.h"
#include "TString.h"

class TGo4CommandReceiver;
class TGo4RemoteCommand;

/** These values define command execution rights */
enum Go4CommandMode_t{
   kGo4ComModeRefused,     // no rights
   kGo4ComModeObserver,    // client may only observe
   kGo4ComModeController,  // client has full control as user
   kGo4ComModeAdministrator // full control over everything
};

/** This is the Go4 Abstract Command Class; part of the Base command pattern */
class TGo4Command  : public TNamed {
   public:

      TGo4Command();

      TGo4Command(const char* name, const char* description, Int_t id);

      virtual ~TGo4Command ();

      /** Do the command action */
      virtual Int_t ExeCom();

      /** For undo functionality (not yet impl.)*/
      virtual Int_t UnExeCom();

      /** Optional action if command is not allowed (warning message e.g.) */
      virtual Int_t RefuseCom();

      virtual Bool_t Log(Int_t mode = 0);

      Bool_t IsEnabled() const;

      Bool_t IsExclusive() const;

      Bool_t IsLocal() const;

      virtual void Help();

      Int_t GetCommandID() const;

      Bool_t IsSynchron() const;

      const char* GetReceiverName() const;
      void SetReceiverName(const char*);

      const char* GetTaskName() const;
      void SetTaskName(const char*);

      const char* What() const;
      void SetDescription(const char*);

      virtual void SetReceiver(TGo4CommandReceiver * rec);

      Go4CommandMode_t GetProtection(){return fiProtection;}
      Go4CommandMode_t GetMode(){return fiMode;}

      /** Set internals of this command from specification of external
        * remote command (command container). Must be overloaded
        * for each specific command! */
       virtual void Set(TGo4RemoteCommand* remcon);

       void SetMode(Go4CommandMode_t m){fiMode=m;}

       /** text description of current execution mode*/
       static const char* GetModeDescription(Go4CommandMode_t mode);

   protected:

      Bool_t fbIsEnabled;   // comand can be executed
      Bool_t fbIsExclusive; // command can be executed only by ReceiverName
      Bool_t fbIsSynchron;  // command is queued = synchronous
      Bool_t fbIsLocal;     // command is to be executed by local command thread

      Int_t fiCommandID;    // command number
      Int_t fiVersion;      // version number

      /** mode value for execution rights of sending task*/
      Go4CommandMode_t fiMode; //
      /** Execute protection of this command. If Execution mode
        * is not sufficient, command is not allowed to be executed.*/
      Go4CommandMode_t fiProtection; //

      void SetProtection(Go4CommandMode_t m){fiProtection=m;}

      /** resolve description of command modes here*/
       static const char* fgcMODEDESCRIPTION[];

   private:

      TString fxDescription;  // short description of command
      TString fxReceiverName; // name of receiver
      TString fxTaskName;     // name of task

   protected:
      TGo4CommandReceiver * fxReceiverBase; //!

   ClassDef(TGo4Command , 1)
};

#endif //TGO4COMMAND_H
