/* HHFunc 14/01/2021

 $$$$$$$$$$$$$$$$
 $   HHFunc.h   $
 $$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Some example definitions of HHObjFunc defs which can be passed to HH to 
 calculate the scalar variable that is to be optimised from a vector of objective
 function values.
 
 If these functions don't match your needs you should define you own HHObjFunc
 

 For example
 
 ObjType 
 wdn_paper(const HHObjective& objs) 
 { 
     return (objs[0] * 200.0) + (1000.0 * objs[1]) + ((objs[2]) * 5.0);

 }
 
*/


#ifndef __HHFUNC_H__
#define __HHFUNC_H__

#ifndef __HHTYPES_H__
#include "HHTypes.h"
#endif


#include <functional>
#include <numeric>

//
// The type of a function to combine or convert objective values into a single value
//

typedef std::function<ObjType(const HHObjective&)> HHObjFunc;

//
// 
//


class HHFunc
{
public:

    
    
    /// Some HHObjFunc defs for combining objective values into a single value
    static ObjType 
    volume(const HHObjective& objs) 
    { 
        return std::accumulate(objs.begin(), objs.end(), (ObjType) 1, std::multiplies<ObjType>());
    }
    
    static ObjType 
    sum(const HHObjective& objs) 
    { 
        return std::accumulate(objs.begin(), objs.end(), (ObjType) 0, std::plus<ObjType>());
    }
    
    static ObjType 
    single_obj(const HHObjective& objs) 
    { 
        return objs[0];
    }
    
    static HHObjFunc
    param_volume( const std::vector<ObjType>& params )
    {
        return std::bind(&p_volume, std::placeholders::_1, params); 
    }
    
    static HHObjFunc
    param_sum( const std::vector<ObjType>& params )
    {
        return std::bind(&p_sum, std::placeholders::_1, params); 
    }
    

    
private:
    
    
    HHFunc(void )=delete;
    ~HHFunc( void )=delete;
    
    static ObjType 
    p_volume(HHObjective objs, const std::vector<ObjType>& params) 
    // i.e return (1.0 + objs[0]) * (1.0 + objs[1]) * (10.0 + objs[2]);
    { 
        std::transform( objs.begin(), objs.end(), params.begin(), objs.begin(),
                        [](const ObjType& v,const ObjType& p)->ObjType { return v + p; } ); 
        
        return std::accumulate(objs.begin(), objs.end(), (ObjType) 1, std::multiplies<ObjType>());
    }
    
    static ObjType 
    p_sum(HHObjective objs, const std::vector<ObjType>& params) 
    // i.e return (objs[0] * 200.0) + (1000.0 * objs[1]) + ((objs[2]) * 5.0);
    { 
        std::transform( objs.begin(), objs.end(), params.begin(), objs.begin(),
                        [](const ObjType& v,const ObjType& p)->ObjType { return v * p; } ); 
        
        return std::accumulate(objs.begin(), objs.end(), (ObjType) 0, std::plus<ObjType>());
    }
    
};


#endif


