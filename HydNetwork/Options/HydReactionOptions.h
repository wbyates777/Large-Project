/* HydReactionOptions 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$$$$$
 $   HydReactionOptions.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDREACTIONOPTIONS_H_
#define _HYDREACTIONOPTIONS_H_

#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

enum class WallReactionOrder
{
	Zero,
	First
};


class HydReactionOptions
{
public:
    HydReactionOptions(void)=default;

	WallReactionOrder 
    wallReactionOrder(void) const { return m_wallReactionOrder; }

    void 
    setWallReactionOrder(WallReactionOrder order) { m_wallReactionOrder = order; }
    
	int 
    bulkReactionOrder(void) const { return m_bulkReactionOrder;  }

    void 
    setBulkReactionOrder(int order) { m_bulkReactionOrder = order; }
    
    HydFloat 
    globalBulkCoeff(void) const { return m_globalBulkCoeff; }
        
    void 
    setGlobalBulkCoeff(HydFloat bulk_coeff) { m_globalBulkCoeff = bulk_coeff; }
    

	HydFloat 
    globalWallCoeff(void) const { return m_globalWallCoeff; }
    
    void 
    setGlobalWallCoeff(HydFloat wall_coeff)  { m_globalWallCoeff = wall_coeff; }
    
    HydFloat 
    limitingConcentration(void) const {  return m_limitingConcentration;  }

    void 
    setLimitingConcentration(HydFloat concentration) { m_limitingConcentration = concentration; }
    
    HydFloat 
    wallCoeffCorrelation(void) const { return m_wallCoeffCorrelation;  }

    void 
    setWallCoeffCorrelation(HydFloat correlation) {  m_wallCoeffCorrelation = correlation; }

private:
    
	WallReactionOrder m_wallReactionOrder;
    
	int m_bulkReactionOrder;

	HydFloat m_globalBulkCoeff;
	HydFloat m_globalWallCoeff;
	HydFloat m_limitingConcentration;
	HydFloat m_wallCoeffCorrelation;
};

#endif
