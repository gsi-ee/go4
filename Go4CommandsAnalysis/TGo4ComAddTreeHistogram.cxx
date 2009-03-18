#include "TGo4ComAddTreeHistogram.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComAddTreeHistogram::TGo4ComAddTreeHistogram()
:TGo4AnalysisCommand("ANAddTreeHis","Add a tree histogram to dynamic list")
{
   TRACE((12,"TGo4ComAddTreeHistogram::TGo4ComAddTreeHistogram() ctor",__LINE__, __FILE__));
   SetProtection(kGo4ComModeController);
}

TGo4ComAddTreeHistogram::TGo4ComAddTreeHistogram(const char* histogramname, const char* treename,const char* varexp, const char* cut)
:TGo4AnalysisCommand("ANAddTreeHis","Add a tree histogram to dynamic list")
{
   TRACE((12,"TGo4ComAddTreeHistogram::TGo4ComAddTreeHistogram() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // to send back status message
   SetProtection(kGo4ComModeController);
   SetHistogramName(histogramname);
   SetTreeName(treename);
   SetVarexp(varexp);
   SetCut(cut);

}

TGo4ComAddTreeHistogram::~TGo4ComAddTreeHistogram()
{
   TRACE((12,"TGo4ComAddTreeHistogram::~TGo4ComAddTreeHistogram() dtor",__LINE__, __FILE__));
}

void TGo4ComAddTreeHistogram::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
SetHistogramName(remcom->GetString(0));
SetTreeName(remcom->GetString(1));
SetVarexp(remcom->GetString(2));
SetCut(remcom->GetString(3));
}



Int_t TGo4ComAddTreeHistogram::ExeCom()
{
   TRACE((12,"TGo4ComAddTreeHistogram::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli)
      {
        TGo4Analysis* ana = TGo4Analysis::Instance();
//         TGo4Log::Debug(" !!! %s executing: adding tree histogram entry %s ",
//            GetName(),fxHistoName.GetName());
         if(ana)
            {
               TString histo = fxHistoName;
               if (histo.Length()==0) histo = "hTreeDraw";
               Bool_t result= ana->AddTreeHistogram(histo.Data(),
                                 fxTreeName.Data(),
                                 fxVarexp.Data(),
                                 fxCut.Data());
               if(result)
                  {
                     cli->SendStatusMessage(1,kTRUE,"Added Dynamic histogram %s for tree %s.",
                                                fxHistoName.Data(),
                                                fxTreeName.Data());
                  }
               else
                  {
                      cli->SendStatusMessage(2,kTRUE,"Could not add Dynamic histogram %s for tree %s !!!",
                                                      fxHistoName.Data(),
                                                      fxTreeName.Data());
                  }
            }// if ana
         else
            {
                 cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",
                                                 GetName());

            }
      } // if cli
   else
      {
      TRACE((11,"TGo4ComAddTreeHistogram::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAddTreeHistogram ''%s'': NO RECEIVER ERROR!!!",GetName());

         return 1;
      }

   return -1;
}
