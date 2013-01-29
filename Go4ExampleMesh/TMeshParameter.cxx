// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TMeshParameter.h"

#include "Riostream.h"

#include "TGo4AnalysisStep.h"
#include "TGo4Analysis.h"

//***********************************************************
TMeshParameter::TMeshParameter() :
   TGo4Parameter("MeshParameter"),
   fbUnpackOn(kTRUE), fbExec1On(kTRUE), fbExec2On(kTRUE),
   fbExec3On(kTRUE),fbExec12On(kTRUE),fbCollectorOn(kTRUE)
{
}
//***********************************************************
TMeshParameter::TMeshParameter(const char* name) :
   TGo4Parameter(name),
   fbUnpackOn(kTRUE), fbExec1On(kTRUE), fbExec2On(kTRUE),
   fbExec3On(kTRUE),fbExec12On(kTRUE),fbCollectorOn(kTRUE)
{
}
//***********************************************************
TMeshParameter::~TMeshParameter()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshParameter::PrintParameter(Text_t * n, Int_t)
{
   std::cout << "Parameter " << GetName()<<":" <<std::endl;
   std::cout << " fbUnpackOn="<<fbUnpackOn<<std::endl;
   std::cout << " fbExec1On="<<fbExec1On<<std::endl;
   std::cout << " fbExec2On="<<fbExec2On<<std::endl;
   std::cout << " fbExec3On="<<fbExec3On<<std::endl;
   std::cout << " fbExec12On="<<fbExec12On<<std::endl;
   std::cout << " fbCollectorOn="<<fbCollectorOn<<std::endl;
   return 0;
}
//-----------------------------------------------------------
Bool_t TMeshParameter::UpdateFrom(TGo4Parameter *pp)
{
   if(pp->InheritsFrom("TMeshParameter"))
   {
      TMeshParameter* from = (TMeshParameter *) pp;
      fbUnpackOn=from->fbUnpackOn;
      fbExec1On=from->fbExec1On;
      fbExec2On=from->fbExec2On;
      fbExec3On=from->fbExec3On;
      fbExec12On=from->fbExec12On;
      fbCollectorOn=from->fbCollectorOn;
      std::cout <<"Updated Parameter "<<GetName() << std::endl;
      TGo4Analysis* ana=TGo4Analysis::Instance();
      TGo4AnalysisStep* unpack=ana->GetAnalysisStep("Unpack");
      if(unpack)
         unpack->SetProcessEnabled(fbUnpackOn);

      TGo4AnalysisStep* pror1=ana->GetAnalysisStep("Input1Provider");
      TGo4AnalysisStep* ex1=ana->GetAnalysisStep("Exec1");
      if(pror1 && ex1)
      {
         pror1->SetProcessEnabled(fbExec1On);
         pror1->SetSourceEnabled(!fbUnpackOn);
         ex1->SetProcessEnabled(fbExec1On);
      }
      TGo4AnalysisStep* pror2=ana->GetAnalysisStep("Input2Provider");
      TGo4AnalysisStep* ex2=ana->GetAnalysisStep("Exec2");
      if(pror2 && ex2)
      {
         pror2->SetProcessEnabled(fbExec2On);
         pror2->SetSourceEnabled(!fbUnpackOn);
         ex2->SetProcessEnabled(fbExec2On);
      }
      TGo4AnalysisStep* pror3=ana->GetAnalysisStep("Input3Provider");
      TGo4AnalysisStep* ex3=ana->GetAnalysisStep("Exec3");
      if(pror3 && ex3)
      {
         pror3->SetProcessEnabled(fbExec3On);
         pror3->SetSourceEnabled(!fbUnpackOn);
         ex3->SetProcessEnabled(fbExec3On);
      }

      TGo4AnalysisStep* pro1=ana->GetAnalysisStep("Output1Provider");
      TGo4AnalysisStep* pro2=ana->GetAnalysisStep("Output2Provider");
      TGo4AnalysisStep* pro3=ana->GetAnalysisStep("Output3Provider");
      TGo4AnalysisStep* ex12=ana->GetAnalysisStep("Exec12");
      if(pro1 && pro2 && pro3 && ex12)
      {
         pro1->SetProcessEnabled(fbExec12On | fbCollectorOn);
         pro1->SetSourceEnabled(!fbExec1On);
         pro2->SetProcessEnabled(fbExec12On | fbCollectorOn);
         pro2->SetSourceEnabled(!fbExec2On);
         pro3->SetProcessEnabled(fbCollectorOn);
         pro3->SetSourceEnabled(!fbExec3On);
         ex12->SetProcessEnabled(fbExec12On);
      }
      TGo4AnalysisStep* pro12=ana->GetAnalysisStep("Output12Provider");
      TGo4AnalysisStep* fin=ana->GetAnalysisStep("Final");
      if(pro12 && fin)
      {
         pro12->SetProcessEnabled(fbCollectorOn);
         pro12->SetSourceEnabled(!fbExec12On);
         fin->SetProcessEnabled(fbCollectorOn);

      }
      std::cout <<"Parameter "<<GetName()<<" changed steps setup" << std::endl;


   }
   else
      std::cout << "Wrong parameter object: " << pp->ClassName() << std::endl;
   return kTRUE;
}
