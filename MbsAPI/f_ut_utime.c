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

#include "f_ut_utime.h"

/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : f_ut_utime                                         */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : char * = f_ut_utime(l_sec,l_msec,char *pc_time)    */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Returns date/time string in VMS format              */
/*                  day-month-year hours:min:sec.00                   */
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+   l_sec     : Seconds as returned from ftime function             */
/*+   l_msec    : Milliseconds                                        */
/*+  pc_time    : Address of string (23 characters)                   */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ Utility     : util                                                */
/*+ File name   : f_ut_utime.c                                       */
/*+ Home direct.: path                                                */
/*+ Declaration : char *f_ut_utime(char *);                          */
/*+ Version     : 1.01                                                */
/*+ Author      : H.G.Essel                                           */
/*+ Created     : 24-Mar-1994                                         */
/*+ Object libr.:                                                     */
/*+ Updates     : Date        Purpose                                 */
/*-               14-apr-97 : POSIX now OK on Lynx /HE                */
/*                            gcc -mposix4d9 -mthreads -DLynx         */
/*1- C Procedure *************+****************************************/

#include <stdio.h>
#include <string.h>
#include "f_ut_time.h"

INTS4 f_ut_utime(INTS4 l_sec, INTS4 l_msec, CHARS *pc_time)
{
  // struct timeb tp;
  struct timespec tp;
  struct tm st_time;
#ifndef _MSC_VER
  struct tm buf_time;
#endif
  CHARS c_allmon[37]="JanFebMarAprMayJunJulAugSepOctNovDec";
  CHARS c_mon[4];
  CHARS *pc_mon;

  if(l_sec == 0) {
    strcpy(pc_time,"no valid date");
    return(0);
  }
  *pc_time=0;
  if (l_sec < 0) {
     clock_gettime(CLOCK_REALTIME, &tp);
     l_msec = tp.tv_nsec / 1000000;
  } else {
    tp.tv_sec = l_sec;
  }
#ifdef _MSC_VER
  st_time = *localtime(&tp.tv_sec);
#else
  st_time = *localtime_r(&tp.tv_sec, &buf_time);
#endif
  pc_mon = (CHARS *) &c_allmon;
  pc_mon += (st_time.tm_mon * 3);
  strncpy(c_mon,pc_mon,3);
  c_mon[3]='\0';
  if(st_time.tm_year < 100)
  sprintf(pc_time,"%02d-%s-19%02d %02d:%02d:%02d.%02d"
    ,st_time.tm_mday
    ,c_mon
    ,st_time.tm_year
    ,st_time.tm_hour
    ,st_time.tm_min
    ,st_time.tm_sec
    ,l_msec/10);
  else
  sprintf(pc_time,"%02d-%s-20%02d %02d:%02d:%02d.%02d"
    ,st_time.tm_mday
    ,c_mon
    ,st_time.tm_year-100
    ,st_time.tm_hour
    ,st_time.tm_min
    ,st_time.tm_sec
    ,l_msec/10);
  return(0);
}
