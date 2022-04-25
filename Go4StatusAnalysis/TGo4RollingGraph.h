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


#ifndef TGO4ROLLINGGRAPH_H
#define TGO4ROLLINGGRAPH_H

#include "TGraphErrors.h"

/** \brief Graphs that renew themselves iteratively to monitor a value
*
* As old points are to be deleted at a certain point, this has the appearance of a rolling counter.
*/
class TGo4RollingGraph : public TGraphErrors
{
   public :
      TGo4RollingGraph();
      TGo4RollingGraph(Int_t NumPoints, Int_t UpdateInterval = 1);

      virtual ~TGo4RollingGraph();

      virtual void   Fill (Double_t value, Double_t xerror=-1, Double_t yerror=-1);
      void   Clear (Option_t *option = "") override;

      Int_t   GetNumPoints () const   { return fiNumPoints; }   ///< Returns number of displayed points
      Int_t   GetUpdateInterval () const   { return fiUpdateInterval; }   ///< Returns interval of entries when a new bin is opened

   protected :
      void   NextBin ();

      Int_t      fiNumPoints{0};        ///< Nominal size of the graph, i.e. number of points to display
      Int_t      fiUpdateInterval{0};   ///< Number of Fill() functions to call before a new average is started
      Long64_t   fiCounter{0};          ///< No. of values filled in total
      Double_t   fdSum{0};              ///< Summed total value in the current Bin
      Double_t   fdSumAveraged{0};      ///< Averaged summed total value in the current Bin

   ClassDefOverride(TGo4RollingGraph, 1);
};

#endif

