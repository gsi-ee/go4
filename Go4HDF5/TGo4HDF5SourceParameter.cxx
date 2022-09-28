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

#include "TGo4HDF5SourceParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4HDF5SourceParameter::TGo4HDF5SourceParameter(const char *name)
: TGo4EventSourceParameter(name,  GO4EV_HDF5)
{
  GO4TRACE((14,"TGo4HDF5SourceParameter::TGo4HDF5SourceParameter(const char*,...)", __LINE__, __FILE__));
}

TGo4HDF5SourceParameter::TGo4HDF5SourceParameter()
: TGo4EventSourceParameter("Default Go4 HDF5 source",  GO4EV_HDF5)
{
  GO4TRACE((14,"TGo4HDF5SourceParameter::TGo4HDF5SourceParameter()", __LINE__, __FILE__));
}


TGo4HDF5SourceParameter::~TGo4HDF5SourceParameter()
{
   GO4TRACE((14,"TGo4HDF5SourceParameter::~TGo4HDF5SourceParameter()", __LINE__, __FILE__));
}
