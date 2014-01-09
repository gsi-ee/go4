#include "TGo4Dabc.h"

#include "TGo4Sniffer.h"
#include "THttpServer.h"

THttpServer* gGo4HttpServer = 0;

bool TGo4Dabc::StartHttpServer(int port)
{
   if (gGo4HttpServer==0) {
      gGo4HttpServer = new THttpServer("");

      gGo4HttpServer->SetSniffer(new TGo4Sniffer("go4_dabc",5));
   }

   // TODO: in the future one can create plain ROOT http engine - no dependency from DABC
   TString engine;
   engine.Form("dabc:http:%d?top=Go4&player=TGo4DabcPlayer", port);

   return gGo4HttpServer->CreateEngine(engine.Data());
}

bool TGo4Dabc::ConnectMaster(const char* master_url)
{

   if (gGo4HttpServer==0) {
      gGo4HttpServer = new THttpServer("");
      gGo4HttpServer->SetSniffer(new TGo4Sniffer("go4_dabc",5));
   }

   TString engine;
   engine.Form("dabc:%s?top=Go4&player=TGo4DabcPlayer", master_url);

   return gGo4HttpServer->CreateEngine(engine.Data());
}
