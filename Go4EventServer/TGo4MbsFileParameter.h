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

#ifndef TGO4MBSFILEPARAMETER_H
#define TGO4MBSFILEPARAMETER_H

#include "TGo4MbsSourceParameter.h"

#include "TString.h"
#include "TObjArray.h"

/** @brief MBS file parameter
 * @ingroup go4_event
 */

class TGo4MbsFileParameter : public TGo4MbsSourceParameter {
  public:
    TGo4MbsFileParameter();

    TGo4MbsFileParameter(const char *name);

    virtual ~TGo4MbsFileParameter();

    /** Name of the Tagfile */
    const char *GetTagName() const { return fxTagFile.Data(); }

    /** Name of the Tagfile */
    void SetTagName(const char *name) { fxTagFile = name; }

    void Print(Option_t *opt = "") const override;

    /** update contents of parameter class with external object. */
    Bool_t UpdateFrom(TGo4Parameter *rhs) override;

    /** Add more file names in to the parameter */
    void AddMoreFile(const char *more);

    /** Number of more file names in parameter */
    Int_t NumMoreFiles() const { return fxMoreFiles.GetLast() + 1; }

    /** Returns name of extra files names, configured in the parameter */
    const char *GetMoreName(Int_t n) const;

  private:

    /** Name of the tagfile associated with the listmode event file. */
    TString fxTagFile;

    /** List of extra files names, specified in the go4analysis */
    TObjArray fxMoreFiles;

  ClassDefOverride(TGo4MbsFileParameter, 3)

};

#endif //TGO4MBSFILEPARAMETER_H
