/* ReturnFunctions 14/08/2017

 $$$$$$$$$$$$$$$$$$$$$$$$$
 $   ReturnFunctions.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __RETURNFUNCTIONS_H__
#define __RETURNFUNCTIONS_H__

#include <cmath>

inline double
logRet( double new_obj, double old_obj ) { return log10(new_obj/old_obj); }

inline double
decRet( double new_obj, double old_obj ) { return (new_obj - old_obj) / old_obj; }

#endif


