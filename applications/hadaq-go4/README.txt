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


---------------------------------------------
// Event Source for HADES  data format (.hld)
// V 1.10 1-Jun-2012
// Joern Adamczewski-Musch, EE, GSI Darmstadt
// j.adamczewski@gsi.de
---------------------------------------------
Contents:
---------
README.txt
THadaqAnalysis.h  
THadaqEventSource.h  
THadaqParam.h  
THadaqUnpackEvent.h  
THadaqUnpackProc.h  
HadaqLinkDef.h
THadaqAnalysis.cxx  
THadaqEventSource.cxx  
THadaqParam.cxx  
THadaqUnpackEvent.cxx  
THadaqUnpackProc.cxx

Required:
---------
-GO4 analysis framework

Usage:
------
unpack tarball "tar zxvf *.gz" to working directory
Set Go4 environment ". go4login" (at gsi).
Compile user analysis "make".
Run analysis with input file name as "user source", e.g.:
"go4analysis -user /data/bkg2d215.hld"
In GUI, specify "usersource" as input of first step.

Details:
--------
The class THadaqEventsource will read file in hades /*.hld format and provide
each event as mbs event with one single subevent. 
The mbs subevent will contain the full data of the hades event, i.e with all
headers and all subevents.
The full id number of the mbs subevent is the original hades event id number.


The example unpack analyis THadaqUnpackProc.cxx will use all hades subevents found
in the mbs subevent.

The subevents will be processed in ProcessSubevent(Hadaq_Subevent* hadsubevent) subfunction.
Here one can check subevent id and call different unpacking functions.
Currently implemented:
 ProcessTDC() for id 0x8800,  
 ProcessTimeTest() for id 0x368, 
 DumpData() for all other subevents.


Printout of event data can be enabled on the fly by setting value
printEvent=1
in the Go4 parameter container THadaqParam. 


Description of time test analysis:
----------------------------------

Parameter has array deltaChannels[] to set index of reference channel for each channel.
If index is -1, there will be no reference calculated. This is used by time difference calculation,
and when setting up correlation plots.

Parameter has boolean switch doBinCalibration. If true, the binwidth for coarse and fine times
are calibrated assuming uniform time distribution over all bins. Correction binwidth for channel i are
stored in histograms FinebinCorrection_i and CoarsebinCorrection_i.
After calibration is acquired, switch should be set to false.
The coeficcients are kept in histograms and will be retrieved from autosave file.
Initial delta histograms can be cleared and file can be processed again.
Note: do not clear all histograms, but only those in folder Timetest. Binwidth calibration is
in separate histogram folder TimeCalibration.
New parameter switches:
   - continuousCalibration for continuous calibration with double buffering of calibration histograms
   - generalCalibration for calibration from begin of the run, without continuous buffering
   - calibrationPeriod specify number of events between two time calibrations




Changes:
-------

V0.1 15-Jun 2011 
-first compiling version

V0.2 16-Jun 2011 
-first version that can read test data from eventbuilder simulator

V0.3 13-Jul 2011 
Many changes when testing with real data file from tdc:
	- corrected 64 bit padding of event/subevents
	- rearranged treatment of subevents to subfunctions
    - implemented real unpack and histograms/pictures for tdc data 
    
V0.4 6-Sep 2011    
Implemented time resolution measurments for Cahit Ugur    
   - compilation defines to switch on /off different analysis
   - parameter for steering of time channel correlation
   
V0.5 12-Sep 2011    
   - Changes in calibration functions by Cahit Ugur
   - Added double buffer mode for continuous calibration
   - Trigger scaler histogram counts occurence of hit pairs

V0.6 22-Sep 2011    
   - Changes in calibration functions by Cahit Ugur
   - Suppress calculation of calibrated values without initial calibration
   
V0.7 10-Jan 2012    
  - added processing of new format for trb v3
  - extended to treat leading/trailing edges and different trb stations
   
V0.8 3-Mar 2012    
  - added delta time reference to channels in other tdcs  
  
V0.9 April 2012        
  - some mcp histograms added by C.Ugur
  
V1.0 31 May 2012
  - extended to cover multiple trb boards 
  - enable new board decoding in unpacker with #define HAD_USE_MULTITRB in THadaqUnpackEvent.h
  - added setup macro set_HadaqPar.C to change setup without recompiling (gui parameter editor is not usable for 3 dimensional arrays)   
  - some improvements defining the mcp maps
  
V1.1 1 June 2012 
  - added setup array SET_TRBS to use only trb board indices that are available
  - reduced granularity of some coarse time histograms for improved memory consumption. Changed range of coarse time histograms.   
  - bugfix with trb index (begins with 1 in data). index 1 is mapped to 0 in go4!
  
  
    
--------------------JAM 1 June-2012----------------------------------------------------
