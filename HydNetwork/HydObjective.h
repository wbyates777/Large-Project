/* HydObjective 01/03/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydObjective.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 The objective functions to optimise - defined on a HydNetwork
 
*/


#ifndef __HYDOBJECTIVE_H__
#define __HYDOBJECTIVE_H__

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif



class HydNetwork;

class HydObjective
{
public:

    static HydFloat
    resilience( HydNetwork* hydNet, int scenario_index = 0 );
    
    static HydFloat
    cost( HydNetwork* hydNet );
    
    static std::pair<HydFloat,HydFloat>
    getMinMaxCost( HydNetwork* hydNet );
    
    // note this method calls setHeadDeficit/setHeadExcess in hydNet
    static HydFloat
    headDeficit( HydNetwork* hydNet, int scenario_index = 0 );
    
private:

    HydObjective( void )=delete;
    ~HydObjective( void )=delete;
    
};

#endif


