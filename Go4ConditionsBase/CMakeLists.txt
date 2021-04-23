set(Go4ConditionBase_headers
   TGo4CondArray.h
   TGo4CondArrayPainter.h
   TGo4CondInside.h
   TGo4Condition.h
   TGo4ConditionPainter.h
   TGo4CounterCond.h
   TGo4LabelPainter.h
   TGo4ListCond.h
   TGo4Marker.h
   TGo4MarkerPainter.h
   TGo4PolyCond.h
   TGo4PolyCondPainter.h
   TGo4PolyCondView.h
   TGo4ShapedCond.h
   TGo4WinCond.h
   TGo4WinCondPainter.h
   TGo4WinCondView.h
)

set(Go4ConditionBase_sources
   TGo4CondArray.cxx
   TGo4CondArrayPainter.cxx
   TGo4Condition.cxx
   TGo4ConditionPainter.cxx
   TGo4CounterCond.cxx
   TGo4LabelPainter.cxx
   TGo4ListCond.cxx
   TGo4Marker.cxx
   TGo4MarkerPainter.cxx
   TGo4PolyCond.cxx
   TGo4PolyCondPainter.cxx
   TGo4PolyCondView.cxx
   TGo4ShapedCond.cxx
   TGo4WinCond.cxx
   TGo4WinCondPainter.cxx
   TGo4WinCondView.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4ConditionBase_headers})

GO4_SOURCES(Go4AnalBase HEADERS ${Go4ConditionBase_headers} SOURCES ${Go4ConditionBase_sources})
