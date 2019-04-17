#ifndef ROOT_TWebCanvasFull
#define ROOT_TWebCanvasFull

#include "TWebCanvas.h"

#include <vector>

class TWebObjectOptions1 {
public:
   std::string snapid;       ///< id of the object
   std::string opt;          ///< drawing options
   std::string fcust;        ///< custom string
   std::vector<double> fopt; ///< custom float array
};

/// Class used to transport ranges from JSROOT canvas
class TWebPadRange1 {
public:
   std::string snapid;                        ///< id of pad
   bool active{false};                        ///< if pad selected as active
   int logx{0}, logy{0}, logz{0};             ///< pad log properties
   int gridx{0}, gridy{0};                    ///< pad grid properties
   int tickx{0}, ticky{0};                    ///< pad ticks properties
   Float_t mleft{0}, mright{0}, mtop{0}, mbottom{0}; ///< frame margins
   bool ranges{false};                        ///< if true, pad has ranges
   Double_t px1{0}, py1{0}, px2{0}, py2{0};   ///< pad range
   Double_t ux1{0}, uy1{0}, ux2{0}, uy2{0};   ///< pad axis range
   unsigned bits{0};                          ///< canvas status bits like tool editor
   std::vector<TWebObjectOptions1> primitives; ///< drawing options for primitives
};


/////////////////////////////////////////////////////////

/// Class used to transport pad click events
class TWebPadClick1 {
public:
   std::string padid;                         ///< id of pad
   std::string objid;                         ///< id of clicked object, "null" when not defined
   int x{-1};                                 ///< x coordinate of click event
   int y{-1};                                 ///< y coordinate of click event
   bool dbl{false};                           ///< when double-click was performed
};

class TWebCanvasFull : public TWebCanvas {
public:
   TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
};


#endif
