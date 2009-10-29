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

#include "TGo4EventCalibration.h"

#include "Riostream.h"

#include "TGo4Log.h"

TGo4EventCalibration::TGo4EventCalibration() :
   TGo4Parameter()
{
}

TGo4EventCalibration::TGo4EventCalibration(const char* name) :
   TGo4Parameter(name,"This is a Go4 event calibration object")
{
TRACE((15,"TGo4EventCalibration::TGo4EventCalibration(const char*)",__LINE__, __FILE__));
}


TGo4EventCalibration::~TGo4EventCalibration()
{
TRACE((15,"TGo4EventCalibration::~TGo4EventCalibration()",__LINE__, __FILE__));
}

Bool_t TGo4EventCalibration::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4EventCalibration::UpdateFrom()",__LINE__, __FILE__));
   TGo4EventCalibration* calipar=dynamic_cast<TGo4EventCalibration*>(rhs);
   if(calipar==0) return kFALSE;
   SetName(calipar->GetName());
   cout << "GO4> !!! WARNING: TGo4EventCalibration::UpdateFrom() not implemented in user calibration class !!!" << endl;
   cout << "GO4>     Just updated the object name, please overwrite virtual method !!!" << endl;
   return kTRUE;
}
