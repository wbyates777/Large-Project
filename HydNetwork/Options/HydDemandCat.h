/* HydDemandCat 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydDemandCat.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDDEMANDCAT_H_
#define _HYDDEMANDCAT_H_

#include <string>

#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

class HydDemandCat
{
public:
	
    HydDemandCat(void)=default;
    
    HydDemandCat(HydFloat base_demand, const std::string& time_pattern, const std::string& category)
    {
        m_baseDemand = base_demand;
        m_sTimePatternID = time_pattern;
        m_sCategory = category;
    }
    
    
    HydFloat 
    baseDemand(void) const { return m_baseDemand; }

    const std::string& 
    timePattern(void) const { return m_sTimePatternID;  }

    const std::string& 
    category(void) const { return m_sCategory; }

    void 
    setBaseDemand(HydFloat base_demand) { m_baseDemand = base_demand; }

    void 
    setTimePattern(const std::string& time_pattern_id) { m_sTimePatternID = time_pattern_id; }

    void 
    setCategory(const std::string& category) { m_sCategory = category;  }

private:
    
	HydFloat m_baseDemand;

	std::string m_sTimePatternID;
	std::string m_sCategory;
};

#endif
