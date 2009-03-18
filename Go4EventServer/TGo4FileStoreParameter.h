#ifndef TGO4FILESTOREPARAMETER_H
#define TGO4FILESTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"

class TGo4FileStoreParameter : public TGo4EventStoreParameter {

   public:
      TGo4FileStoreParameter() ;

      TGo4FileStoreParameter(const char* name,
                             Int_t splitlevel=99,
                             Int_t bufsize=32000,
                             Int_t compression=5,
                             Int_t autosave=5000000);

      virtual ~TGo4FileStoreParameter();

      /** basic method to printout status information
        * on stdout; to be overridden by specific subclass */
      virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

      /** update contents of paramter class with external object. */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

      Int_t GetSplitlevel() const { return fiSplit; }
      void SetSplitlevel(Int_t split) { fiSplit=split; }

      Int_t GetBufsize() const { return fiBufsize; }
      void SetBufsize(Int_t bufsize) { fiBufsize=bufsize; }

      Int_t GetCompression() const { return fiCompression; }
      void SetCompression(Int_t comp) { fiCompression=comp; }

      Int_t GetAutosaveSize() const { return fiAutosavesize; }
      void SetAutosaveSize(Int_t bufsize) { fiAutosavesize=bufsize; }

      Int_t IsOverwriteMode() const { return fbOverwrite; }
      void SetOverwriteMode(Bool_t over=kTRUE) { fbOverwrite = over ? 1 : 0; }

   private:

      /** Splitlevel for Tree. Default is 1.*/
      Int_t fiSplit;

      /** Size of the root io buffer. */
      Int_t fiBufsize;

      /** Compression level of the root file. Default is 5. */
      Int_t fiCompression;

      /** Autosave interval for the Tree. */
      Int_t fiAutosavesize;

      /** True if old filestore of same name shall be overwritten.
        * False if old filestore should be opened in append (update) mode.
        * internally, we use Int_t for root streamer problems.*/
      Int_t fbOverwrite;

   ClassDef(TGo4FileStoreParameter,1)
};

#endif //TGO4FILESTOREPARAMETER_H
