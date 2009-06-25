#include "TGo4CompositeEvent.h"

#include "Riostream.h"

#include "TTree.h"
#include "TROOT.h"
#include "TObjArray.h"

R__EXTERN TTree *gTree;

TGo4CompositeEvent::TGo4CompositeEvent() :
   TGo4EventElement(),
   fNElements(0),
   fEventElements(0)
{
}

TGo4CompositeEvent::TGo4CompositeEvent(const char*aName, const char* aTitle, Short_t aBaseCat) :
   TGo4EventElement(aName,aTitle, aBaseCat),
   fNElements(0),
   fEventElements(0)
{
   fEventElements = new TObjArray(12);
}

TGo4CompositeEvent::~TGo4CompositeEvent()
{
   //cout << "Start TGo4CompositeEvent::~TGo4CompositeEvent(void)"<< this << endl;
   if(fEventElements!=0) {
        fEventElements->Sort();

         if(!fEventElements->IsEmpty()){
            fEventElements->Delete();
             //cout << "TGo4CompositeEvent::~TGo4CompositeEvent(void) " <<this << " "  <<fEventElements->IsEmpty() << endl;
         }
      delete fEventElements;

   }
    //cout << " End  TGo4CompositeEvent::~TGo4CompositeEvent(void)"<< this <<endl;
}


void TGo4CompositeEvent::makeBranch(TBranch *parent)
{
  TGo4EventElement  **par;
  TBranch *b = 0;
  Int_t i=0;

  TObjArray &vector=*fEventElements;
  for(i=0;i<=fEventElements->GetLast();i++) {
    par=(TGo4EventElement **)&(vector[i]);
    if ((*par)!=NULL) {
      b=gTree->TTree::Branch(Form("%s.",(*par)->GetName()), (*par)->ClassName(),par,4000,99);
      (*par)->makeBranch(b);
    }
  }
  TGo4EventElement::makeBranch( parent );
}


Int_t  TGo4CompositeEvent::activateBranch( TBranch *branch, Int_t splitLevel, Int_t init){

  // synchronize with TTree for Composite Event
  if ( branch ){
    TString cad=branch->GetName();
    TTree *tree = branch->GetTree();

    TGo4CompositeEvent *dump=this;
    tree->SetBranchStatus(cad.Data(),1);
    tree->SetBranchAddress(cad.Data(), &dump);
    cad+="*";
    tree->SetBranchStatus(cad.Data(),1);

    TGo4EventElement::activateBranch(branch,splitLevel);

    if (fDebug)
    printf("##### TGo4CompositeEvent::activateBranch called from obj:%s \n",this->GetName());

    TObjArray *br = tree->GetListOfBranches();
    TBranch*  b=NULL;
    TClass*  cl=NULL;
    char* obj=NULL;

    Int_t i=init;

    if (fDebug)
    printf("-I-TGo4CompositeEvent::activateBranch from obj:%s bname%s Elements :%i  index:%i\n",
             this->GetName(),cad.Data(), fNElements,init);

    int max = init+fNElements;
    while ( i < init+fNElements ){
      i++;
      TGo4EventElement *par = NULL;
      b = (TBranch*) br->At(i);
      if(b){

   if (fDebug)
        printf("-I TGo4CompositeEvent::activateBranch from obj:%s (while) bname:%s cname:%s init:%i elem:%i \n",
          this->GetName(), b->GetName(), b->GetClassName(), init, fNElements);

   cl=gROOT->GetClass(b->GetClassName());
   obj= (char*) cl->New();
   par= (TGo4EventElement*) obj;
   if(par) {
       Int_t offset=par->activateBranch(b, splitLevel,i);
       if (fDebug)
       printf("-I activate from obj:%s elems:%i index:%i adding:%s\n",
         this->GetName(), max , i, par->GetName());

       addEventElement(par,1);
       i+=offset;
       fNElements+=offset;
       if (fDebug)
       printf("-I from obj:%s activate indexing after offset:%i index:%i elems:%i\n",
         this->GetName(),
         offset,i,max);
   }
      }

    } //!while
    Clear();
  }//!branch

  if (fDebug)
  printf("-I activate return value from obj:%s offset:%i \n", this->GetName(),
     fNElements);
  return fNElements;

}




void  TGo4CompositeEvent::synchronizeWithTree( TTree *tree, Int_t splitLevel){
  // synchronize with TTree for composite Event
  // FIXME !
  // should <test> the differents splitlevel?


  if (tree==0) return;

  if (fEventElements==0)
     fEventElements = new TObjArray(12);

    TGo4CompositeEvent *dump=this;

    //-- Initialize the global Branch structure
    TBranch* b=NULL;
    TObjArray *br = tree->GetListOfBranches();
    b=(TBranch*) br->At(0);
    TString cad=b->GetName();

    tree->SetBranchStatus(cad.Data(),1);
    tree->SetBranchAddress(cad.Data(), &dump);
    cad+="*";
    tree->SetBranchStatus(cad.Data(),1);

    // Initialize underlying  TTree
    TGo4EventElement::activateBranch(b,splitLevel);
    //--
    TClass* cl=NULL;
    char* obj=NULL;

    if(fDebug)
            printf("-I- TGo4CompositeEvent::synchronize(tree): Elements :%i Id:%i branch:%s \n", fNElements ,fIdentifier,cad.Data());

    //-- Rebuild the Elements array
    Int_t i=0;
    while( i<fNElements ){
     i++;
      TGo4EventElement *par = NULL;
      b = (TBranch*) br->At(i);
      if(b){

       if (fDebug)
    printf("-I TGo4CompositeEvent::synchronize(tree) bname:%s cname:%s Elem:%i\n",
                          b->GetName(), b->GetClassName(), fNElements);

   cl=gROOT->GetClass(b->GetClassName());
   obj= (char*) cl->New();
   par= (TGo4EventElement*) obj;
   if(par) {
     Int_t offset=par->activateBranch(b, splitLevel,i);
     if (fDebug)
           printf("-I synchronize index:%i from:%s adding:%s\n",
          i, this->GetName(), par->GetName());
     addEventElement(par,1);
     i+=offset;
          fNElements+=offset;

     if (fDebug)
          printf("-I synchronize indexing after offset:%i index:%i elem:%i \n",
         offset,i,fNElements);
   }

      }

    } //!while
     Clear();
     //-- Resynchronize all branches with the TTree (needed once)
     //TGo4LockGuard Glob2;
     tree->GetEntry(0);
}




void TGo4CompositeEvent::Clear(Option_t *) {
  //Clears the data in the event (i.e. clears the internal buffers...)

     TIter next(fEventElements);
     TGo4EventElement *ev;
     while ( (ev=(TGo4EventElement *)next())!=NULL) {
              ev->Clear();
     }
}



void TGo4CompositeEvent::clearAll(Int_t level) {
  // Clears the data in the event and the event structure (list of subevents...)
  // FIXME
  // Levels are to be defined

  if (level==0) {
    // if (fEventElements) fEventElements->Delete();
  } else {
    TIter next(fEventElements);
     TGo4EventElement *ev;
    while ((ev=( TGo4EventElement *)next())!=NULL) {
      ev->clearAll(level);
    }
  }
}


void TGo4CompositeEvent::addEventElement( TGo4EventElement* aElement, Int_t reading){
  // if Identifiers  are needed for fast retrieval of elements
  // one should use:
  // >>> fEventElements->AddAtAndExpand(aElement,aElement->getId());
  // Note: When reading from file, adding elements should not be
  // incremented

 if (getEventElement(aElement->GetName(),1)){
      printf("-E-1 <TGo4CompositeEvent::addEventElement> object:%s already in structure => not added ! \n",
                   aElement->GetName());
      return;
   }

  if ( aElement->getId() < 0 ) {
    printf("-E-2 <TGo4CompositeEvent::addEventElement> object:%s with invalid Id:%i   => not added ! \n",
             aElement->GetName(), aElement->getId());
    return;
  }

  if ( getEventElement( aElement->getId() ) ){
      printf("-E-3 <TGo4CompositeEvent::addEventElement> object:%s with Id:%i already used => not added ! \n",
        aElement->GetName(), aElement->getId());
  }

  if (fDebug)
  printf("-I adding element in :%s :%p at location:%i \n",GetName(),aElement, aElement->getId());
  fEventElements->AddAtAndExpand (aElement, aElement->getId());
  if( reading == 0 ) fNElements++;

}


TGo4EventElement* TGo4CompositeEvent::getEventElement(Int_t idx) {
  // Returns a pointer to the partial event with number idx.

  if ( idx > fEventElements->GetLast() ) return NULL;
  return ( ( TGo4EventElement*) fEventElements->At(idx) );
}


TGo4EventElement* TGo4CompositeEvent::getEventElement(const char* name,Int_t final) {

  TIter next(fEventElements);
  TGo4EventElement *ev=NULL,*inter=NULL;
  while ((ev=( TGo4EventElement *)next())!=NULL) {
      if(strcmp(name,ev->GetName())==0) return ev;
      inter=NULL;
      if (ev->isComposed()) {
         inter= ((TGo4CompositeEvent*) ev)->getEventElement(name,1);
         if (inter !=0 ) return inter;
      }
  }
  if(final==0)
     printf("-E- TGo4CompositeEvent => Element:%s not found in Composite:%s \n",name,this->GetName());
  return NULL;
}

void TGo4CompositeEvent::deactivate(){
   //

  TIter next(fEventElements);
  TGo4EventElement *ev=NULL;

  while ((ev=( TGo4EventElement *)next())!=NULL) {
   ev->deactivate();
  }
}

void TGo4CompositeEvent::activate(){
      //
  TIter next(fEventElements);
  TGo4EventElement *ev=NULL;

  while ((ev=( TGo4EventElement *)next())!=NULL) {
   ev->activate();
  }
}




TObjArray* TGo4CompositeEvent::getListOfComposites(Bool_t toplevel){
   //
  TObjArray *comp = new TObjArray(12);
  TObjArray *dump = NULL;
  TIter next(fEventElements);
  TGo4EventElement *ev,*ev2;
  //-- Add top level composite
  if (toplevel) {
    comp->Add(this);
    // this->Dump();
  }

  ev=NULL;
  while ((ev=( TGo4EventElement *)next())!=NULL) {
     if ( ev->isComposed() ) {
      comp->Add( ev );
      dump = ((TGo4CompositeEvent*) ev)->getListOfComposites(kFALSE);
      TIter next2( dump);
      ev2=NULL;
       while( (ev2=( TGo4EventElement *)next2())!=NULL) {
   comp->Add( ev2 );
      }
    }
  }
  return comp;
}


TGo4EventElement& TGo4CompositeEvent::operator[]( Int_t i )
{
   return *dynamic_cast<TGo4EventElement*> ((*fEventElements)[i]);
}

void TGo4CompositeEvent::Streamer(TBuffer &R__b)
{
  // Stream an object of class TGo4CompositeEvent.
  //  char buffer[255];
   //
  if (R__b.IsReading()) {
    // printf("TGo4CompositeEvent::Streamer reading \n");
    Version_t R__v = R__b.ReadVersion(); if (R__v) { }
    TGo4EventElement::Streamer(R__b);
    R__b >> fEventElements;
    R__b >> fNElements;

  } else {
    //   printf("TGo4CompositeEvent::Streamer writing \n");
   TGo4EventElement::Streamer(R__b);
    R__b << fEventElements;
    R__b << fNElements;
  }
}



