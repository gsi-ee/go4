// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 19. 3.2001 by Horst Goeringer
 *********************************************************************
 * rawapigsi.h
 *   entry definitions for client api: GSI extensions
 **********************************************************************
 * 19. 3.2001, H.G.: rfio_gsi_disconnect added
 **********************************************************************
 */

#define USER_API
#define RFILE srawAPIFile

extern int rfio_gsi_disconnect(int);
   /* close connection to remote server */

