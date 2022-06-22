#include <stdio>
{
   TString expression = "*1";
   TString searchfolder = "Go4";
   TObject *result = nullptr;
   Bool_t reset = kTRUE;
   while ((result = go4->NextMatchingObject(expression.Data(), searchfolder.Data(), reset)) != nullptr) {
      reset = kFALSE;
      printf("Found result object: %s\n", result->GetName());
      go4->SendObjectToGUI((TNamed *)result);
   }
}
