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

#include "TGo4DrawObjProxy.h"

#include "TList.h"
#include "THStack.h"
#include "TMultiGraph.h"

TGo4DrawObjProxy::TGo4DrawObjProxy(TObject* obj, Bool_t owner, Bool_t doclear) :
   TGo4ObjectProxy(obj, owner),
   fDoClear(doclear)
{
}

TGo4DrawObjProxy::~TGo4DrawObjProxy()
{
  if (fDoClear) TryToClearObject();
}

Bool_t TGo4DrawObjProxy::RemoveRegisteredObject(TObject* obj)
{
   if (obj == fObject) TryToClearObject();
   return TGo4ObjectProxy::RemoveRegisteredObject(obj);
}

void TGo4DrawObjProxy::TryToClearObject()
{
   if (fObject==0) return;
   if (fObject->InheritsFrom(THStack::Class())) {
      THStack* hs = (THStack*) fObject;
      if (hs->GetHists())
          hs->GetHists()->Clear();
   } else
   if (fObject->InheritsFrom(TMultiGraph::Class())) {
      TMultiGraph* mgr = (TMultiGraph*) fObject;
      if (mgr->GetListOfGraphs())
         mgr->GetListOfGraphs()->Clear();
    }
}
