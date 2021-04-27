
if(NOT DEFINED CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR "bin" CACHE PATH "user executables (bin)")
endif()

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  if(gnuinstall)
    set(CMAKE_INSTALL_LIBDIR "lib/go4" CACHE PATH "object code libraries (lib/go4)")
  else()
    set(CMAKE_INSTALL_LIBDIR "lib" CACHE PATH "object code libraries (lib)")
  endif()
endif()

if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  if(gnuinstall)
    set(CMAKE_INSTALL_INCLUDEDIR "include/go4" CACHE PATH "C header files (include/go4)")
  else()
    set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "C header files (include)")
  endif()
endif()

if(NOT CMAKE_INSTALL_ICONSDIR)
  set(CMAKE_INSTALL_ICONSDIR "" CACHE PATH "icons (DATADIR/icons)")
  if(gnuinstall)
    set(CMAKE_INSTALL_ICONSDIR "${CMAKE_INSTALL_DATADIR}/icons")
  else()
    set(CMAKE_INSTALL_ICONSDIR "icons")
  endif()
endif()

if(NOT CMAKE_INSTALL_MACROSDIR)
  set(CMAKE_INSTALL_MACROSDIR "" CACHE PATH "macros (DATADIR/macros)")
  if(gnuinstall)
    set(CMAKE_INSTALL_MACROSDIR "${CMAKE_INSTALL_DATADIR}/macros")
  else()
    set(CMAKE_INSTALL_MACROSDIR "macros")
  endif()
endif()

if(NOT CMAKE_INSTALL_ETCDIR)
  set(CMAKE_INSTALL_ETCDIR "" CACHE PATH "etc (DATADIR/etc)")
  if(gnuinstall)
    set(CMAKE_INSTALL_ETCDIR "${CMAKE_INSTALL_DATADIR}/etc")
  else()
    set(CMAKE_INSTALL_ETCDIR "etc")
  endif()
endif()

if(NOT CMAKE_INSTALL_DOCSDIR)
  set(CMAKE_INSTALL_DOCSDIR "" CACHE PATH "docs (DATADIR/docs)")
  if(gnuinstall)
    set(CMAKE_INSTALL_DOCSDIR "${CMAKE_INSTALL_DATADIR}/docs")
  else()
    set(CMAKE_INSTALL_DOCSDIR "docs")
  endif()
endif()

if(NOT CMAKE_INSTALL_BUILDDIR)
  set(CMAKE_INSTALL_BUILDDIR "" CACHE PATH "build (DATADIR/build)")
  if(gnuinstall)
    set(CMAKE_INSTALL_BUILDDIR "${CMAKE_INSTALL_DATADIR}/build")
  else()
    set(CMAKE_INSTALL_BUILDDIR "build")
  endif()
endif()

if(NOT CMAKE_INSTALL_HTMLDIR)
  set(CMAKE_INSTALL_HTMLDIR "" CACHE PATH "html (DATADIR/html)")
  if(gnuinstall)
    set(CMAKE_INSTALL_HTMLDIR "${CMAKE_INSTALL_DATADIR}/html")
  else()
    set(CMAKE_INSTALL_HTMLDIR "html")
  endif()
endif()

if(NOT CMAKE_INSTALL_MAINDIR)
  set(CMAKE_INSTALL_MAINDIR "" CACHE PATH "main dir (DATADIR)")
  if(gnuinstall)
    set(CMAKE_INSTALL_MAINDIR "${CMAKE_INSTALL_DATADIR}")
  else()
    set(CMAKE_INSTALL_MAINDIR "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

mark_as_advanced(
  CMAKE_INSTALL_BINDIR
  CMAKE_INSTALL_LIBDIR
  CMAKE_INSTALL_INCLUDEDIR
  CMAKE_INSTALL_ICONSDIR
  CMAKE_INSTALL_MACROSDIR
  CMAKE_INSTALL_ETCDIR
  CMAKE_INSTALL_DOCSDIR
  CMAKE_INSTALL_BUILDDIR
  CMAKE_INSTALL_HTMLDIR
  CMAKE_INSTALL_MAINDIR
)


