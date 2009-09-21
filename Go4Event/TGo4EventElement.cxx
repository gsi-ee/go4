#include "TGo4EventElement.h"

#include "Riostream.h"
#include "TTree.h"

#include "TGo4Log.h"

#include "TGo4EventSource.h"

R__EXTERN TTree *gTree;

TGo4EventElement::TGo4EventElement()
: TNamed("Go4Element","This is a Go4 EventElement"),
   fbIsValid(kTRUE), fxParent(0), fxEventSource(0)
{

   TRACE((15,"TGo4EventElement::TGo4EventElement()",__LINE__, __FILE__));
   fIdentifier=-1;
   isActivated=kFALSE;
   fDebug=kFALSE;
}

TGo4EventElement::TGo4EventElement(const char* name)
: TNamed(name,"This is a Go4 EventElement"),
   fbIsValid(kTRUE), fxParent(0), fxEventSource(0)
{
TRACE((15,"TGo4EventElement::TGo4EventElement(const char*)",__LINE__, __FILE__));
 fIdentifier=-1;
 isActivated=kFALSE;
 fDebug=kFALSE;
}

TGo4EventElement::TGo4EventElement(const char* aName,
                                   const char* aTitle,
                                   Short_t aBaseCat)
{

//Allocates a new event Element with name aName and title aTitle
// aBaseCat could be the identifier of the Element for fast
// retrieval of objects in the composite structure
  SetName(aName);
  SetTitle(aTitle);
  fIdentifier=aBaseCat;
  isActivated=kFALSE;
  fDebug=kFALSE;

}

TGo4EventElement::~TGo4EventElement()
{

TRACE((15,"TGo4EventElement::~TGo4EventElement()",__LINE__, __FILE__));
}

Bool_t TGo4EventElement::CheckEventSource(const char* classname)
{
   TRACE((12,"TGo4EventElement::CheckEventSource(Text_t*)",__LINE__, __FILE__));
   if(fxEventSource==0)
      {
         return kFALSE;
      }
   else
      {
        return ( fxEventSource->InheritsFrom(classname) );
      }
}

void TGo4EventElement::PrintEvent()
{
   TRACE((12,"TGo4EventElement::PrintEvent()",__LINE__, __FILE__));

   TGo4Log::Debug( " EventElement printout: ");
   TGo4Log::Debug( "\tIsValid=%d ",fbIsValid);
   if(fxEventSource)
      {
         TGo4Log::Debug( "\tEventSource: %s of class %s",
                        fxEventSource->GetName(),
                        fxEventSource->ClassName() );
      }
   else
      {
         TGo4Log::Debug( "\tNO EventSource");
      }
}

void TGo4EventElement::Print(Option_t* option) const
{
   ((TGo4EventElement*)this) -> PrintEvent();
}

void TGo4EventElement::makeBranch(TBranch *parent)
{
  // method for recursive branching algorithm

}

Int_t  TGo4EventElement::activateBranch(TBranch *branch,Int_t splitLevel,Int_t init){

  TString cad=branch->GetName();
  if(!isActivated){
      TTree* tree = branch->GetTree();
      TGo4EventElement *dump = this;
      tree->SetBranchAddress(cad.Data(), &dump );

      tree->SetBranchStatus(cad.Data(), 1);
      cad+="*";
      tree->SetBranchStatus(cad.Data(), 1);
      isActivated=kTRUE;
  }

  branch->GetEntry(0);
  return 0;
}

void TGo4EventElement::deactivate()
{
  TString name=GetName();
  name+=".";
  gTree->SetBranchStatus(name.Data(), 0);
  name+="*";
  gTree->SetBranchStatus(name.Data(), 0);
  cout << "-I- Deactivating elements at location :" << name << endl;
}

void TGo4EventElement::activate(){

TString name=GetName();
  name+=".";
  gTree->SetBranchStatus(name.Data(), 1);
  name+="*";
  gTree->SetBranchStatus(name.Data(), 1);
  cout << "-I- Activating elements at location :" << name << endl;;
}

void TGo4EventElement::Clear(Option_t *)
{
}

Int_t TGo4EventElement::Init()
{
   Int_t res=0;
   Clear();
   SetValid(kTRUE);
   cout << "**** Event " << GetName();
   if (fxEventSource)
      cout << " has source " << fxEventSource->GetName() << " class: " << fxEventSource->ClassName() << endl;
   else {
      cout << " has no data source" << endl;
      res = 1;
   }
   return res;
}

Int_t TGo4EventElement::Fill()
{
   Int_t res(0);
   Clear();

   if (fxEventSource) {
      if (!fxEventSource->BuildEvent(this)) res = 1;
   } else
      res = 1;

   return res;
}

