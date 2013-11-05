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

#ifndef TGO4COMADDTREEHISTOGRAM_H
#define TGO4COMADDTREEHISTOGRAM_H

#include "TGo4AnalysisCommand.h"
#include "TString.h"

/**
 * Adds a tree-histogram entry into dynamic list.
 * Tree will be searched by name and linked
 * to that entry.
 * If histogram of given name does not exist, it
 * will be created on first TTree::Draw. Varexp
 * string and TCut is used to define what to draw.
 * Tree will be processed event by event within
 * the analysis MainCycle.
 * @author J. Adamczewski
 * @since 01-Nov-2001
 */

class TGo4ComAddTreeHistogram : public TGo4AnalysisCommand {
   public:

      TGo4ComAddTreeHistogram();

      TGo4ComAddTreeHistogram(const char* histogramname, const char* treename,const char* varexp, const char* cut);

      virtual ~TGo4ComAddTreeHistogram();

      virtual Int_t ExeCom();

      /** Set internals of this command from specification of external
       * remote command (command container). Must be overloaded
       * for each specific command! */
      virtual void Set(TGo4RemoteCommand* remcon);

      /** Set histogram (dynamic entry) name. */
      void SetHistogramName(const char* name) { fxHistoName = name; }

      /** Set tree name. */
      void SetTreeName(const char* name) { fxTreeName = name; }

      /** Set dynamic entry index name. */
      void SetVarexp(const char* exp) { fxVarexp = exp; }

      /** Set TCut to be applied on this tree histogram. */
      void SetCut(const char* cut) { fxCut = cut; }

   private:

      /** Name of the histogram for dynamic list. */
      TString fxHistoName;

      /** Name of Tree linked to that histogram. */
      TString fxTreeName;

      /** Variable expression defining what to draw in the histogram. */
      TString fxVarexp;

      /** String with cut expression. */
      TString fxCut;

   ClassDef(TGo4ComAddTreeHistogram,1)
};

#endif //TGO4COMADDTREEHISTOGRAM_H
