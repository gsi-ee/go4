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


   double   p_dBeta (double a,double b, unsigned int *seed);
   int      p_iBinomial (double p,int n,unsigned int *seed);
   double   p_dExponential (double lambda,unsigned int *seed);
   double   p_dGammaInt (int a,unsigned int *seed);
   double   p_dGammaGen (double A,unsigned *seed);
   int      p_iGeometric (double p,unsigned int *seed);
   double   p_dNormal (double mean,double sigma, unsigned int *seed);
   int      p_iPoisson(double mu,unsigned *seed);
   unsigned p_iUniform (unsigned *seed);
   double   p_dUniform(unsigned *seed);
