set(MbsAPIbase_headers
   err_mask_def.h
   errnum_def.h
   f_his_hist.h
   f_his_swpbas.h
   f_his_toupper.h
   f_stccomm.h
   f_swaplw.h
   f_ut_compress.h
   f_ut_status.h
   ml_def.h
   portnum_def.h
   sbs_def.h
   s_daqst.h
   s_daqst_v2.h
   s_endian.h
   s_head.h
   s_his_comm.h
   s_his_head.h
   s_set_ml.h
   s_set_mo.h
   s_setup.h
   s_stdint.h
   sys_def.h
   typedefs.h
   typedefs_nt.h
)

set(MbsAPIbase_sources
   f_his_hist.c
   f_his_swpbas.c
   f_his_toupper.c
   f_mbs_status.c
   f_stccomm.c
   f_swaplw.c
   f_ut_compress.c
   f_ut_seg_show.c
   f_ut_status.c
   f_ut_time.c
)

GO4_INSTALL_HEADERS(${MbsAPIbase_headers})

GO4_SOURCES(Go4AnalysisBase SOURCES ${MbsAPIbase_sources})