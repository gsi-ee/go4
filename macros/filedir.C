#include <fstream.h>
#include "Riostream.h"
using namespace std;


void filedir_namiter(TFile *f, TString fulldir, const char* wildcard)
{
  TString fullname;
  TString curname;
  TRegexp wild(wildcard,kTRUE);
  TKey *key;
  TString curdir;
  TIter next(gDirectory->GetListOfKeys());
  while((key=(TKey*)next()))
    {
      if(strcmp(key->GetClassName(),"TDirectory")==0)
   {
     curdir.Form(fulldir.Data());
     curdir.Append(key->GetName());
     curdir.Append("/");
     f->cd(curdir.Data());
     filedir_namiter(f,curdir,wildcard);
    }
      else
   {
     fullname.Form("%s%s",fulldir.Data(),key->GetName());
     curname.Form("%s",key->GetName());
     if(curname.Index(wild) != kNPOS)
       {
         std::cout.width(32);
         std::cout.fill('.');
         std::cout.setf(ios::left);
              std::cout << key->GetClassName() << " ";
              std::cout << fullname.Data() << std::endl;
   }
   }
    }
  return;
}

void filedir(const char* file, const char* wildcard)
{
  TString fulldir;
  TObject *namo;
  TFile *f;
  std::cout << "Directory of " << file << "(" << wildcard << ")" << std::endl;
  if(f = TFile::Open(file,"r"))
    {
      fulldir.Form("");
      filedir_namiter(f,fulldir,wildcard);
      f->Close();
    }
  else
  std::cout << file << " not found" << std::endl;

}
