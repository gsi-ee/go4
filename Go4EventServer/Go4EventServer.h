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

// master include file to get all go4 event class services

#ifndef GO4EVENTSERVER_H
#define GO4EVENTSERVER_H

#include "TGo4EventFactory.h"
#include "TGo4EventElement.h"

#include "TGo4EventProcessor.h"
#include "TGo4EventStore.h"
#include "TGo4EventSource.h"
#include "TGo4EventProcessorParameter.h"
#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"

#include "Go4EventServerTypes.h"

#include "TGo4MbsEvent.h"
#include "TGo4MbsSubEvent.h"
#include "TGo4MbsFile.h"
#include "TGo4MbsStream.h"
#include "TGo4MbsTransport.h"
#include "TGo4MbsEventServer.h"
#include "TGo4RevServ.h"
#include "TGo4MbsRandom.h"
#include "TGo4MbsFileParameter.h"
#include "TGo4MbsStreamParameter.h"
#include "TGo4MbsTransportParameter.h"
#include "TGo4MbsEventServerParameter.h"
#include "TGo4RevServParameter.h"
#include "TGo4MbsRandomParameter.h"

#include "TGo4FileSource.h"
#include "TGo4FileStore.h"
#include "TGo4BackStore.h"
#include "TGo4TreeSource.h"
#include "TGo4TreeStore.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4FileStoreParameter.h"
#include "TGo4BackStoreParameter.h"
#include "TGo4TreeSourceParameter.h"
#include "TGo4TreeStoreParameter.h"
#include "TGo4UserSourceParameter.h"
#include "TGo4UserStoreParameter.h"

#endif //GO4EVENTSERVER_H
