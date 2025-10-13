include(GNUInstallDirs)

if(NOT DEFINED CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR "bin" CACHE PATH "user executables (bin)")
endif()

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  if(gnuinstall)
    set(CMAKE_INSTALL_LIBDIR "lib/root" CACHE PATH "object code libraries (lib/root)")
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

if(NOT DEFINED CMAKE_INSTALL_DATADIR)
  set(CMAKE_INSTALL_DATADIR "" CACHE PATH "read-only architecture-independent data")
  if(gnuinstall)
    set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}/go4")
  else()
    set(CMAKE_INSTALL_DATADIR ".")
  endif()
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
   if(MSVC)
      set(GO4_INSTALL_MAINDIR "." CACHE PATH "main go4 dir (DATADIR)")
      set(_go4sys "")
   else()
      set(GO4_INSTALL_MAINDIR "${CMAKE_INSTALL_DATADIR}" CACHE PATH "main go4 dir (DATADIR)")
      set(_go4sys "${CMAKE_INSTALL_DATADIR}/")
   endif()
else()
   set(CMAKE_INSTALL_BINDIR "bin" CACHE PATH "go4 executables (bin)")
   set(CMAKE_INSTALL_LIBDIR "lib" CACHE PATH "go4 code libraries (lib)")
   set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "go4 header files (include)")
   if(MSVC)
      set(GO4_INSTALL_MAINDIR "." CACHE PATH "main go4 dir (prefix)")
   else()
      set(GO4_INSTALL_MAINDIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "main go4 dir (prefix)")
   endif()
   set(_go4sys "")
endif()

set(GO4_INSTALL_ICONSDIR "${_go4sys}icons" CACHE PATH "go4 icons (icons/)")
set(GO4_INSTALL_MACROSDIR "${_go4sys}macros" CACHE PATH "go4 macros (macros/)")
set(GO4_INSTALL_ETCDIR "${_go4sys}etc" CACHE PATH "go4 etc files (etc/)")
set(GO4_INSTALL_DOCSDIR "${_go4sys}docs" CACHE PATH "go4 docs (docs/)")
set(GO4_INSTALL_BUILDDIR "${_go4sys}build" CACHE PATH "go4 build support (build/)")
set(GO4_INSTALL_HTMLDIR "${_go4sys}html" CACHE PATH "go4 html/js files (html/)")
set(GO4_INSTALL_HTML5DIR "${_go4sys}html5" CACHE PATH "go4 new html/js files (html5/)")
set(GO4_INSTALL_PYTHONDIR "${_go4sys}python" CACHE PATH "go4 python files (python/)")


# Result directories
#
foreach(dir MAINDIR)
  if(NOT IS_ABSOLUTE ${GO4_INSTALL_${dir}})
    set(GO4_INSTALL_FULL_${dir} "${CMAKE_INSTALL_PREFIX}/${GO4_INSTALL_${dir}}")
  else()
    set(GO4_INSTALL_FULL_${dir} "${GO4_INSTALL_${dir}}")
  endif()
endforeach()

mark_as_advanced(
  CMAKE_INSTALL_BINDIR
  CMAKE_INSTALL_LIBDIR
  CMAKE_INSTALL_INCLUDEDIR
  CMAKE_INSTALL_DATADIR
  GO4_INSTALL_MAINDIR
  GO4_INSTALL_FULL_MAINDIR
  GO4_INSTALL_ICONSDIR
  GO4_INSTALL_MACROSDIR
  GO4_INSTALL_ETCDIR
  GO4_INSTALL_DOCSDIR
  GO4_INSTALL_BUILDDIR
  GO4_INSTALL_HTMLDIR
  GO4_INSTALL_HTML5DIR
  GO4_INSTALL_PYTHONDIR
)
