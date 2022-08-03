/* HydSourceQuality 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$$$
 $   HydSourceQuality.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDSOURCEQUALITY_H_
#define _HYDSOURCEQUALITY_H_

#include <iostream>
#include <string>

#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

enum class HydSourceType
{
	Concentration,
	MassBooster,
	SetpointBooster,
	FlowPacedBooster
};


class HydSourceQuality
{
public:
    HydSourceQuality(void)=default;
    HydSourceQuality(HydFloat quality, HydSourceType sourceType, const std::string& timePattern)
    {
        m_sourceQuality = quality;
        m_sourceType = sourceType;
        m_sTimePatternID = timePattern;
    }

    HydFloat 
    sourceQuality(void) const { return m_sourceQuality; }

    void 
    setSourceQuality(HydFloat source_quality)  { m_sourceQuality = source_quality; }
    
    const std::string& 
    timePatternID(void) const { return m_sTimePatternID; }
    
    void 
    setTimePatternID(const std::string& time_pattern_id) { m_sTimePatternID = time_pattern_id;  }
    
    HydSourceType 
    sourceType(void) const { return m_sourceType; }

    void 
    setSourceType(HydSourceType source_type)  { m_sourceType = source_type; }

private:
    
	HydFloat      m_sourceQuality;
    HydSourceType m_sourceType;
    std::string   m_sTimePatternID;
};

#endif
