#include "TGo4AbstractInterface.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "Riostream.h"
#include "TObjString.h"

#include "TGo4Log.h"
#include "TGo4Iter.h"
#include "TGo4Slot.h"
#include "TGo4ObjectManager.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"

TGo4AbstractInterface* TGo4AbstractInterface::fgInstance = 0;

TGo4AbstractInterface* TGo4AbstractInterface::Instance()
{
   return fgInstance;
}

void TGo4AbstractInterface::DeleteInstance()
{
   if (fgInstance!=0) delete fgInstance;
}

TGo4AbstractInterface::TGo4AbstractInterface() :
   TObject(),
   fOM(0),
   fBrowser(0),
   fxCommands()
{
   fgInstance = this;
}

void TGo4AbstractInterface::Initialize(TGo4ObjectManager* om, TGo4BrowserProxy* br)
{
   fOM = om;
   fBrowser = br;

   Int_t error = 0;
   gROOT->ProcessLine("TGo4AbstractInterface* go4 = TGo4AbstractInterface::Instance();", &error);
   gROOT->ProcessLine("TGo4ObjectManager* om = TGo4AbstractInterface::Instance()->OM();", &error);
   gROOT->ProcessLine("TGo4BrowserProxy* br = TGo4AbstractInterface::Instance()->Browser();", &error);
   gROOT->ProcessLine(".x $GO4SYS/etc/go4macroinit.C");
}

TGo4AbstractInterface::~TGo4AbstractInterface()
{
   FreeHotStartCmds();

   gROOT->ProcessLine(".x $GO4SYS/etc/go4macroclose.C");

   gInterpreter->DeleteGlobal(fBrowser);
   gInterpreter->DeleteGlobal(fOM);
   gInterpreter->DeleteGlobal(this);

   fgInstance = 0;

}

TGo4AnalysisProxy* TGo4AbstractInterface::Analysis()
{
   return Browser() ? Browser()->FindAnalysis() : 0;
}

void TGo4AbstractInterface::LoadLibrary(const char* fname)
{
   if ((fname==0) || (strlen(fname)==0)) return;

   TString libs = gInterpreter->GetSharedLibs();

   const char* token = strtok((char*) libs.Data(), " ,\t\n");

   while(token != 0) {
      if (strcmp(token, fname)==0) return;
      token = strtok(NULL, " ,\t\n");
   }

   gSystem->Load(fname);
}

void TGo4AbstractInterface::OpenFile(const char* fname)
{
   Browser()->OpenFile(fname);
   ProcessEvents(-1);
}

Bool_t TGo4AbstractInterface::SaveToFile(const char* itemname,
                                         const char* filename,
                                         const char* filetitle)
{
    return Browser()->SaveBrowserToFile(filename, kFALSE, itemname, filetitle);
}

Bool_t TGo4AbstractInterface::ExportToFile(const char* itemname,
                                           const char* dirpath,
                                           const char* format,
                                           const char* filetitle)
{
    TGo4Slot* topslot = Browser()->ItemSlot(itemname);
    if (topslot==0) return kFALSE;

    TObjArray items;

    TGo4Iter iter(topslot, kTRUE);

    TString buf;

    while (iter.next())
       if (Browser()->BrowserItemName(iter.getslot(), buf))
          items.Add(new TObjString(buf));

    Browser()->ExportItemsTo(&items, kFALSE, "null", dirpath, format, filetitle);

    items.Delete();

    return kTRUE;
}


void TGo4AbstractInterface::ConnectHServer(const char* servername,
                                       Int_t portnumber,
                                       const char* basename,
                                       const char* userpass,
                                       const char* filter)
{
   Browser()->ConnectHServer(servername,
                             portnumber,
                             basename,
                             userpass,
                             filter);
   ProcessEvents(200);
}

Bool_t TGo4AbstractInterface::IsAnalysisConnected()
{
   return Analysis()==0 ? kFALSE : Analysis()->IsConnected();
}


void TGo4AbstractInterface::ExecuteLine(const char* remotecmd)
{
   TGo4AnalysisProxy* anal = Analysis();
   if ((anal!=0) && (remotecmd!=0)) {
      anal->ExecuteLine(remotecmd);
      TGo4Log::Message(1, "Exec: %s", remotecmd);
   }
}

void TGo4AbstractInterface::RequestAnalysisConfig()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal!=0)
     anal->RequestAnalysisSettings();
}

void TGo4AbstractInterface::MonitorItem(const char* itemname, Bool_t on)
{
   TGo4Slot* itemslot = Browser()->BrowserSlot(itemname);
   if (itemslot!=0)
     Browser()->SetItemMonitored(itemslot, on);
}

void TGo4AbstractInterface::StartMonitoring(Int_t period)
{
   Browser()->ToggleMonitoring(period*1000);
}

void TGo4AbstractInterface::StopMonitoring()
{
   Browser()->ToggleMonitoring(-1);
}

TString TGo4AbstractInterface::FindItem(const char* objname)
{
   // if itemname is specified, return as is
   if (Browser()->ItemSlot(objname)!=0) return TString(objname);

   return Browser()->FindItem(objname);
}

Bool_t TGo4AbstractInterface::CopyItem(const char* itemname)
{
   return Browser()->ProduceExplicitCopy(itemname);
}

Bool_t TGo4AbstractInterface::DeleteItem(const char* itemname)
{
   return Browser()->DeleteDataSource(Browser()->ItemSlot(itemname));
}

void TGo4AbstractInterface::FetchItem(const char* itemname, Int_t wait_time)
{
   Browser()->FetchItem(itemname, wait_time);
}

void TGo4AbstractInterface::RedrawItem(const char* itemname)
{
   Browser()->RedrawItem(itemname);
}

TObject* TGo4AbstractInterface::GetObject(const char* itemname, Int_t updatelevel)
{
   if ((itemname==0) || (strlen(itemname)==0)) return 0;

   return Browser()->GetBrowserObject(itemname, updatelevel);
}

TString TGo4AbstractInterface::SaveToMemory(const char* path, TObject* obj, Bool_t ownership)
{
   TString res = Browser()->SaveToMemory(path, obj, ownership, kTRUE);

   ProcessEvents();

   return res;
}

Bool_t TGo4AbstractInterface::LoadHotStart(const char* filename)
{
   FreeHotStartCmds();

   ifstream f(filename);

   char buf[10000];

   while (!f.eof()) {
      f.getline(buf, 10000);
      fxCommands.Add(new TObjString(buf));
   }

   if (fxCommands.IsEmpty()) return kFALSE;

   // Add empty string, which will be deleted first
   fxCommands.AddFirst(new TObjString(""));
   return kTRUE;
}

Bool_t TGo4AbstractInterface::IsHotStart()
{
   return !fxCommands.IsEmpty();
}

const char* TGo4AbstractInterface::NextHotStartCmd()
{
   if (fxCommands.IsEmpty()) return 0;
   TObject* obj = fxCommands.First();
   fxCommands.Remove(obj);
   delete obj;
   const char* res = 0;
   do {
      if (fxCommands.IsEmpty()) return 0;
      TObjString* str = (TObjString*) fxCommands.First();
      res = str->GetName();
      if ((res==0) || (strlen(res)==0)) {
         res = 0;
         fxCommands.Remove(str);
         delete str;
      }

   } while (res == 0);
   return res;
}

void TGo4AbstractInterface::FreeHotStartCmds()
{
   fxCommands.Delete();
}

