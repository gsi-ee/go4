#ifndef TGO4DABC_H
#define TGO4DABC_H

class TGo4Dabc {
   public:
      TGo4Dabc() {}
      ~TGo4Dabc() {}

      static bool StartHttpServer(int port);

      static bool ConnectMaster(const char* name);
};

#endif
