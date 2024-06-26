// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef PORTNUM_DEF_H
#define PORTNUM_DEF_H

/*
 *   definitions of the port numbers used
 */

#define PORT__TRANSPORT      6000
#define PORT__ESONE_SERV     6001
#define PORT__STREAM_SERV    6002
#define PORT__EVENT_SERV     6003
#define PORT__PROMPT         6004
#define PORT__MSG_LOG        6005
#define PORT__PROMPT_R       6006
#define PORT__MSG_REM        6007
#define PORT__STAT_SERV      6008
#define PORT__HIST_SERV      6009
#define MO__PORT_BASE        6100 /* m_ds and m_dr processes construct their
                                * necessary ports from this base. It requires,
                                * that ports 6100 - 6256 are reserved (if max
                                * nr of m_ds and m_dr nodes is defined to 16.*/

#endif
