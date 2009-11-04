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

#include "TGo4AnalysisCommandList.h"

#include "TGo4ComInitAnalysis.h"
#include "TGo4ComCloseAnalysis.h"

#include "TGo4AnalysisObjectCommand.h"
#include "TGo4ComClearObject.h"
#include "TGo4ComDeleteObject.h"
#include "TGo4ComGetEnvelope.h"
#include "TGo4ComSetProtections.h"
#include "TGo4ComGetCurrentEvent.h"
#include "TGo4ComSetPrintEvent.h"
#include "TGo4ComSetObject.h"

#include "TGo4ComGetObjectStatus.h"
#include "TGo4ComGetNamesList.h"
#include "TGo4ComGetAnalysisStatus.h"
#include "TGo4ComSetAnalysisStatus.h"
#include "TGo4ComLoadAnalysisStatus.h"
#include "TGo4ComSaveAnalysisStatus.h"

#include "TGo4ComAddTreeHistogram.h"

#include "TGo4ComPrintConditions.h"
#include "TGo4ComPrintHistograms.h"
#include "TGo4ComPrintDynList.h"
#include "TGo4ComAutosave.h"

TGo4AnalysisCommandList::TGo4AnalysisCommandList() :
   TGo4TaskHandlerCommandList("Go4 Analysis Framework Command list")
{
    // add all application specific commands here:

    AddCommand (new TGo4ComInitAnalysis);
    AddCommand (new TGo4ComCloseAnalysis);

    AddCommand (new TGo4ComGetAnalysisStatus);
    AddCommand (new TGo4ComSetAnalysisStatus);
    AddCommand (new TGo4ComLoadAnalysisStatus);
    AddCommand (new TGo4ComSaveAnalysisStatus);

    AddCommand (new TGo4AnalysisObjectCommand);
    AddCommand (new TGo4ComClearObject);
    AddCommand (new TGo4ComDeleteObject);
    AddCommand (new TGo4ComGetEnvelope);
    AddCommand (new TGo4ComSetProtections);
    AddCommand (new TGo4ComGetCurrentEvent);
    AddCommand (new TGo4ComSetPrintEvent);

    AddCommand (new TGo4ComSetObject);

    AddCommand (new TGo4ComAddTreeHistogram);

    AddCommand (new TGo4ComGetObjectStatus);
    AddCommand (new TGo4ComGetNamesList);

    AddCommand (new TGo4ComPrintConditions);
    AddCommand (new TGo4ComPrintHistograms);
    AddCommand (new TGo4ComPrintDynList);
    AddCommand (new TGo4ComAutosave);
}

TGo4AnalysisCommandList::~TGo4AnalysisCommandList()
{
}
