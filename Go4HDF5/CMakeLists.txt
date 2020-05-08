set(Go4HDF5Parameter_headers
   TGo4HDF5SourceParameter.h
   TGo4HDF5StoreParameter.h
)

set(Go4HDF5Parameter_sources
   TGo4HDF5SourceParameter.cxx
   TGo4HDF5StoreParameter.cxx
)

GO4_SOURCES(Go4AnalysisBase HEADERS ${Go4HDF5Parameter_headers} SOURCES ${Go4HDF5Parameter_sources})

set(Go4HDF5_headers
   TGo4HDF5Adapter.h
   TGo4HDF5DataHandle.h
   TGo4HDF5Source.h
   TGo4HDF5Store.h
)

set(Go4HDF5_sources
   TGo4HDF5Adapter.cxx
   TGo4HDF5DataHandle.cxx
   TGo4HDF5Source.cxx
   TGo4HDF5Store.cxx
)

GO4_SOURCES(Go4Analysis HEADERS ${Go4HDF5_headers} SOURCES ${Go4HDF5_sources})

GO4_INSTALL_HEADERS(${Go4HDF5Parameter_headers} ${Go4HDF5_headers} Go4HDF5.h)

