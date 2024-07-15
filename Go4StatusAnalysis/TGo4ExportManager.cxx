// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ExportManager.h"

#include <fstream>
#include <istream>

#include "TSystem.h"
#include "TFolder.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"

#include "Riostream.h"

#include "TGo4Log.h"

// JAM7-2024:
#define GO4_EXMAN_CREATE_HISTOGRAM(X,Y) \
    switch(X){ \
              case 1:\
                 theHisto = new TH1##Y(name.Data(), title.Data(), bins[0], axmin[0], axmax[0]);\
              break;\
              case 2:\
                 theHisto = new TH2##Y(name.Data(), title.Data(), bins[0], axmin[0], axmax[0], bins[1], axmin[1], axmax[1]);\
              break;\
              case 3:\
                 theHisto = new TH3##Y(name.Data(), title.Data(), bins[0], axmin[0], axmax[0], bins[1], axmin[1], axmax[1], bins[2], axmin[2], axmax[2]);\
              break;\
    }

TGo4ExportManager::TGo4ExportManager(const char *name)
   : TNamed(name,"This is a Go4 export manager"),fiFilter(GO4EX_ROOT)
{
  fxOutFile = "Go4Export";
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

void TGo4ExportManager::SetCurrentDir(const char *dir)
{
   if (dir) {
      fxCurrentDir = dir;
      gSystem->cd(dir);
   } else {
      fxCurrentDir = gSystem->WorkingDirectory();
   }
}

void TGo4ExportManager::SetStartDir(const char *dir)
{
   if (dir)
      fxStartDir = dir;
   else
      fxStartDir = gSystem->WorkingDirectory();
}

void TGo4ExportManager::Export(TObject *ob, Go4Export_t format)
{
   if(!ob) return;
   SetFilter(format);
   Export(ob);
}


void TGo4ExportManager::Export(TObject *myobject)
{
  if(!myobject) return;

  if (myobject->InheritsFrom(TDirectory::Class())) {
      TDirectory *subdir = dynamic_cast<TDirectory *>(myobject);
      Export(subdir);
   } else if (myobject->InheritsFrom(TFolder::Class())) {
      TFolder *subfold = dynamic_cast<TFolder *>(myobject);
      Export(subfold);
   } else if (myobject->InheritsFrom(TCollection::Class())) {
      TCollection *col = dynamic_cast<TCollection *>(myobject);
      Export(col);
   } else if (myobject->InheritsFrom(TH1::Class())) {
      TH1 *histo = dynamic_cast<TH1 *>(myobject);
      Export(histo);
   } else if (myobject->InheritsFrom(TGraph::Class())) {
      TGraph *graph = dynamic_cast<TGraph *>(myobject);
      Export(graph);
   } else {
      switch (fiFilter) {
      case GO4EX_ROOT:
         ExportRoot(myobject);
         break;
      case GO4EX_XML:
         ExportXML(myobject);
         break;
      case GO4EX_ASCII:
      case GO4EX_ASCII_CHANNELS:
      case GO4EX_RADWARE:
      default:
         TGo4Log::Message(2,
               "ExportManager: NOT Converting object %s of class %s",
               myobject->GetName(), myobject->ClassName());
      }
   }
}


void TGo4ExportManager::Export(TFolder *fold)
{
   if (!fold)
      return;
   TGo4Log::Message(0, "ExportManager: Converting contents of folder %s",
         fold->GetName());
   if (fiFilter == GO4EX_ROOT) {
      // root filter will write collection completely into one root file
      // otherwise, each object would be written separately into flat hierarchy
      ExportRoot(fold);
      return;
   }

   TString dirname = fold->GetName();
   gSystem->cd(fxCurrentDir.Data()); // create subdirectory in file system
   TString com = "mkdir " + dirname;
   gSystem->Exec(com);
   gSystem->cd(dirname.Data());
   fxCurrentDir = gSystem->WorkingDirectory();
   TCollection *folderlist = fold->GetListOfFolders();
   Export(folderlist);
   gSystem->cd(fxCurrentDir.Data());
   gSystem->cd("..");
   fxCurrentDir = gSystem->WorkingDirectory(); // go up one directory level again
   gSystem->cd(fxStartDir.Data());
}

void TGo4ExportManager::Export(TDirectory *source)
{
   if (!source)
      return;
   TGo4Log::Message(0, "ExportManager: Converting contents of directory %s", source->GetName());
   if (fiFilter == GO4EX_ROOT) {
      // root filter will write collection completely into one root file
      // otherwise, each object would be written separately into flat hierarchy
      ExportRoot(source);
      return;
   }

   TString dirname = source->GetName();
   if (!dirname.Contains(".root")) {
      gSystem->cd(fxCurrentDir.Data()); // create subdirectory in file system
      TString com = "mkdir " + dirname;
      gSystem->Exec(com);
      gSystem->cd(dirname.Data());
      fxCurrentDir = gSystem->WorkingDirectory();
   }
   source->cd();
   gSystem->cd(fxStartDir.Data());
   TIter iter(source->GetListOfKeys());
   while (auto mykey = (TKey *)iter()) {
      auto myobject = mykey->ReadObj();
      if (myobject) {
         Export(myobject);
      } else {
         TGo4Log::Message(2, "ExportManager: Could not read key %s", mykey->GetName());
      }
   } // while
   if (!dirname.Contains(".root")) {
      gSystem->cd(fxCurrentDir.Data());
      gSystem->cd("..");
      fxCurrentDir = gSystem->WorkingDirectory(); // go up one directory level again
      gSystem->cd(fxStartDir.Data());
   }
}

void TGo4ExportManager::Export(TCollection *col)
{
  if(!col) return;
  TGo4Log::Message(0,"ExportManager: Converting contents of collection %s",col->GetName());
  if(fiFilter == GO4EX_ROOT) {
     // root filter will write collection completely into one root file
     // otherwise, each object would be written separately into flat hierarchy
     ExportRoot(col);
     return;
  }

   TIter iter(col);
   while(auto ob = iter())
      Export(ob);
}

void TGo4ExportManager::Export(TH1 *histo)
{
   switch(fiFilter) {
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

void TGo4ExportManager::Export(TGraph *gra)
{
   switch(fiFilter) {
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

void TGo4ExportManager::ExportASCII(TH1 *histo, Bool_t channels)
{
if(!histo) return;
try{
   TString objectname=histo->GetName();
   TString outname=objectname;
   outname.Append(".hdat");
   gSystem->cd(fxCurrentDir.Data());

   std::ofstream outfile(outname.Data());
   if(!outfile) {
      TGo4Log::Message(3,"ExportManager: Error opening outputfile %s",outname.Data());
      return;
   }
   TGo4Log::Message(0,"ExportManager: Converting histogram %s to ASCII",histo->GetName());
   Int_t maxbinX = histo->GetNbinsX();
   Int_t maxbinY = histo->GetNbinsY();
   Int_t maxbinZ = histo->GetNbinsZ();
   Int_t globalbin = 0;
   Stat_t  cont = 0;
   outfile <<"# Histogram "<<histo->ClassName() <<": "<<histo->GetName()<< std::endl;
   if(channels)
      outfile <<"# Xbin \tYbin \tZbin \tContent"<< std::endl;
   else
      outfile <<"# X \tY \tZ \tContent"<< std::endl;
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
                  outfile <<x<<" \t"<<y<<" \t"<<z<<" \t"<<cont<< std::endl;
               }
              else
               {
                  Axis_t xval = 0;
                  TAxis *xax = histo->GetXaxis();
                  //if(xax) xval=xax->GetBinCenter(x);
                  if(xax) xval=xax->GetBinLowEdge(x);
                  Axis_t yval = 0;
                  TAxis *yax = histo->GetYaxis();
                  //if(yax) yval=yax->GetBinCenter(y);
                  if(yax) yval = yax->GetBinLowEdge(y);
                  Axis_t zval = 0;
                  TAxis *zax = histo->GetZaxis();
                  //if(zax) zval=zax->GetBinCenter(z);
                  if(zax) zval = zax->GetBinLowEdge(z);
                  outfile <<xval<<" \t"<<yval<<" \t"<<zval<<" \t"<<cont<< std::endl;
               }
            }
        }

    }
   outfile.close();
   gSystem->cd(fxStartDir.Data()); // needed in case of TKey read from (file) directory
}// try
catch(std::exception& ex) // treat standard library exceptions
{
  TGo4Log::Message(3,"standard exception %s in TGo4ExportManager::ExportASCII(TH1 *)",
            ex.what());
}
catch(...)
{
  TGo4Log::Message(3,"!!! Unexpected exception in TGo4ExportManager::ExportASCII(TH1 *)!!!");
} // catch

}

void TGo4ExportManager::ExportASCII(TGraph *graph)
{
   if (!graph)
      return;
   try {
      TString objectname = graph->GetName();
      TString outname = objectname;
      outname.Append(".gdat");
      gSystem->cd(fxCurrentDir.Data());
      std::ofstream outfile(outname.Data());
      if (!outfile) {
         TGo4Log::Message(3, "ExportManager: Error opening outputfile %s", outname.Data());
         return;
      }
      TGo4Log::Message(0, "ExportManager: Converting graph %s to ASCII", graph->GetName());
      Int_t maxpoints = graph->GetN();
      outfile << "# Graph " << graph->ClassName() << ": " << graph->GetName() << std::endl;
      outfile << "# Point \tX \tY" << std::endl;
      for (Int_t point = 0; point < maxpoints; ++point) {
         Double_t xg = 0, yg = 0;
         graph->GetPoint(point, xg, yg);
         outfile << point << " \t\t" << xg << " \t" << yg << std::endl;
      }
      outfile.close();
      gSystem->cd(fxStartDir.Data());
   } // try

   catch (std::exception &ex) // treat standard library exceptions
   {
      TGo4Log::Message(3, "standard exception %s in TGo4ExportManager::ExportASCII(TGraph *)", ex.what());
   } catch (...) {
      TGo4Log::Message(3, "!!! Unexpected exception in TGo4ExportManager::ExportASCII(TGraph *)!!!");
   } // catch
}

void TGo4ExportManager::ExportRadware(TH1 *histo)
{
   if (histo->InheritsFrom(TH2::Class())) {
      TH2 *map = dynamic_cast<TH2 *>(histo);
      ExportRadware(map);
   } else if (histo->InheritsFrom(TH3::Class())) {
      TGo4Log::Message(2, "ExportManager: Converting 3d histogram %s to radware not supported yet!", histo->GetName());
   } else {

      try {
         TString objectname = histo->GetName();
         TString outname = objectname;
         outname.Append(".spe");
         TString hname = objectname;
         hname.Append("        ");
         Int_t maxbinX = histo->GetNbinsX();
         Int_t l_chan = maxbinX; // get record size from histo here...
         Int_t l_head[6];
         l_head[0] = l_chan;
         l_head[1] = 1;
         l_head[2] = 1;
         l_head[3] = 1;
         l_head[4] = 24;
         l_head[5] = l_chan * 4; /* record size */
         gSystem->cd(fxCurrentDir.Data());
         std::ofstream outfile(outname.Data());
         if (!outfile) {
            TGo4Log::Message(3, "ExportManager: Error opening outputfile %s", outname.Data());
            return;
         }
         TGo4Log::Message(0, "ExportManager: Converting 1d histogram %s to RADWARE", histo->GetName());
         //// write first long of head, i.e. value 24
         Int_t first = 24;
         outfile.write((char *)&first, sizeof(Int_t));
         ////write name string:
         outfile.write(hname.Data(), 8); // name limited to 8 characters
                                         //// write complete header:
         outfile.write((char *)l_head, 6 * sizeof(Int_t));
         //// write data field:
         Float_t cont = 0;
         for (Int_t xbin = 0; xbin < maxbinX; ++xbin) {
            cont = (Float_t)histo->GetBinContent(xbin);
            outfile.write((char *)&cont, sizeof(Float_t));
         }
         //// write length of data in chars to end of data field
         Int_t charnum = l_chan * sizeof(Float_t);
         outfile.write((char *)&charnum, sizeof(Int_t));
         Int_t hislen = l_chan * 4 + 40;
         Int_t byteswritten = outfile.tellp();
         //  consistency check: chars written are data field+header size
         if (byteswritten == hislen) {
            TGo4Log::Message(1, "Histogram %s:  %d bytes (data %d) written to %s", hname.Data(), byteswritten, hislen,
                             outname.Data());
         } else {
            TGo4Log::Message(3, "Histogram %s:  Size mismatch: %d bytes written to %s, datalength is %d", hname.Data(),
                             byteswritten, outname.Data(), hislen);
         }
         outfile.close();
         gSystem->cd(fxStartDir.Data());
      } // try /////////////////////////////////////////////

      catch (std::exception &ex) // treat standard library exceptions
      {
         TGo4Log::Message(3, "standard exception %s in TGo4ExportManager::ExportRadware(TH1 *)", ex.what());
      } catch (...) {
         TGo4Log::Message(3, "!!! Unexpected exception in TGo4ExportManager::ExportRadware(TH1 *)!!!");
      } // catch

   } // if(histo->InheritsFrom...)
}

void TGo4ExportManager::ExportRadware(TH2 *histo)
{
   TGo4Log::Message(2,"ExportManager: Converting 2d histo %s to radware not supported yet!",histo->GetName());
}
void TGo4ExportManager::ExportRadware(TGraph *graph)
{
   TGo4Log::Message(2,"ExportManager: Converting graph %s to radware not supported yet!",graph->GetName());
}

void TGo4ExportManager::ExportXML(TObject *ob)
{
   if (!ob) return;
   TString fname=ob->GetName();
   fname += ".xml";

   TFile *f = TFile::Open(fname.Data(), "recreate");
   if (!f) return;
   f->cd();
   ob->Write();
   delete f;
   TGo4Log::Message(0,"ExportManager: Wrote object %s of class %s to root file %s",
      ob->GetName(), ob->ClassName(),fname.Data() );
}


void TGo4ExportManager::ExportRoot(TObject *ob)
{
   if(!ob) return;
   TString fname=fxOutFile;
   TString ftitle=fxOutFileComment;
   if(!fname.Contains(".root")) fname.Append(".root");
   TFile *f = TFile::Open(fname.Data(), "recreate");
   if (!f) return;
   f->SetTitle(ftitle.Data());
   f->cd();
   ob->Write();
   delete f;
   TGo4Log::Message(0,"ExportManager: Wrote object %s of class %s to root file %s",
      ob->GetName(), ob->ClassName(),fname.Data() );
}



TH1* TGo4ExportManager::ImportHistogram(const char *filename,
      Go4Import_t format)
{
   switch (format) {
      case GO4IM_ASCII:
         return ImportHistogramGo4Ascii(filename);
//         TGo4Log::Message(2,
//               "ExportManager: not yet implemented plain ascii histogram import,could not use file %s  ",
//               filename);
      break;

      case GO4IM_ORTEC_MCA:
         return ImportHistogramOrtec(filename);
         break;

      default:
         TGo4Log::Message(2,
               "ExportManager: Can not import histogram from file %s with unknown import type %d ",
               filename, format);
   }
   return nullptr;
}


TH1* TGo4ExportManager::ImportHistogramOrtec(const char *nom)
{
// here duplicate functionality of script import_spe.C:
   TString path = nom;
   if (!path.Contains(".Spe")) {
      path.Append(".Spe");
   }
   std::ifstream in;
   in.open(path.Data());
   if (!in.good()) {
      TGo4Log::Message(2,
            "ExportManager: could not open file %s for Ortec MCA histogram import",
            path.Data());
      return nullptr;
   }

   Double_t value = 0;
   Int_t numbins = 1024;
   Int_t xmin = 0;
   Int_t xmax = 1024;

   // TODO: evaluate histogram specs from header first
   std::string header;
   std::string date = "unknown date";
   std::string desc = "no id";
   Int_t headerlines = 12;
   for (Int_t i = 0; i < headerlines; ++i) {
      getline(in, header);
      //cout << "getting header "<<i<<":"<<header.c_str() << endl;
      if (header.find("$DATA:") != std::string::npos) {
         in >> xmin >> xmax;
         xmax += 1; // root TH1 upper limit is exclusive...
         numbins = xmax - xmin;
         i++;
         //cout << " - got xmin="<<xmin<<", xmax="<<xmax<<", numbins="<<numbins << endl;

      } else if (header.find("$DATE_MEA:") != std::string::npos) {
         getline(in, date);
         i++;
         //cout << " - got measurement date="<<date.c_str() << endl;
      } else if (header.find("$SPEC_ID:") != std::string::npos) {
         getline(in, desc);
         i++;
         //cout << " - got description="<<desc.c_str() << endl;
      }

   }

   TString name = path;
   // strip leading path and suffix:
   Ssiz_t lastslash = name.Last('/');
   name = name(lastslash + 1, name.Length());
   Ssiz_t lastdot = name.Last('.');
   name = name(0, lastdot);

   TString title = TString::Format("%s of MCA from %s, %s", name.Data(),
         date.c_str(), desc.c_str());
   TH1D *theHisto = new TH1D(name.Data(), title.Data(), numbins, xmin, xmax);

   for (Int_t b = 0; b < numbins; ++b) {
      in >> value;
      if (!in.good())
         break;
      //theHisto->Fill(b, value);
      theHisto->SetBinContent(b + 1, value);
      //cout << "got bin "<<b<<":"<<value << endl;
   }
   theHisto->ResetStats(); // because we do not Fill() have to calculate stats afterwards

   // TODO: optionally evaluate footer infos here?
   getline(in, header); // dummy read to handle intermediate line feed ??
   //cout << "getting intermdiate line :"<<header.c_str()<<":" << endl;
   Int_t footerlines = 14;
   for (Int_t i = 0; i < footerlines; ++i) {
      getline(in, header);
      //cout << "getting footer "<<i<<":"<<header.c_str() << endl;
   }
   /////////

   return theHisto;
}


TH1* TGo4ExportManager::ImportHistogramGo4Ascii(const char *nom)
{
   TH1 *theHisto = nullptr;
   TString path = nom;
   if (!path.Contains(".hdat")) {
      path.Append(".hdat");
   }
   std::ifstream in;
   in.open(path.Data());
   if (!in.good()) {
      TGo4Log::Message(2,
            "ExportManager: could not open file %s for go4 ASCII histogram import",
            path.Data());
      return nullptr;
   }
   TString name = path;
   // strip leading path and suffix:
   Ssiz_t lastslash = name.Last('/');
   name = name(lastslash + 1, name.Length());
   Ssiz_t lastdot = name.Last('.');
   name = name(0, lastdot);
   TString title = TString::Format("%s imported from ASCII %s.hdat",
         name.Data(), name.Data());

   // TODO: evaluate histogram specs from header first
   std::string header, dummy;

   getline(in, header);
   //std::cout << "getting header " << header.c_str() << std::endl;
   getline(in, dummy);

   Int_t hdim = 0;
   Char_t type = '0';
   std::size_t pos1=header.find("TH1");
   std::size_t pos2=header.find("TH2");
   std::size_t pos3=header.find("TH3");


   if (pos1 != std::string::npos) {
      hdim = 1;
      type=header.at(pos1+3);
   } else if (pos2 != std::string::npos) {
      hdim = 2;
      type=header.at(pos2+3);
   } else if (pos3!= std::string::npos) {
      hdim = 3;
      type=header.at(pos3+3);
   }
   //std::cout << "got histogram dimension " << hdim <<", type is "<< type <<std::endl;

   // TODO: scan number of bins and range
   // JAM7-2024: this works for fixed bin size histograms only. TODO: evaluate bin steps indivually?

   Int_t totalbins=0;
   //Int_t xbins = 0, ybins = 0, zbins  = 0;
   Double_t val=0;
   Double_t axval[3]={0.};
   Double_t axmin[3]={0.};
   Double_t axmax[3]={0.};
   Int_t bins[3]={0};
   Bool_t firstline=kTRUE;

   while (in.good()) {
      in >> axval[0]>> axval[1]>> axval[2] >> val;
      for (Int_t i = 0; i < 3; ++i) {
         if(firstline){
            // for negative axis values we need to initialize with actual first value instead 0
            axmin[i] = axmax[i] = axval[i];
         }
         if (axval[i] <= axmin[i]) {
            axmin[i] = axval[i];
         }
         if (axval[i] > axmax[i]) {
            axmax[i] = axval[i];
            bins[i]++;
         }
      } // for
      totalbins++;
      firstline=kFALSE;
   } // while

   // adjust ROOT upper limit: add one binsize
   for (Int_t i = 0; i < 3; ++i) {
      if(bins[i]==0) continue;
      bins[i]++; // first increment is not counted as bin, add one more
      Double_t binsize = (axmax[i] - axmin[i]) / (bins[i]);
      axmax[i]+=binsize;

//      std::cout <<" -- axmin["<<i<<"]="<< axmin[i] << std::endl;
//      std::cout <<" -- axmax["<<i<<"]="<< axmax[i] << std::endl;
//      std::cout <<" -- bins["<<i<<"]="<< bins[i] << std::endl;
   }



   in.clear(); // reset eof/error flag
   in.seekg(in.beg); // read once again
   getline(in, dummy);
   getline(in, dummy); // skip known headers

   // create histogram of given binsize and range:
   switch(type){
      default:
      case 'D':
         GO4_EXMAN_CREATE_HISTOGRAM(hdim,D);
      break;
      case 'F':
         GO4_EXMAN_CREATE_HISTOGRAM(hdim, F);
         break;
      case 'I':
         GO4_EXMAN_CREATE_HISTOGRAM(hdim, I);
         break;
//      case 'L':
//         GO4_EXMAN_CREATE_HISTOGRAM(hdim, L);
//         break;
      case 'S':
         GO4_EXMAN_CREATE_HISTOGRAM(hdim, S);
         break;
      case 'C':
         GO4_EXMAN_CREATE_HISTOGRAM(hdim, C);
         break;
   };

   // fill histogram:
   while (in.good()) {
        in >> axval[0]>> axval[1] >> axval[2] >> val;
        Int_t globalbin=theHisto->FindBin(axval[0], axval[1], axval[2]);
        //std::cout<<"Read x:"<< axval[0]<<", y:"<<axval[1]<<", z:"<< axval[2]<<", val="<<val<<"- globalbin:"<<globalbin<<std::endl;
        theHisto->SetBinContent(globalbin, val);
     } // while

   theHisto->ResetStats();
   return theHisto;
}

