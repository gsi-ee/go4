#include "TGo4RuntimeException.h"

#include "TApplication.h"

#include "TGo4Log.h"

TGo4RuntimeException::TGo4RuntimeException()
{
   fxDescription= "!!!-- Go4 Runtime Exception --!!!";
}

TGo4RuntimeException::~TGo4RuntimeException()
{
}

TGo4RuntimeException::TGo4RuntimeException(const TGo4RuntimeException &right)
  :TGo4Exception(right)
{
}

Int_t TGo4RuntimeException::Handle()
{

   TGo4Exception::Handle(); // superclass handler: output of exception
   TGo4Log::Debug("\n !!! Unspecified Runtime Exception, aborting application !!! \n");
   gApplication->Terminate(0);
   return 0;
}

TGo4RuntimeException & TGo4RuntimeException::operator=(const TGo4RuntimeException &right)
{
  if (&right!=this)
    {
      TGo4Exception::operator=(right); // copy base class members
      // put additional member copies here...
      return *this;
    }
  else
    {
      // copy is already source object
      return *this;
    }
}

