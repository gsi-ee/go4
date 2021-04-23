set(Go4EventServerParameter_headers
   TGo4BackStoreParameter.h
   TGo4FileSourceParameter.h
   TGo4FileStoreParameter.h
   TGo4MbsEventServerParameter.h
   TGo4MbsFileParameter.h
   TGo4MbsRandomParameter.h
   TGo4MbsSourceParameter.h
   TGo4MbsStreamParameter.h
   TGo4MbsTransportParameter.h
   TGo4RevServParameter.h
   TGo4TreeSourceParameter.h
   TGo4TreeStoreParameter.h
   TGo4UserSourceParameter.h
   TGo4UserStoreParameter.h
)

set(Go4EventServerParameter_sources
   TGo4BackStoreParameter.cxx
   TGo4FileSourceParameter.cxx
   TGo4FileStoreParameter.cxx
   TGo4MbsEventServerParameter.cxx
   TGo4MbsFileParameter.cxx
   TGo4MbsRandomParameter.cxx
   TGo4MbsSourceParameter.cxx
   TGo4MbsStreamParameter.cxx
   TGo4MbsTransportParameter.cxx
   TGo4RevServParameter.cxx
   TGo4TreeSourceParameter.cxx
   TGo4TreeStoreParameter.cxx
   TGo4UserSourceParameter.cxx
   TGo4UserStoreParameter.cxx
)

GO4_SOURCES(Go4AnalBase HEADERS ${Go4EventServerParameter_headers} SOURCES ${Go4EventServerParameter_sources})

set(Go4EventServer_headers
   TGo4BackStore.h
   TGo4EventHeader10.h
   TGo4EventServerFactory.h
   TGo4FileSource.h
   TGo4FileStore.h
   TGo4GSIEventHeader.h
   TGo4MainTree.h
   TGo4MbsEvent.h
   TGo4MbsEventServer.h
   TGo4MbsFile.h
   TGo4MbsRandom.h
   TGo4MbsSource.h
   TGo4MbsStream.h
   TGo4MbsSubEvent.h
   TGo4MbsTransport.h
   TGo4RevServ.h
   TGo4StepFactory.h
   TGo4SubEventHeader10.h
   TGo4TreeSource.h
   TGo4TreeStore.h
)

set(Go4EventServer_sources
   random-coll.c
   TGo4BackStore.cxx
   TGo4EventServerFactory.cxx
   TGo4FileSource.cxx
   TGo4FileStore.cxx
   TGo4MainTree.cxx
   TGo4MbsEvent.cxx
   TGo4MbsEventServer.cxx
   TGo4MbsFile.cxx
   TGo4MbsRandom.cxx
   TGo4MbsSource.cxx
   TGo4MbsStream.cxx
   TGo4MbsSubEvent.cxx
   TGo4MbsTransport.cxx
   TGo4RevServ.cxx
   TGo4StepFactory.cxx
   TGo4TreeSource.cxx
   TGo4TreeStore.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4EventServer_headers} SOURCES ${Go4EventServer_sources})

GO4_INSTALL_HEADERS(MAKESUB ${Go4EventServerParameter_headers} ${Go4EventServer_headers} Go4EventServer.h Go4EventServerTypes.h)

