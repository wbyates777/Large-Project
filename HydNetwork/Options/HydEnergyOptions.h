/* HydEnergyOptions 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$$$
 $   HydEnergyOptions.h   $
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

#ifndef _HYDENERGYOPTIONS_H_
#define _HYDENERGYOPTIONS_H_

#include <string>

#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

class HydEnergyOptions
{
public:
    
    HydEnergyOptions(void)=default;

    int 
    pumpEfficiency(void) const { return m_pumpEfficiency; }

    void 
    setPumpEfficiency(int efficiency)  {  m_pumpEfficiency = efficiency; }

    HydFloat 
    energyPricePerKWh(void) const { return m_energyPricePerKWh; }

    void 
    setEnergyPricePerKWh(HydFloat energy_price) { m_energyPricePerKWh = energy_price; }

    HydFloat 
    demandCharge(void) const { return m_demandCharge; }

    void 
    setDemandCharge(HydFloat demand_charge) { m_demandCharge = demand_charge;  }

    const std::string& 
    pricePattern(void) const { return m_sPricePattern; }

    void 
    setPricePattern(const std::string& price_pattern) {  m_sPricePattern = price_pattern; }


private:
    
	int m_pumpEfficiency;

	HydFloat m_energyPricePerKWh;
	HydFloat m_demandCharge;

	std::string m_sPricePattern;
};

#endif // !_HYDENERGYOPTIONS_H_

