/*
***********************************************************************
* vesselparameter.hpp:
* function to define the model properties of each vessel
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _VESSELPARAMETER_H_
#define _VESSELPARAMETER_H_

/* model properties of vessel 1
*********************************************************************** */
struct vessel {
  const int m;
  const int n = 3;
  const int nz = 2 * m + n;
  const int num_constraints = 4 * m;  // the number of constraints
  const int nmu = n;
  const int ny = nz + nmu + num_constraints;

  // absolute value of max generalized force in the each direction
  const double max_positive_forcex = 3;
  const double max_negative_forcex = 2;
  const double max_positive_forcey = 2;
  const double max_negative_forcey = 2;
  const double max_positive_momentz = 2;
  const double max_negative_momentz = 2;
};

#endif /*_VESSELPARAMETER_H_ */
