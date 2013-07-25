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

#include "TGo4MbsHist.h"

#include <string.h>

#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TList.h"
#include "TFolder.h"
#include "TTimeStamp.h"
#include "TRegexp.h"
#include "snprintf.h"

#include "TGo4ThreadManager.h"
#include "TGo4AnalysisObjectManager.h"

const Int_t TGo4MbsHist::fgiLISTLEN=512;

TGo4MbsHist::TGo4MbsHist() :
   TObject(),
   fiBufLen(128),
   fiBuffer(0),
   fxCursor(0),
   fiHisNum(0)
{
   fiBuffer=new Int_t[fiBufLen];
}

TGo4MbsHist::TGo4MbsHist(TH1* histo) :
   TObject(),
   fiBufLen(1),
   fiBuffer(0),
   fxCursor(0),
   fiHisNum(1)
{
if(histo)
   {
      PrepareHeader(histo,0,&fxHistoHead);
      if(histo->GetDimension()==2)
         {
            // two dim histo
            Int_t i=0;
            Stat_t value=0;

            Int_t maxu=histo->GetNbinsX();
            Int_t maxv=histo->GetNbinsY();
            fiBufLen=(maxu)*(maxv); // note: we ignore over + underflow bins
            fiBuffer=new Int_t[fiBufLen];
            // test: we swap x and y loops
            for (Int_t v = 0;v<maxv; v++)
               {
                  for (Int_t u = 0; u<maxu; u++)
                        {
                           value=histo->GetBinContent(u,v);
                           SetValue((char*) &fiBuffer[i++], value);
                        }
               } // for
         }
      else
          {
            // one dim histo and all others
            Int_t maxu=histo->GetNbinsX();
            fiBufLen=maxu; // note: we ignore over + underflow bins
            fiBuffer=new Int_t[fiBufLen];
            Stat_t value=0;
            for (Int_t u = 0; u<maxu;u++)
               {
                  value=histo->GetBinContent(u);
                  SetValue((char*) &fiBuffer[u], value);
               }
         } // if(histo->GetDimension()))
      }
else
   {
     // case of dummy object!
     fiBuffer=new Int_t[fiBufLen];
   }    // if(histo)

}

TGo4MbsHist::TGo4MbsHist(TFolder* folder, const char* filter) :
   TObject(),
   fiBufLen(fgiLISTLEN),
   fiBuffer(0),
   fxCursor(0),
   fiHisNum(0)
{
   // allocate buffer of total size
   //std::cout <<"Init buflen with "<< fiBufLen << std::endl;
   fiBuffer=new Int_t[fiBufLen];
   fxCursor= (s_his_head*) fiBuffer;
   ScanGo4Folder(folder,0,filter);
}


TGo4MbsHist::~TGo4MbsHist()
{
   delete [] fiBuffer;
}

void TGo4MbsHist::PrepareHeader(TH1* source, const char* path, s_his_head* target)
{
   if(source==0 || target==0) return;
   //std::cout <<"MMMMMMMM Preparing header for histogram "<< source->GetName() << std::endl;
   s_his_head* dest=target; // use local pointer to avoid changes by snprintf
   dest->l_bins_1=source->GetNbinsX();
   dest->l_bins_2=source->GetNbinsY();
   //std::cout <<" \tMMMMMMMM Prepare header - bins1="<< dest->l_bins_1 << std::endl;
   //std::cout <<"\tMMMMMMMM Prepare header - bins2="<< dest->l_bins_2 << std::endl;
// display of path enabled again to test Origin problems JA
   if(path)
      snprintf(dest->c_name,64,"%s%s",path,source->GetName());
   else
      snprintf(dest->c_name,64,"%s",source->GetName());
   snprintf(dest->c_lettering_1,64,"%s",source->GetXaxis()->GetTitle());
   snprintf(dest->c_lettering_2,64,"%s",source->GetYaxis()->GetTitle());
   snprintf(dest->c_lettering_res,64,"%s",source->GetYaxis()->GetTitle());
   dest->r_limits_low=source->GetXaxis()->GetXmin();
   dest->r_limits_up=source->GetXaxis()->GetXmax();
   dest->r_limits_low_2=source->GetYaxis()->GetXmin();
   dest->r_limits_up_2=source->GetYaxis()->GetXmax();
   //std::cout <<" \tMMMMMMMM Prepare header - 1low="<< dest->r_limits_low << std::endl;
   //std::cout <<"\tMMMMMMMM Prepare header - 1up ="<< dest->r_limits_up << std::endl;
   //std::cout <<" \tMMMMMMMM Prepare header - 2low="<< dest->r_limits_low_2 << std::endl;
   //std::cout <<"\tMMMMMMMM Prepare header - 2up ="<< dest->r_limits_up_2 << std::endl;

   if(source->InheritsFrom(TH1D::Class()) ||
      source->InheritsFrom(TH1F::Class()) ||
      source->InheritsFrom(TH2D::Class()) ||
      source->InheritsFrom(TH2F::Class())) {
         //std::cout <<" \tMMMMMMMM Prepare header has float histo" << std::endl;
         strcpy(dest->c_dtype,"r");
      }
   else
   {
      //std::cout <<" \tMMMMMMMM Prepare header has int histo" << std::endl;
      strcpy(dest->c_dtype,"i");
   }
   TTimeStamp now;
   snprintf(dest->c_data_time_cre,28,"%s",now.AsString("l"));    // creation time
   snprintf(dest->c_clear_date,28,"%s",now.AsString("l"));       // clear time, dummy here

}


void TGo4MbsHist::SetValue(char* address, Stat_t value)
{
if(!strcmp(fxHistoHead.c_dtype,"r"))
   {
      Float_t* faddress=(Float_t*) address;
      *faddress= (Float_t) value;
   }
else
   {
      Int_t* iaddress=(Int_t*) address;
      *iaddress= (Int_t) value;
   }
}


void TGo4MbsHist::ScanGo4Folder(TFolder* folder, const char* superfolders, const char* filter)
{
  // scan folder for histogram status objects
// if(filter)
//    std::cout <<"ScanGo4Folder with filter "<<filter << std::endl;
// else
//    std::cout <<"ScanGo4Folder with no filter "<< std::endl;
//

   if(folder==0) return;

   TIter iter(folder->GetListOfFolders());
   TObject* entry = 0;
   while((entry = iter()) !=0) {
       char pathbuffer[TGo4ThreadManager::fguTEXTLENGTH];
       Int_t num=0;
       if(superfolders)
          num=snprintf(pathbuffer,TGo4ThreadManager::fguTEXTLENGTH,"%s/",superfolders);
       else
//                  num=snprintf(pathbuffer,TGo4ThreadManager::fguTEXTLENGTH,"");
          strcpy(pathbuffer,"");
       char* cursor=pathbuffer + num;
       Int_t edge=TGo4ThreadManager::fguTEXTLENGTH-num;
       if(entry->InheritsFrom(TFolder::Class()))
          {
              // found subfolder, process it recursively
              const char* foldname=entry->GetName();
              // filter out folders without histograms or graphs:
              if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcDYNFOLDER)){;}
              else if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcCONDFOLDER)){;}
              else if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcPARAFOLDER)){;}
              else if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcTREEFOLDER)){;}
              else if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcPICTFOLDER)){;}
              else if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcCANVFOLDER)){;}
              else if(!strcmp(foldname,TGo4AnalysisObjectManager::fgcANALYSISFOLDER)){;}
              else
                 {
                    //std::cout <<"##### parsing folder "<< foldname << std::endl;
                    snprintf(cursor,edge,"%s",foldname);
                    TFolder* subobj= dynamic_cast<TFolder*> (entry);
                    ScanGo4Folder(subobj,pathbuffer, filter);
                 }
          }
        else if (entry->InheritsFrom(TH1::Class()) || entry->InheritsFrom(TGraph::Class()))
           {
              // first check if filter is ok:
              Bool_t ismatching=kFALSE;
              const char* entryname=entry->GetName();
              TString entrystring=entryname;
              TRegexp reg(filter,kTRUE);
              if(filter==0)
                 ismatching=kTRUE; // no filter set, take all
              else if(!strcmp(filter,"*"))
                 ismatching=kTRUE; // take all in this folder
//                      else if (strstr(filter,entryname))
//                       ismatching=kTRUE; // expression just contained in name
              else if (entrystring.Index(reg,0)!=kNPOS)
                ismatching=kTRUE; // root regular expression class
              else
                ismatching=kFALSE;

              if(ismatching)
                 {
                 //std::cout <<"found matching:" << entryname << std::endl;
                 TH1* hist= dynamic_cast<TH1*> (entry);
                 if(hist==0)
                    {
                        TGraph* graf=dynamic_cast<TGraph*> (entry);
                        if(graf && graf->GetN()>0)
                           {
                              hist=graf->GetHistogram();
                           }
                        if(hist==0) continue;
                    } // if(hist==0)
                 PrepareHeader(hist,pathbuffer,fxCursor);
                 fxCursor++;
                 fiHisNum++;
                 //std::cout <<"MMMMMMMM found histogram "<< entry->GetName()<<" cr="<<(Int_t) fxCursor<< std::endl;
                 if( (char*) (fxCursor) > (char*)(fiBuffer)+fiBufLen*sizeof(Int_t)- sizeof(s_his_head))
                       {
                     //std::cout <<"MMMMMMM Realloc buffer cursor "<< (Int_t) fxCursor << std::endl;
                     Int_t oldbuflen=fiBufLen;
                     fiBufLen+=fgiLISTLEN;
                     Int_t* otherbuf= new Int_t[fiBufLen];
                     memcpy(otherbuf, fiBuffer,oldbuflen*sizeof(Int_t) );
                     delete[] fiBuffer;
                     fiBuffer=otherbuf;
                     fxCursor= (s_his_head*) fiBuffer;
                     for(Int_t n=0; n< fiHisNum; ++n) fxCursor++; // set cursor in new buffer
//                          std::cout <<"MMMMMMM Copied buffer, cursor set "<< std::endl;
//                          std::cout <<" hisnum="<< (Int_t) fiHisNum<< std::endl;
//                          std::cout <<" newcr="<< (Int_t) fxCursor<< std::endl;
//                          std::cout <<" buffer="<< (Int_t) fiBuffer<< std::endl;
                    } else {}
              }//if(ismatching)
           }
    } // while
}
