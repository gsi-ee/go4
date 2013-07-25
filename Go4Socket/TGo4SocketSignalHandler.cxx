// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4SocketSignalHandler.h"


Int_t TGo4SocketSignalHandler::fgiLastSignal = 0; // unix signal id

TGo4SocketSignalHandler::TGo4SocketSignalHandler(Int_t signum, Bool_t enabled)
{
   SetSignalAction(signum,enabled);
}

TGo4SocketSignalHandler::~TGo4SocketSignalHandler()
{
}


void TGo4SocketSignalHandler::Handle(int signum)
{
   SetSignalAction(signum,0);
   //std::cout << "got a signal " << signum << " ("<< getpid() << ") Id: ";
// optional part:
//   if(signum == SIGWINCH)
//      {
//         //std::cout << "got a SIGWINCH\n";
//         //TGo4SocketSignalHandler::fgiLastSignal = SIGWINCH;
//
//       }
// end optional part
   TGo4SocketSignalHandler::fgiLastSignal = signum;
   SetSignalAction(signum,1);
}

#ifndef WIN32

#include <signal.h>

void TGo4SocketSignalHandler::SetSigWINCH(Bool_t enabled)
{
   SetSignalAction(SIGWINCH, enabled);
}

void TGo4SocketSignalHandler::SetSignalAction(Int_t signum, Bool_t enabled)
{
  struct sigaction new_action, old_action;

  /* Set up the structure to specify the new action. */
  if(enabled)
    new_action.sa_handler = TGo4SocketSignalHandler::Handle;
  else
    new_action.sa_handler = SIG_IGN;

  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;
  sigaction (signum, NULL, &old_action);
  if(enabled)
    sigaction (signum, &new_action, NULL);
}

void TGo4SocketSignalHandler::DisableSigPipe()
{
   signal(SIGPIPE, SIG_IGN);
}

bool TGo4SocketSignalHandler::IsLastSignalWINCH()
{
   return GetLastSignal() == SIGWINCH;
}

#else

// do nothing in windows

void TGo4SocketSignalHandler::SetSignalAction(Int_t signum, Bool_t enabled)
{
}

void TGo4SocketSignalHandler::DisableSigPipe()
{
}

void TGo4SocketSignalHandler::SetSigWINCH(Bool_t)
{
}

bool TGo4SocketSignalHandler::IsLastSignalWINCH()
{
   return false;
}

#endif

Int_t TGo4SocketSignalHandler::GetLastSignal()
{
   return fgiLastSignal;
}

void TGo4SocketSignalHandler::SetLastSignal(Int_t v)
{
   fgiLastSignal = v;
}


