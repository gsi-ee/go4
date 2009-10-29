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

#ifndef LOCKGUARD_H
#define LOCKGUARD_H

// dummy definition for lockguard for designer plugin

//typedef int TGo4LockGuard;
// this one will give "unused" warnings, better use:
class TGo4LockGuard
{
public:
    TGo4LockGuard(): dummy(0){};

private:
    int dummy;

};

#endif
