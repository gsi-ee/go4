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

#ifndef TGO4MBSHIST_H
#define TGO4MBSHIST_H

#include "TObject.h"
#include "Htypes.h"

extern "C"
{
#include "s_his_head.h"
}

class TFolder;
class TH1;

/**
 * Object container for the mbs
 * histogram buffer structure.
 */
class TGo4MbsHist : public TObject {
   public:

      TGo4MbsHist();

      /** Create queue object from a single histogram
        *   in this case, histogram header will fill s_his_head,
        * and data is copied to fiBuffer */
      TGo4MbsHist(TH1* histo);

      /** Create queue object with header information from a folder of histograms.
        * headers of histograms will be put into fiBuffer, the s_his_head is empty.
        * String filter gives expression to filter out histograms by name. */
      TGo4MbsHist(TFolder* histofolder, const char* filter=0);

      virtual ~TGo4MbsHist();

      s_his_head* GetHead() { return &fxHistoHead; }
      Int_t* GetBuffer() { return fiBuffer; }
      Int_t GetBufLen() const { return fiBufLen; }
      Int_t GetHisNum() const { return fiHisNum; }

      /** Default buffer length for list of histograms */
      static const Int_t fgiLISTLEN;  //!

   private:

      /** extract header information from histogram to mbs header structur */
      void PrepareHeader(TH1* source, const char* path, s_his_head* target);

      /** Scan go4 folders containing histograms and subfolders of histograms
        *  header information is exctracted to buffer. filter may be applied*/
      void ScanGo4Folder(TFolder* fold, const char* superfolders=0, const char* filter=0);

      /** set value to adress in histogram buffer. Check
        * histogram format in header for correct cast to int or float */
      void SetValue(char* address, Stat_t value);

      s_his_head fxHistoHead;
      Int_t   fiBufLen;
      Int_t*  fiBuffer; //!

      /* Points to next histogram header in buffer in case of nameslist */
      s_his_head* fxCursor; //!

      /** Number of histogram headers (1 in case of single his) */
      Int_t fiHisNum;
};

#endif //TGO4MBSHIST_H
