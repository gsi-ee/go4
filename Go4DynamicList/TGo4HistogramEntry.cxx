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

#include "TGo4HistogramEntry.h"

#include "RVersion.h"
#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4Condition.h"
#include "TGo4DynamicListException.h"

const char* TGo4HistogramEntry::fgcNOCONDITION = "No Condition";
const char* TGo4HistogramEntry::fgcNODATA = "No Data";
const char* TGo4HistogramEntry::fgcNOEVENT = "No Event";

TGo4HistogramEntry::TGo4HistogramEntry() :
   TGo4DynamicEntry(),
   fxHistogram(0),
   fxCondition(0),
   fbNeedInitialisation(kTRUE)
{
}

TGo4HistogramEntry::TGo4HistogramEntry(const char* name) :
   TGo4DynamicEntry(name),
   fxHistogram(0),
   fxCondition(0),
   fbNeedInitialisation(kTRUE)
{
   fxHistogramName=fgcNOCONDITION;
   for (UInt_t t=0; t<__MAXHISDIM__;++t) {
      SetHisEventName(t,fgcNOEVENT);
      SetHisVarName(t,fgcNODATA);
   }

   fxConditionName = fgcNOCONDITION;
   for (Int_t t=0; t<__MAXCONDIM__;++t) {
      SetConEventName(t,fgcNOEVENT);
      SetConVarName(t,fgcNODATA);
   }

   Reset();
}

TGo4HistogramEntry::~TGo4HistogramEntry()
{
}

void TGo4HistogramEntry::SetHisVarName(Int_t ix, const char* name)
{
  if((ix>=0) && (ix<__MAXHISDIM__))
     fxHisVarName[ix]=name;
}

const char* TGo4HistogramEntry::GetHistVarName(Int_t ix) const
{
   return ((ix>=0) && (ix<__MAXHISDIM__)) ? fxHisVarName[ix].Data() : 0;
}

void TGo4HistogramEntry::SetHisEventName(Int_t ix, const char* name)
{
   if((ix>=0) && (ix<__MAXHISDIM__))
      fxHisEventName[ix]=name;
}

const char* TGo4HistogramEntry::GetHistEventName(Int_t ix) const
{
   return ((ix>=0) && (ix<__MAXHISDIM__)) ? fxHisEventName[ix].Data() : 0;
}

void TGo4HistogramEntry::SetConVarName(Int_t ix, const char* name)
{
  TRACE((12,"TGo4DynamicEntry::SetConVarName(UInt_t, char*)",__LINE__, __FILE__));
  if((ix>=0) && (ix<__MAXCONDIM__))
     fxConVarName[ix]=name;
}

const char* TGo4HistogramEntry::GetConVarName(Int_t ix) const
{
   return ((ix>=0) && (ix<__MAXCONDIM__)) ? fxConVarName[ix].Data() : 0;
}

void TGo4HistogramEntry::SetConEventName(Int_t ix, const char* name)
{
   TRACE((12,"TGo4DynamicEntry::SetConEventName(UInt_t, char*)",__LINE__, __FILE__));
   if((ix>=0) && (ix<__MAXCONDIM__))
      fxConEventName[ix]=name;
}

const char* TGo4HistogramEntry::GetConEventName(Int_t ix) const
{
   return ((ix>=0) && (ix<__MAXCONDIM__)) ? fxConEventName[ix].Data() : 0;
}


void TGo4HistogramEntry::Reset()
{
   fxHistogram=0;
   for(Int_t t =0; t<__MAXHISDIM__; ++t) {
      fxHisEvents[t] = 0;
      fxHistType[t] = 0;
      fxHistPtr[t] = 0;
   }

   fxCondition = 0;
   for(Int_t t =0; t<__MAXCONDIM__; ++t) {
      fxConEvents[t] = 0;
      fxCondType[t] = 0;
      fxCondPtr[t] = 0;
   }

   fbNeedInitialisation = kTRUE;
}

void TGo4HistogramEntry::InitHistPointer(Int_t ix, TObject* event, TDataMember* member, Long_t offset)
{
   if ((ix<0) || (ix>=__MAXHISDIM__)) return;
   fxHistPtr[ix] = 0;
   fxHisEvents[ix] = event;
   if ((event==0) || (member==0) || !member->IsBasic()) return;
   fxHistPtr[ix] = (char*) event+ offset;
   fxHistType[ix] = member->GetDataType()->GetType();
}

void TGo4HistogramEntry::InitCondPointer(Int_t ix, TObject* event, TDataMember* member, Long_t offset)
{
   if ((ix<0) || (ix>=__MAXCONDIM__)) return;
   fxCondPtr[ix] = 0;
   fxConEvents[ix] = event;
   if ((event==0) || (member==0) || !member->IsBasic()) return;
   fxCondPtr[ix] = (char*) event+ offset;
   fxCondType[ix] = member->GetDataType()->GetType();
}

Double_t TGo4HistogramEntry::GetPtrValue(Int_t type, void* ptr)
{
   if (ptr==0) return 0.;
   switch (type) {
       case kUInt_t:     return *((UInt_t*)ptr);
       case kInt_t:      return *((Int_t*)ptr);
       case kULong_t:    return *((ULong_t*)ptr);
       case kLong_t:     return *((Long_t*)ptr);
       case kULong64_t:  return *((ULong64_t*)ptr);
       case kLong64_t:   return *((Long64_t*)ptr);
       case kUShort_t:   return *((UShort_t*)ptr);
       case kShort_t:    return *((Short_t*)ptr);
       case kUChar_t:    return *((UChar_t*)ptr);
       case kChar_t:     return *((Char_t*)ptr);
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,3,2)
       case kBool_t:     return *((Bool_t*)ptr);
#endif
       case kFloat_t:    return *((Float_t*)ptr);
       case kDouble_t:   return *((Double_t*)ptr);
       case kDouble32_t: return *((Double32_t*)ptr);
   }
   return 0.;
}

Bool_t TGo4HistogramEntry::TestConditionNew()
{
   if (fxCondition==0) return kTRUE;

   Bool_t rev = kTRUE;

   Double_t dat[__MAXCONDIM__];

   UInt_t j,dimension;
   // find out how many dimensions are set for condition:
   // maximum condition dimension defined by first index
   // with all data pointers set to zero.
   for(dimension=0;dimension<__MAXCONDIM__; ++dimension)
      if (fxCondPtr[dimension]==0) break;

   // fill pointers to test variables:
   for(j=0;j<dimension; ++j)
      dat[j] = GetPtrValue(fxCondType[j], fxCondPtr[j]);

   switch(dimension) {
//         case 0:
//            rev = fxCondition->GetLast(); // no own variables: last result
//            //cout << "found zero dimension for condition" << endl;
//            break;
         case 1:
            rev=fxCondition->Test(dat[0]);
            //cout << "found one dimension for condition" << endl;
            break;
         case 2:
            rev=fxCondition->Test(dat[0], dat[1]);
            //cout << "found two dimensions for condition" << endl;
            break;
         default:
            rev = kTRUE;
            break;
      } // switch(maxt)

   return rev;
}

void TGo4HistogramEntry::ProcessNew(Bool_t* evvalid)
{
   if (fxHistogram==0) return;

   if (!TestConditionNew()) return;

   Double_t dat[3];
   Int_t dimension = fxHistogram->GetDimension();
   for(Int_t j=0; j<dimension; ++j)
     dat[j] = GetPtrValue(fxHistType[j], fxHistPtr[j]);
   switch (dimension) {
     case 1:
        if(evvalid[0]) fxHistogram->Fill(dat[0]);
        break;
     case 2:
        if(evvalid[0] && evvalid[1]) {
           TH2* his2 = dynamic_cast<TH2*>(fxHistogram);
           if(his2!=0) his2->Fill(dat[0],dat[1]);
        }
        break;
     case 3:
        if(evvalid[0] && evvalid[1] && evvalid[2]) {
           TH3* his3 = dynamic_cast<TH3*>(fxHistogram);
           if(his3!=0) his3->Fill(dat[0],dat[1],dat[2]);
        }
        break;
     default:
        throw TGo4DynamicListException(this,
          "Dynamic Histogram Entry %s Process error: Wrong histogram dimension %d !!!",
                     GetName(), dimension);
  } // switch(dimension)
}

void TGo4HistogramEntry::RecursiveRemove(TObject* obj)
{
   if (obj==0) return;

   if(fxHistogram==obj) Reset();

   if(fxCondition==obj) Reset();

   for (Int_t n=0;n<__MAXCONDIM__;n++)
     if (fxConEvents[n]==obj) Reset();

   for (Int_t n=0;n<__MAXHISDIM__;n++)
     if (fxHisEvents[n]==obj) Reset();
}

void TGo4HistogramEntry::Print(Option_t* dummy) const
{
   // this trick is needed since root defines Print as const function...
   TROOT::IndentLevel();
   cout <<"-Dynamic Histogram Entry " << GetName()<<" :"<<endl;
   TROOT::IncreaseDirLevel();
   TROOT::IndentLevel();

   if(IsEnabledProcessing()) cout <<"\t-- Enabled --" << endl;
                        else cout <<"\t-- Disabled --" << endl;

   TROOT::IndentLevel();
   cout <<"\tCondition: "<< GetConditionName() << endl;
   for(Int_t i=0;i<__MAXCONDIM__; ++i) {
      TROOT::IndentLevel();
      cout << "\t ConEvent(" << i << "): " << GetConEventName(i)
           << "\t ConVar(" << i << "): " << GetConVarName(i) << endl;
   }
   TROOT::IndentLevel();
   cout <<"\tHistogram: "<< GetHistogramName() << endl;
   for(Int_t i=0;i<__MAXHISDIM__; ++i) {
      TROOT::IndentLevel();
      cout << "\t HisEvent(" << i<<"): " << GetHistEventName(i)
           << "\t HisVar(" << i<< "): " << GetHistVarName(i) << endl;
   }
   TROOT::DecreaseDirLevel();
   TROOT::IndentLevel();
   cout <<"-End "<<GetName()<<"-----------" << endl;
}

const char* TGo4HistogramEntry::Get_fgcNOCONDITION()
{
   return fgcNOCONDITION;
}

const char* TGo4HistogramEntry::Get_fgcNODATA()
{
   return fgcNODATA;
}

const char* TGo4HistogramEntry::Get_fgcNOEVENT()
{
   return fgcNOEVENT;
}

