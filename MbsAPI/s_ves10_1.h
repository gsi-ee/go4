// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef S_VES10_1
#define S_VES10_1

#include "typedefs.h"
/*  ================= GSI VME Subevent header =======================  */

#if MBS_ENDIAN == 1

typedef struct
{
INTS4  l_dlen;   /*  Data length +2 in words */
INTS2 i_subtype;
INTS2 i_type;
CHARS  h_control;   /*  Processor type code */
CHARS  h_subcrate;   /*  Subcrate number */
INTS2 i_procid;   /*  Processor ID [as loaded from VAX] */
} s_ves10_1 ;
  /* ------------------------------------------------------------------ */

#else

typedef struct
{
INTS4  l_dlen;   /*  Data length +2 in words */
INTS2 i_type;
INTS2 i_subtype;
INTS2 i_procid;   /*  Processor ID [as loaded from VAX] */
CHARS  h_subcrate;   /*  Subcrate number */
CHARS  h_control;   /*  Processor type code */
} s_ves10_1 ;

#endif

#endif
