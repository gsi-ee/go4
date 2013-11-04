#include "TGo4Dabc.h"

#include "dabc/string.h"
#include "dabc/api.h"
#include "dabc/logging.h"
#include "dabc/Command.h"
#include "dabc/Url.h"
#include "dabc/Manager.h"
#include "dabc/Configuration.h"

#include "TGo4DabcSniffer.h"
#include "TGo4AnalysisImp.h"

bool TGo4Dabc::StartHttpServer(int port)
{
   if (!dabc::mgr.null()) return false;

   dabc::CreateManager("dabc", -1);

   dabc::mgr.CreateThread("MainThread");

   dabc::mgr.CreatePublisher();

   dabc::Command cmd2("any");
   cmd2.SetBool("enabled", true);
   cmd2.SetBool("batch", false);
   cmd2.SetStr("prefix", "Go4");
   TGo4DabcSniffer* sniff = new TGo4DabcSniffer("/Go4", cmd2);

   sniff->InstallSniffTimer();

   dabc::WorkerRef w2 = sniff;

   w2.MakeThreadForWorker("MainThread");

   DOUT2("Create go4 sniffer");

   dabc::CmdCreateObject cmd1("http::Server","/http");
   cmd1.SetBool("enabled", true);
   cmd1.SetInt("port", port);
   cmd1.SetStr("topname", "Go4");
   cmd1.SetStr("select", "Go4");

   if (!dabc::mgr.Execute(cmd1)) return false;

   dabc::WorkerRef w1 = cmd1.GetRef("Object");
   w1.MakeThreadForWorker("MainThread");

   DOUT2("Create http server");
   return true;

}

bool TGo4Dabc::ConnectMaster(const char* master_url)
{
   if (!dabc::mgr.null()) return false;

   if ((master_url==0) || (strlen(master_url)==0)) {
      EOUT("Master name is not specified!!!");
      return false;
   }

   dabc::CreateManager("dabc", 0);

   dabc::mgr.CreateThread("MainThread");

   dabc::mgr.CreatePublisher();

   dabc::Command cmd2("any");
   cmd2.SetBool("enabled", true);
   cmd2.SetBool("batch", false);
   cmd2.SetStr("prefix", "Go4/" + dabc::mgr.GetLocalId());
   TGo4DabcSniffer* sniff = new TGo4DabcSniffer("/Go4", cmd2);

   sniff->InstallSniffTimer();

   dabc::WorkerRef w2 = sniff;

   w2.MakeThreadForWorker("MainThread");

   DOUT2("Create go4 sniffer");

   DOUT0("Create command channel for %s ", master_url);

   dabc::Command cmd("ConfigureMaster");
   cmd.SetStr("Master", master_url);
   cmd.SetStr("NameSufix", "Go4");
   if (dabc::mgr.GetCommandChannel().Execute(cmd) != dabc::cmd_true) {
      DOUT0("FAIL to activate connection to master %s", master_url);
      return false;
   }

   DOUT0("Master %s configured !!!", master_url);

   return true;


}
