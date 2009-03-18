#ifndef TGO4DYNAMICENTRY_H
#define TGO4DYNAMICENTRY_H

#include "TNamed.h"

/** ABC for all entries that can be kept in a dynamic list.
 * The Dynamic list itself is a dynamic entry, too; thus
 * the analysis can handle a dynamic list containing
 * dynamic sub-lists of histograms.
 * @interface
 * @author J. Adamczewski
 * @since 1/2001 */

class TGo4DynamicEntry : public TNamed {

   public:

    /** default ctor for streamer. */
    TGo4DynamicEntry();

    TGo4DynamicEntry(const char* name, const char* title = "This is a Go4 dynamic entry");

    virtual ~TGo4DynamicEntry();

    /** Switch the autoprocessing state of this entry. */
    void EnableProcessing(Bool_t on=kTRUE) { fbProcessEntry=on; }
    Bool_t IsEnabledProcessing() const { return fbProcessEntry; }

    /** Resets this entry to an initial status. */
    virtual void Reset() {}

  protected:

    /** True if object shall be processed by main event loop automatically.
      * For example, histogram autofill from event index values. */
    Bool_t fbProcessEntry;

  ClassDef(TGo4DynamicEntry,3)
};

#endif //TGO4DYNAMICENTRY_H
