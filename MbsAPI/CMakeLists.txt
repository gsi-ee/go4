set(MbsAPI_headers
   clnt_buf_def.h
   f_evcli.h
   f_evt.h
   fLmd.h
   f_radware.h
   f_ut_utime.h
   gps_sc_def.h
   s_bufhe.h
   s_bufhe_swap.h
   s_clntbuf.h
   s_clnt_filter.h
   s_clnt_filter_swap.h
   s_evhe.h
   s_evhe_swap.h
   s_filhe.h
   s_filhe_swap.h
   s_filter.h
   s_flt_descr.h
   s_flt_descr_swap.h
   sMbs.h
   s_opc1.h
   s_pat.h
   s_spe.h
   s_stdint.h
   s_ve10_1.h
   s_ve10_1_swap.h
   s_ves10_1.h
   s_ves10_1_swap.h
)

set(MbsAPI_sources
   f_evcli.c
   f_evt.c
   fLmd.c
   f_radware.c
   f_ut_utime.c
)

GO4_INSTALL_HEADERS(MAKESUB ${MbsAPI_headers})

GO4_SOURCES(Go4Analysis SOURCES ${MbsAPI_sources})
