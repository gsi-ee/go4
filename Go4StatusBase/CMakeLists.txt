set(Go4StatusBase_headers
   TGo4BranchStatus.h
   TGo4MemberStatus.h
   TGo4ObjectStatus.h
   TGo4Parameter.h
   TGo4ParameterMember.h
   TGo4ParameterStatus.h
   TGo4Picture.h
   TGo4Status.h
)

set(Go4StatusBase_sources
   TGo4BranchStatus.cxx
   TGo4MemberStatus.cxx
   TGo4ObjectStatus.cxx
   TGo4Parameter.cxx
   TGo4ParameterMember.cxx
   TGo4ParameterStatus.cxx
   TGo4Picture.cxx
   TGo4Status.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4StatusBase_headers})

GO4_SOURCES(Go4Base HEADERS ${Go4StatusBase_headers} SOURCES ${Go4StatusBase_sources})
