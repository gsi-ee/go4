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

#include "TGo4ExportManager.h"

#include <stdexcept>

#include "Riostream.h"
#include "TSystem.h"
#include "TFolder.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"

#include "TGo4Log.h"

TGo4ExportManager::TGo4ExportManager(const char* name)
   : TNamed(name,"This is a Go4 export manager"),fiFilter(GO4EX_ROOT)
{
  fxOutFile="Go4Export";
}

TGo4ExportManager::TGo4ExportManager()
   :fiFilter(GO4EX_ROOT)
{
}

TGo4ExportManager::~TGo4ExportManager()
{
}

void TGo4ExportManager::SetFilter(Go4Export_t format)
{
   fiFilter = format;
}

void TGo4ExportManager::SetCurrentDir(const char* dir)
{
  if(dir)
      {
         fxCurrentDir=dir;
         gSystem->cd(dir);
      }
  else
      {
          fxCurrentDir=gSystem->WorkingDirectory();
      }
}

void TGo4ExportManager::SetStartDir(const char* dir)
{
     if(dir)
         fxStartDir=dir;
     else
         fxStartDir=gSystem->WorkingDirectory();
}

void TGo4ExportManager::Export(TObject* ob, Go4Export_t format)
{
if(ob==0) return;
SetFilter(format);
Export(ob);

}


void TGo4ExportManager::Export(TObject* myobject)
{
  if(myobject==0) return;
  if(myobject->InheritsFrom(TDirectory::Class()))
    {
      TDirectory* subdir=dynamic_cast<TDirectory*>(myobject);
      Export(subdir);
    }
  else if (myobject->InheritsFrom(TFolder::Class()))
    {
      TFolder* subfold=dynamic_cast<TFolder*> (myobject);
      Export(subfold);
    }
  else if (myobject->InheritsFrom(TCollection::Class()))
    {
      TCollection* col=dynamic_cast<TCollection*> (myobject);
      Export(col);
    }
  else if(myobject->InheritsFrom(TH1::Class()))
    {
      TH1* histo= dynamic_cast<TH1*>(myobject);
      Export(histo);
    }
  else if (myobject->InheritsFrom(TGraph::Class()))
    {
      TGraph* graph= dynamic_cast<TGraph*>(myobject);
      Export(graph);
    }
  else
    switch(fiFilter) {
      case GO4EX_ROOT:
         ExportRoot(myobject);
         break;
      case GO4EX_XML:
         ExportXML(myobject);
         break;
      default:
         TGo4Log::Message(2,"ExportManager: NOT Converting object %s of class %s",
           myobject->GetName(),myobject->ClassName());
    }
}


void TGo4ExportManager::Export(TFolder* fold)
{
if(fold==0) return;
TGo4Log::Message(0,"ExportManager: Converting contents of folder %s",fold->GetName());
if(fiFilter==GO4EX_ROOT)
   {
       // root filter will write collection completely into one root file
       // otherwise, each object would be written separately into flat hierarchy
       ExportRoot(fold);
       return;
   }

TString dirname=fold->GetName();
gSystem->cd(fxCurrentDir.Data()); // create subdirectory in file system
TString com="mkdir "+dirname;
gSystem->Exec(com);
gSystem->cd(dirname.Data());
fxCurrentDir=gSystem->WorkingDirectory();
TCollection* folderlist=fold->GetListOfFolders();
Export(folderlist);
gSystem->cd(fxCurrentDir.Data());
gSystem->cd("..");
fxCurrentDir=gSystem->WorkingDirectory(); // go up one directory level again
gSystem->cd(fxStartDir.Data());

}

void TGo4ExportManager::Export(TDirectory* source)
{
if(source==0) return;
TGo4Log::Message(0,"ExportManager: Converting contents of directory %s",source->GetName());
if(fiFilter==GO4EX_ROOT)
   {
       // root filter will write collection completely into one root file
       // otherwise, each object would be written separately into flat hierarchy
       ExportRoot(source);
       return;
   }

TString dirname=source->GetName();
if(!dirname.Contains(".root"))
{
  gSystem->cd(fxCurrentDir.Data()); // create subdirectory in file system
  TString com="mkdir "+dirname;
  gSystem->Exec(com);
  gSystem->cd(dirname.Data());
  fxCurrentDir=gSystem->WorkingDirectory();
}
TObject* myobject=0;
TKey* mykey=0;
source->cd();
gSystem->cd(fxStartDir.Data());
TIter iter(source->GetListOfKeys());
while((mykey=(TKey*) iter())!=0)
  {
    myobject= mykey->ReadObj();
    if(myobject)
      {
        Export(myobject);
      }
    else
      {
        TGo4Log::Message(2,"ExportManager: Could not read key %s", mykey->GetName());
      }
  } // while
if(!dirname.Contains(".root"))
   {
     gSystem->cd(fxCurrentDir.Data());
     gSystem->cd("..");
     fxCurrentDir=gSystem->WorkingDirectory(); // go up one directory level again
     gSystem->cd(fxStartDir.Data());
   }
}

void TGo4ExportManager::Export(TCollection* col)
{
  if(col==0) return;
  TGo4Log::Message(0,"ExportManager: Converting contents of collection %s",col->GetName());
  if(fiFilter==GO4EX_ROOT) {
     // root filter will write collection completely into one root file
     // otherwise, each object would be written separately into flat hierarchy
     ExportRoot(col);
     return;
  }

   TIter iter(col);
   TObject* ob=0;
   while((ob=iter())!=0)
       Export(ob);
}

void TGo4ExportManager::Export(TH1* histo)
{
switch(fiFilter)
   {
      case GO4EX_ASCII:
         ExportASCII(histo,kFALSE);
         break;
      case GO4EX_ASCII_CHANNELS:
         ExportASCII(histo,kTRUE);
         break;
      case GO4EX_RADWARE:
         ExportRadware(histo);
         break;
      case GO4EX_ROOT:
         ExportRoot(histo);
         break;
      case GO4EX_XML:
         ExportXML(histo);
         break;
      default:
         TGo4Log::Message(2,"ExportManager: Unknown Export filter %d!",fiFilter);
         break;
   };

}

void TGo4ExportManager::Export(TGraph* gra)
{
switch(fiFilter)
   {
      case GO4EX_ASCII:
      case GO4EX_ASCII_CHANNELS:
         ExportASCII(gra);
         break;
      case GO4EX_RADWARE:
         ExportRadware(gra);
         break;
      case GO4EX_XML:
         ExportXML(gra);
         break;
      case GO4EX_ROOT:
         ExportRoot(gra);
         break;
      default:
         TGo4Log::Message(2,"ExportManager: Unknown Export filter %d!",fiFilter);
         break;
   };

}

void TGo4ExportManager::ExportASCII(TH1* histo, Bool_t channels)
{
if(histo==0) return;
try{
   TString objectname=histo->GetName();
   TString outname=objectname;
   outname.Append(".hdat");
   gSystem->cd(fxCurrentDir.Data());

   std::ofstream outfile(outname.Data());
   if(!outfile)
    {
      TGo4Log::Message(3,"ExportManager: Error opening outputfile %s",outname.Data());
      //cout <<"Error opening outputfile "<<outname.Data() << endl;
      return;
    }
   TGo4Log::Message(0,"ExportManager: Converting histogram %s to ASCII",histo->GetName());
   //cout <<"Converting histogram "<< histo->GetName() << endl;
   Int_t maxbinX=histo->GetNbinsX();
   Int_t maxbinY=histo->GetNbinsY();
   Int_t maxbinZ=histo->GetNbinsZ();
   Int_t globalbin=0;
   Stat_t  cont=0;
   outfile <<"# Histogram "<<histo->ClassName() <<": "<<histo->GetName()<<endl;
   if(channels)
      outfile <<"# Xbin \tYbin \tZbin \tContent"<<endl;
   else
      outfile <<"# X \tY \tZ \tContent"<<endl;
   for(Int_t x=1; x<=maxbinX; ++x)
    {
      for(Int_t y=1; y<=maxbinY; ++y)
        {
          for(Int_t z=1; z<=maxbinZ; ++z)
            {
              globalbin=histo->GetBin(x,y,z);
              cont=histo->GetBinContent(globalbin);
              if(channels)
               {
                  outfile <<x<<" \t"<<y<<" \t"<<z<<" \t"<<cont<<endl;
               }
              else
               {
                  Axis_t xval=0;
                  TAxis* xax=histo->GetXaxis();
                  //if(xax) xval=xax->GetBinCenter(x);
                  if(xax) xval=xax->GetBinLowEdge(x);
                  Axis_t yval=0;
                  TAxis* yax=histo->GetYaxis();
                  //if(yax) yval=yax->GetBinCenter(y);
                  if(yax) yval=yax->GetBinLowEdge(y);
                  Axis_t zval=0;
                  TAxis* zax=histo->GetZaxis();
                  //if(zax) zval=zax->GetBinCenter(z);
                  if(zax) zval=zax->GetBinLowEdge(z);
                  outfile <<xval<<" \t"<<yval<<" \t"<<zval<<" \t"<<cont<<endl;
               }
            }
        }

    }
   outfile.close();
   gSystem->cd(fxStartDir.Data()); // needed in case of TKey read from (file) directory
}// try
catch(std::exception& ex) // treat standard library exceptions
{
  TGo4Log::Message(3,"standard exception %s in TGo4ExportManager::ExportASCII(TH1*)",
            ex.what());
}
catch(...)
{
  TGo4Log::Message(3,"!!! Unexpected exception in TGo4ExportManager::ExportASCII(TH1*)!!!");
} // catch



}

void TGo4ExportManager::ExportASCII(TGraph* graph)
{
if(graph==0) return;
try{
   TString objectname=graph->GetName();
   TString outname=objectname;
   outname.Append(".gdat");
   gSystem->cd(fxCurrentDir.Data());
   std::ofstream outfile(outname.Data());
   if(!outfile) {
      TGo4Log::Message(3,"ExportManager: Error opening outputfile %s",outname.Data());
      return;
   }
   TGo4Log::Message(0,"ExportManager: Converting graph %s to ASCII",graph->GetName());
   Int_t maxpoints=graph->GetN();
   outfile <<"# Graph "<<graph->ClassName() <<": "<<graph->GetName()<<endl;
   outfile <<"# Point \tX \tY"<<endl;
   for(Int_t point=0; point<maxpoints; ++point)
    {
      Double_t xg=0;
      Double_t yg=0;
      graph->GetPoint(point,xg,yg);
      outfile <<point<<" \t\t"<<xg<<" \t"<<yg<<endl;
    }
   outfile.close();
   gSystem->cd(fxStartDir.Data());
}//try

catch(std::exception& ex) // treat standard library exceptions
{
  TGo4Log::Message(3,"standard exception %s in TGo4ExportManager::ExportASCII(TGraph*)",
            ex.what());
}
catch(...)
{
  TGo4Log::Message(3,"!!! Unexpected exception in TGo4ExportManager::ExportASCII(TGraph*)!!!");
} // catch



}





void TGo4ExportManager::ExportRadware(TH1* histo)
{
//TGo4Log::Message(2,"ExportManager: Converting histogram %s to radware not supported yet!",histo->GetName());
//return;
if(histo->InheritsFrom(TH2::Class()))
   {
       TH2* map=dynamic_cast<TH2*>(histo);
       ExportRadware(map);
   }
else if (histo->InheritsFrom(TH3::Class()))
   {
     TGo4Log::Message(2,"ExportManager: Converting 3d histogram %s to radware not supported yet!",histo->GetName());
   }
else
   {

try{
   TString objectname=histo->GetName();
   TString outname=objectname;
   outname.Append(".spe");
   TString hname = objectname;
   hname.Append("        ");
   Int_t maxbinX=histo->GetNbinsX();
   Int_t l_chan=maxbinX; // get record size from histo here...
   Int_t l_head[6];
   l_head[0]=l_chan;
   l_head[1]=1;
   l_head[2]=1;
   l_head[3]=1;
   l_head[4]=24;
   l_head[5]=l_chan*4; /* record size */
   gSystem->cd(fxCurrentDir.Data());
   std::ofstream outfile(outname.Data());
  if(!outfile)
    {
      TGo4Log::Message(3,"ExportManager: Error opening outputfile %s",outname.Data());
      return;
    }
  TGo4Log::Message(0,"ExportManager: Converting 1d histogram %s to RADWARE",histo->GetName());
//// write first long of head, i.e. value 24
   Int_t first=24;
   outfile.write((char *) &first, sizeof(Int_t));
////write name string:
   outfile.write(hname.Data(), 8);// name limited to 8 characters
//// write complete header:
   outfile.write((char *) l_head, 6*sizeof(Int_t));
//// write data field:
   Float_t cont=0;
   for(Int_t xbin=0; xbin<maxbinX; ++xbin)
       {
          cont=(Float_t) histo->GetBinContent(xbin);
          outfile.write( (char *) &cont, sizeof(Float_t) );
       }
//// write length of data in chars to end of data field
   Int_t charnum=l_chan*sizeof(Float_t);
   outfile.write((char *) &charnum, sizeof(Int_t));
   Int_t hislen = l_chan*4 + 40;
   Int_t byteswritten=outfile.tellp();
   //cout <<"position in stream is "<<byteswritten << endl;
   //cout <<"datalength was"<<hislen << endl;
   // consistency check: chars written are data field+header size
   if (byteswritten == hislen)
   {
     TGo4Log::Message(1,"Histogram %s:  %d bytes (data %d) written to %s",
        hname.Data(), byteswritten, hislen, outname.Data());
   }
   else
   {
     TGo4Log::Message(3,"Histogram %s:  Size mismatch: %d bytes written to %s, datalength is %d",
        hname.Data(), byteswritten, outname.Data(), hislen);
   }
   outfile.close();
   gSystem->cd(fxStartDir.Data());
}// try /////////////////////////////////////////////


catch(std::exception& ex) // treat standard library exceptions
{
  TGo4Log::Message(3,"standard exception %s in TGo4ExportManager::ExportRadware(TH1*)",
            ex.what());
}
catch(...)
{
  TGo4Log::Message(3,"!!! Unexpected exception in TGo4ExportManager::ExportRadware(TH1*)!!!");
} // catch

}// if(histo->InheritsFrom...)

}

void TGo4ExportManager::ExportRadware(TH2* histo)
{
   TGo4Log::Message(2,"ExportManager: Converting 2d histo %s to radware not supported yet!",histo->GetName());
}
void TGo4ExportManager::ExportRadware(TGraph* graph)
{
TGo4Log::Message(2,"ExportManager: Converting graph %s to radware not supported yet!",graph->GetName());
}

void TGo4ExportManager::ExportXML(TObject* ob)
{
   if (ob==0) return;
   TString fname=ob->GetName();
   fname += ".xml";

   TFile* f = TFile::Open(fname.Data(), "recreate");
   if (f==0) return;
   f->cd();
   ob->Write();
   delete f;
   TGo4Log::Message(0,"ExportManager: Wrote object %s of class %s to root file %s",
      ob->GetName(), ob->ClassName(),fname.Data() );
}


void TGo4ExportManager::ExportRoot(TObject* ob)
{
   if(ob==0) return;
   TString fname=fxOutFile;
   TString ftitle=fxOutFileComment;
   if(!fname.Contains(".root")) fname.Append(".root");
   TFile* f = TFile::Open(fname.Data(), "recreate");
   if (f==0) return;
   f->SetTitle(ftitle.Data());
   f->cd();
   ob->Write();
   delete f;
   TGo4Log::Message(0,"ExportManager: Wrote object %s of class %s to root file %s",
      ob->GetName(), ob->ClassName(),fname.Data() );
}
