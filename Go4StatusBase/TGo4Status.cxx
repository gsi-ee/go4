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

#include "TGo4Status.h"

#include <stdarg.h>
#include <iostream>

#include "snprintf.h"
#include "TROOT.h"

#include "TGo4Log.h"

TGo4Status::TGo4Status() :
   TNamed()
{
}

TGo4Status::TGo4Status(const char *name) :
   TNamed(name, "This is a Go4 Status Object")
{
   GO4TRACE((12,"TGo4Status::TGo4Status(const char *)",__LINE__, __FILE__));
}

TGo4Status::TGo4Status(const char *name, const char *title) :
   TNamed(name, title)
{
   GO4TRACE((12,"TGo4Status::TGo4Status(const char *, const char *)",__LINE__, __FILE__));
}

TGo4Status::~TGo4Status()
{
   GO4TRACE((12,"TGo4Status::~TGo4Status()",__LINE__, __FILE__));
}

void TGo4Status::Print(Option_t *dummy) const
{
   // this trick is needed since root defines Print as const function...
   // keep code here to support by chance old objects with implemented PrintStatus
   TGo4Status *localthis = const_cast<TGo4Status *>(this);
   localthis->PrintStatus();
}

Int_t TGo4Status::PrintStatus(Text_t *buffer, Int_t buflen)
{
   GO4TRACE((12,"TGo4Status::PrintStatus()",__LINE__, __FILE__));

   if(buflen <= 0 && buffer) return 0;

   Int_t size = 0;
   TString localbuf = TString::Format("G-OOOO-> Status Class %s, name: %s <-OOOO-G\n", ClassName(), GetName());

   if(!buffer) {
      std::cout << localbuf << std::endl;
   } else {
      size = localbuf.Length();
      if(size>buflen-1) size = buflen-1;
      strncpy(buffer,localbuf.Data(), size);
   }
   return size;
}

Text_t *TGo4Status::PrintIndent(Text_t *buffer, Int_t &buflen)
{
   if(buflen < 0 && buffer)
      return nullptr;
   Int_t restlen = buflen;
   Text_t *cursor = buffer;
   for (int i = 0; (i < TROOT::GetDirLevel()) && (i < buflen); i++) {
      cursor = PrintBuffer(cursor, restlen, " ");
   }
   buflen = restlen;
   return cursor;
}

Text_t *TGo4Status::PrintBuffer(char *buffer, Int_t &buflen, const char *text,...)
{
   if(!buffer || buflen < 0)
      return nullptr;
   va_list args;
   va_start(args, text);
   Int_t size=vsnprintf(buffer, buflen, text, args);
   va_end(args);
   if(size>buflen || size<0)
      return nullptr;
   char *current = buffer + size;
   buflen -= size;
   return current;
}

void TGo4Status::PrintLine(const char *text, ...)
{
   const int bufsize = 2000;
   char buffer[bufsize];
   char *cursor = buffer;
   int len = bufsize;

   for (int i = 0; (i < TROOT::GetDirLevel()) && (len > 10); i++) {
      *cursor++ = ' ';
      len--;
   }

   va_list args;
   va_start(args, text);
   vsnprintf(cursor, len-1, text, args);
   va_end(args);

   buffer[bufsize-1] = 0; // ensure null-terminated string

   std::cout << buffer << std::endl;
}

