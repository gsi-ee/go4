// $Id: TXXXUnpackEvent.cxx 478 2009-10-29 12:26:09Z linev $
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

#include "TXXXUnpackEvent.h"


static UInt_t    Config_Crates[XXX_NUM_CRATES] = NR_MODULES;






//***********************************************************
TXXXCrate::TXXXCrate(const char* name,Short_t id) :
   TGo4CompositeEvent(name,name,id)
{
if(id <0 || id>XXX_NUM_CRATES)
	{
		printf("TXXXCrate id %d outside range!\n",id);

	}
else
	{
		TString modname;
		for(UInt_t ix=0; ix<Config_Crates[id]; ++ix)
			{
				modname.Form("XXXCrate%d_XXXModule%d",id,ix);
				addEventElement(new TXXXModule(modname.Data(),ix));
			}
	}

}




//***********************************************************
TXXXUnpackEvent::TXXXUnpackEvent(const char* name) :
   TGo4CompositeEvent(name,name,0)
{
	TString modname;
	for(UInt_t ix=0; ix<XXX_NUM_CRATES; ++ix)
		{
			if(Config_Crates[ix]==0) continue; // skip empty crates
			modname.Form("XXXCrate%d",ix);
			addEventElement(new TXXXCrate(modname.Data(),ix));
		}
}
