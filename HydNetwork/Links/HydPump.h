/* HydPump 07/02/2021

 $$$$$$$$$$$$$$$$$
 $   HydPump.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDPUMP_H_
#define _HYDPUMP_H_

#ifndef _HYDLINK_H_
#include "HydLink.h"
#endif

class HydPump : public HydLink
{
public:
    
    HydPump(void) : HydLink("NONE", HydType::Pump),
                    m_power(0.0),
                    m_speed(0.0),
                    m_energyPrice(0.0),
                    m_headloss(0.0),
                    m_sPumpCurveID(),
                    m_sEfficCurveID(),
                    m_sPatternID(),
                    m_sPricePatternID(),
                    m_initialStatus() {}
    
	HydPump(const std::string& hid): HydLink(hid, HydType::Pump),
                                    m_power(0.0),
                                    m_speed(0.0),
                                    m_energyPrice(0.0),
                                    m_headloss(0.0),
                                    m_sPumpCurveID(),
                                    m_sEfficCurveID(),
                                    m_sPatternID(),
                                    m_sPricePatternID(),
                                    m_initialStatus() {}

    HydFloat power(void) const { return m_power; }
    void setPower(HydFloat power) { m_power = power; }
    
    HydFloat speed(void) const { return m_speed; }
    void setSpeed(HydFloat speed) { m_speed = speed; }
    
    HydFloat energyPrice(void) const { return m_energyPrice; }
    void setEnergyPrice(HydFloat energy_price) { m_energyPrice = energy_price; }
    
    HydFloat headloss(void) const { return m_headloss; }
    void setHeadloss(HydFloat headloss) { m_headloss = headloss; }
    
    const std::string& pumpCurveID(void) const { return m_sPumpCurveID; }
    void setPumpCurveID(const std::string& pump_curve_id) { m_sPumpCurveID = pump_curve_id; }
    
    const std::string& efficCurveID(void) const { return m_sEfficCurveID; }
    void setEfficCurveID(const std::string& effic_curve_id) { m_sEfficCurveID = effic_curve_id; }
    
    const std::string& patternID(void) const { return m_sPatternID; }
    void setPatternID(const std::string& pattern_id) { m_sPatternID = pattern_id; }
    
    const std::string& pricePatternID(void) const { return m_sPricePatternID; }
    void setPricePatternID(const std::string& price_pattern_id) { m_sPricePatternID = price_pattern_id; }
    
    LinkStatusOption initialStatus(void) const { return m_initialStatus; }
    void setInitialStatus(LinkStatusOption initial_status) { m_initialStatus = initial_status; }

    void
    clear(void) override
    {
        m_quality.clear();
        m_flow.clear();
        m_status.clear();
    }
    
private:
    
	HydFloat m_power;
	HydFloat m_speed;
	HydFloat m_energyPrice;
	HydFloat m_headloss;

	std::string m_sPumpCurveID;
	std::string m_sEfficCurveID;
	std::string m_sPatternID;
	std::string m_sPricePatternID;

	LinkStatusOption m_initialStatus;
};




#endif
