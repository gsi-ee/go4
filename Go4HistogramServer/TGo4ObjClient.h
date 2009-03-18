#ifndef TGO4OBJCLIENT_H
#define TGO4OBJCLIENT_H

#include "TNamed.h"
#include "TString.h"

class TGo4AnalysisObjectNames;
class TGo4Socket;
class TBuffer;

/**
 * Client for the go4 object server delivering root objects.
 * @author J. Adamczewski
 * @since 17-Jan-2003
 */

class TGo4ObjClient : public TNamed {
   public:

      TGo4ObjClient();

      TGo4ObjClient(const char* clientname,
                    const char* base=0,
                    const char* passwd=0,
                    const char* host=0,
                    Int_t port=0);

      virtual ~TGo4ObjClient();

      /**  Get nameslist object from client.
        *  basename, password,host and portnumber may be specified at any time new
        * if left out (default), the previously stored values are used */
      TGo4AnalysisObjectNames* RequestNamesList(const char* base=0,
                                                const char* passwd=0,
                                                const char* host=0,
                                                Int_t port=0);

       /**  Get object by name from client.
        *  basename, password, host and portnumber may be specified at any time new
        * if left out (default), the previously stored values are used */
      TObject* RequestObject(const char* objectname,
                             const char* base=0,
                             const char* passwd=0,
                             const char* host=0,
                             Int_t port=0);

      void SetBase(const char* serverbase) { fxServerName = serverbase; }
      void SetPasswd(const char* serverpasswd) { fxServerPass = serverpasswd; }
      void SetHost(const char* serverhost) { fxHostname=serverhost; }
      void SetPort(Int_t port) { fiPort=port; }

      const char* GetBase() const { return fxServerName.Data(); }
      const char* GetPasswd() const { return fxServerPass.Data(); }
      const char* GetHost() const { return fxHostname.Data(); }
      Int_t GetPort() const { return fiPort; }

   protected:

      /**  Send (command) string to server. Command may be just name of requested object*/
      void SendCommand(const char* com);

      /**  Receive root object streamed from server */
      TObject* ReceiveObject();

   private:

      /**  contains api basename */
      TString fxServerName;

      /**  contains api password */
      TString fxServerPass;

      /**  hostname of the server to connect to */
      TString fxHostname;

      /**  serversocket port number */
      Int_t fiPort;

      /**  transport socket for connection to server */
      TGo4Socket* fxTransport; //!

      /**  Set server parameters if necessary */
      void SetParms(const char* base, const char* passwd,
                    const char* host, Int_t port);

      Int_t ConnectServer();

      Int_t DisconnectServer();

      /**  Receive root buffer containing requested object */
      TBuffer* ReceiveBuffer();

   ClassDef(TGo4ObjClient,1)
};

#endif
