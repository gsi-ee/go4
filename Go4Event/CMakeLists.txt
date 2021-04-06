set(Go4EventParameter_headers
   TGo4EventProcessorParameter.h
   TGo4EventSourceParameter.h
   TGo4EventStoreParameter.h
)

set(Go4EventParameter_sources
   TGo4EventProcessorParameter.cxx
   TGo4EventSourceParameter.cxx
   TGo4EventStoreParameter.cxx
)

GO4_SOURCES(Go4AnalBase HEADERS ${Go4EventParameter_headers} SOURCES ${Go4EventParameter_sources})

set(Go4Event_headers
   TGo4CompositeEvent.h
   TGo4CompositeProcessor.h
   TGo4EventCalibration.h
   TGo4EventElement.h
   TGo4EventEndException.h
   TGo4EventErrorException.h
   TGo4EventFactory.h
   TGo4EventProcessor.h
   TGo4EventSourceException.h
   TGo4EventSource.h
   TGo4EventStoreException.h
   TGo4EventStore.h
   TGo4EventTimeoutException.h
)

set(Go4Event_sources
   TGo4CompositeEvent.cxx
   TGo4CompositeProcessor.cxx
   TGo4EventCalibration.cxx
   TGo4EventElement.cxx
   TGo4EventEndException.cxx
   TGo4EventErrorException.cxx
   TGo4EventFactory.cxx
   TGo4EventProcessor.cxx
   TGo4EventSource.cxx
   TGo4EventSourceException.cxx
   TGo4EventStore.cxx
   TGo4EventStoreException.cxx
   TGo4EventTimeoutException.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4Event_headers} SOURCES ${Go4Event_sources})

GO4_INSTALL_HEADERS(${Go4EventParameter_headers} ${Go4Event_headers})

