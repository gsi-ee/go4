#include "TGo4EventProcessor.h"

#include "Riostream.h"
#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4EventCalibration.h"
#include "TGo4AnalysisImp.h"

TGo4EventProcessor::TGo4EventProcessor(const char* name)
: TGo4EventSource(name), fxInputEvent(0), fxCalibration(0)
{
TRACE((15,"TGo4EventProcessor::TGo4EventProcessor()",__LINE__, __FILE__));
}

TGo4EventProcessor::~TGo4EventProcessor()
{
TRACE((15,"TGo4EventProcessor::~TGo4EventProcessor()",__LINE__, __FILE__));
}

Bool_t TGo4EventProcessor::CheckInputEvent(const char* classname)
{
   TRACE((12,"TGo4EventProcessor::CheckInputEvent()",__LINE__, __FILE__));
   if(fxInputEvent==0)
      {
         return kFALSE;
      }
   else
      {
         if(!strcmp(fxInputEvent->ClassName(),classname))
            {
               return kTRUE;
            }
         else
            {
               return kFALSE;
            }

     }

}

Bool_t TGo4EventProcessor::CheckCalibration(const char* classname)
{
   TRACE((12,"TGo4EventProcessor::CheckCalibration()",__LINE__, __FILE__));
   if(fxCalibration==0)
      {
         return kFALSE;
      }
   else
      {
         if(!strcmp(fxCalibration->ClassName(),classname))
            {
               return kTRUE;
            }
         else
            {
               return kFALSE;
            }
     }
}

Bool_t TGo4EventProcessor::AddObject(TNamed* any, const Text_t* subfolder)
{
   return (TGo4Analysis::Instance()->AddObject(any,subfolder));
}

Bool_t TGo4EventProcessor::AddHistogram(TH1* his, const Text_t* subfolder, Bool_t replace)
{
   return (TGo4Analysis::Instance()->AddHistogram(his,subfolder,replace));
}

Bool_t TGo4EventProcessor::AddParameter(TGo4Parameter* par,const Text_t* subfolder)
{
  return (TGo4Analysis::Instance()->AddParameter(par,subfolder));
}

Bool_t TGo4EventProcessor::AddPicture(TGo4Picture* pic,const Text_t* subfolder)
{
  return (TGo4Analysis::Instance()->AddPicture(pic,subfolder));
}

Bool_t TGo4EventProcessor::AddCanvas(TCanvas* can, const Text_t* subfolder)
{
  return (TGo4Analysis::Instance()->AddCanvas(can,subfolder));
}

Bool_t  TGo4EventProcessor::AddAnalysisCondition(TGo4Condition* con,const Text_t* subfolder)
{
  return (TGo4Analysis::Instance()->AddAnalysisCondition(con,subfolder));
}

Bool_t TGo4EventProcessor::RemoveHistogram(const Text_t* name)
{
  return (TGo4Analysis::Instance()->RemoveHistogram(name));
}

Bool_t TGo4EventProcessor::RemoveParameter(const Text_t* name)
{
  return (TGo4Analysis::Instance()->RemoveParameter(name));
}

Bool_t TGo4EventProcessor::RemovePicture(const Text_t* name)
{
  return (TGo4Analysis::Instance()->RemovePicture(name));
}

Bool_t TGo4EventProcessor::RemoveCanvas(const Text_t* name)
{
  return (TGo4Analysis::Instance()->RemoveCanvas(name));
}

Bool_t TGo4EventProcessor::RemoveAnalysisCondition(const Text_t* name)
{
  return (TGo4Analysis::Instance()->RemoveAnalysisCondition(name));
}

TNamed* TGo4EventProcessor::GetObject(const Text_t* name, const Text_t* folder)
{
   return (TGo4Analysis::Instance()->GetObject(name,folder));
}


TH1* TGo4EventProcessor::GetHistogram(const Text_t* name)
{
   return (TGo4Analysis::Instance()->GetHistogram(name));
}

TGo4Parameter* TGo4EventProcessor::GetParameter(const Text_t* name)
{
   return (TGo4Analysis::Instance()->GetParameter(name));
}

TGo4Picture* TGo4EventProcessor::GetPicture(const Text_t* name)
{
   return (TGo4Analysis::Instance()->GetPicture(name));
}

TCanvas* TGo4EventProcessor::GetCanvas(const Text_t* name)
{
   return (TGo4Analysis::Instance()->GetCanvas(name));
}

TGo4Condition*  TGo4EventProcessor::GetAnalysisCondition(const Text_t* name)
{
   return (TGo4Analysis::Instance()->GetAnalysisCondition(name));
}

TGo4EventElement* TGo4EventProcessor::GetInputEvent(const Text_t* stepname)
{
   return (TGo4Analysis::Instance()->GetInputEvent(stepname) );
}

TGo4EventElement* TGo4EventProcessor::GetOutputEvent(const Text_t* stepname)
{
   return (TGo4Analysis::Instance()->GetOutputEvent(stepname) );
}

void TGo4EventProcessor::Message(Int_t prio, const Text_t* text,...)
{
   Text_t txtbuf[__MESSAGETEXTLENGTH__];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, __MESSAGETEXTLENGTH__, text, args);
   va_end(args);
   TGo4Analysis::Instance()->Message(prio,txtbuf);
}

void TGo4EventProcessor::SendObjectToGUI(TNamed* ob)
{
   return (TGo4Analysis::Instance()->SendObjectToGUI(ob ) );
}


void TGo4EventProcessor::Clear(Option_t* opt)
{
 cout <<"Default Clear of eventprocessor "<< GetName() << endl;
 // dummy clear, may be implemented by user
}

