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

#ifndef TGO4STATUS_H
#define TGO4STATUS_H

#include "TNamed.h"

class TGo4Status : public TNamed {
   public:

      TGo4Status();

      TGo4Status(const char* name);

      TGo4Status(const char* name, const char* title);

      virtual ~TGo4Status();

      /** DEPRECATED - please use Print(Option_t*) signature
        * basic method to printout status information
        * on stdout; to be overridden by specific subclass  */
      virtual Int_t PrintStatus(Text_t *buffer = nullptr, Int_t buflen = 0);

      void Print(Option_t * dummy="") const override;

      /**
       * Insert indentation blanks into text buffer. Uses TROOT::DirLevel,
       * but extends IndentLevel to any text buffer. Return value is
       * buffer cursor pointer after insertion. Returns 0 if cursor exceeds
       * buffer length. */
      static Text_t* PrintIndent(Text_t* buffer, Int_t& buflen);

      /**
       * Print text with variable expression into buffer. Returns cursor pointer
       * in buffer, i.e. the next character after the printed text.
       * If cursor would exceed buflen, 0 is returned. Bufferlength
       * buflen shall contain buffer size, it will be decremented by the size of
       * printed text.  */
      static Text_t* PrintBuffer(char* buffer, Int_t& buflen, const char* text, ...)
      #if defined(__GNUC__) && !defined(__CINT__)
        __attribute__((format(printf, 3, 4)))
      #endif
      ;

      /**
       * Print single line of debug output with appropriate indent */
      static void PrintLine(const char *text, ...)
      #if defined(__GNUC__) && !defined(__CINT__)
        __attribute__((format(printf, 1, 2)))
      #endif
      ;


    /** Go4 status bits to be used in object manager to suppress
     * resetting or deleting histograms, etc */
    enum {
       kGo4NoReset        = BIT(20),  // don't reset this object
       kGo4CanDelete      = BIT(21),  // may delete this object
       kGo4BackStoreReset = BIT(22)   // set for tree that belongs to backstore
    };

   ClassDefOverride(TGo4Status,2)
};

#endif //TGO4STATUS_H
