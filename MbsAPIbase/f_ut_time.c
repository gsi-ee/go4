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

#include "typedefs.h"
/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                               */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : f_ut_time                                     */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : CHARS * = f_ut_time(CHARS *)                          */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Returns date/time string in format                  */
/*                  day-month-year hours:min:sec                      */
/*                must be linked on Lynx with -X (=posix library)     */
/*                because of the time correction done.                */
/*                On VMS with /PREF=ALL                               */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                                                                    */
/*                Length of returned string is 17. No \n.             */
/*                                                                    */
/*3+Examples******+***********+****************************************/
/*                Description                                         */
/*2+Implementation************+****************************************/
/*+ Utility     : util                                                */
/*+ File name   : f_ut_time.c                                         */
/*+ Home direct.: path                                                */
/*+ Declaration : CHARS *f_ut_time(CHARS *);                            */
/*+ Version     : 1.01                                                */
/*+ Author      : H.G.Essel                                           */
/*+ Created     : 24-Mar-1994                                         */
/*+ Object libr.:                                                     */
/*+ Updates     : Date        Purpose                                 */
/*-             : 03-apr-97 : support VMS, AIX, DECunix, Lynx         */
/*1- C Procedure *************+****************************************/

#include <stdio.h>
#include <time.h>

CHARS *f_ut_time (CHARS *pc_time)
{
  struct timespec tp;
  struct tm st_time;
#ifndef WIN32
  struct tm buf_time;
#endif

  clock_gettime(CLOCK_REALTIME, &tp);

#ifdef WIN32
  st_time = *localtime(&tp.tv_sec);
#else
  st_time = *localtime_r(&tp.tv_sec, &buf_time);
#endif

  strftime(pc_time,30,"%d-%h-%y %T",&st_time);
  return ((CHARS *) pc_time);
}
