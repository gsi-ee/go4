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

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// go4 commands for analysis framework
#pragma link C++ class TGo4AnalysisCommand+;
#pragma link C++ class TGo4ComInitAnalysis+;
#pragma link C++ class TGo4ComCloseAnalysis+;
#pragma link C++ class TGo4ComGetNamesList+;

#pragma link C++ class TGo4AnalysisObjectCommand+;
#pragma link C++ class TGo4ComClearObject+;
#pragma link C++ class TGo4ComDeleteObject+;
#pragma link C++ class TGo4ComGetEnvelope+;
#pragma link C++ class TGo4ComSetProtections+;
#pragma link C++ class TGo4ComGetCurrentEvent+;
#pragma link C++ class TGo4ComSetPrintEvent+;
#pragma link C++ class TGo4ComSetObject+;

#pragma link C++ class TGo4ComGetObjectStatus+;

#pragma link C++ class TGo4ComGetAnalysisStatus+;
#pragma link C++ class TGo4ComSetAnalysisStatus+;
#pragma link C++ class TGo4ComLoadAnalysisStatus+;
#pragma link C++ class TGo4ComSaveAnalysisStatus+;

#pragma link C++ class TGo4ComAddTreeHistogram+;

#pragma link C++ class TGo4ComPrintHistograms+;
#pragma link C++ class TGo4ComPrintConditions+;
#pragma link C++ class TGo4ComPrintDynList+;
#pragma link C++ class TGo4ComAutosave+;

#endif
