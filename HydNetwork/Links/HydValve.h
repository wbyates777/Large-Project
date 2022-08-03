/* HydValve 07/02/2021

 $$$$$$$$$$$$$$$$$$
 $   HydValve.h   $
 $$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDVALVE_H_
#define _HYDVALVE_H_

#ifndef _HYDPVLINK_H_
#include "HydPVLink.h"
#endif

class HydValve : public HydPVLink
{
public:
    
    HydValve(void): HydPVLink("NONE", HydType::Valve), m_setting(0.0), m_type(), m_fixedStatus() {}
	HydValve(const std::string& hid) : HydPVLink(hid, HydType::Valve), m_setting(0.0), m_type(), m_fixedStatus() {}

    HydFloat headloss(void) const { return m_headloss; }
    void setHeadloss(HydFloat headloss)  { m_headloss = headloss; }
    
    
    HydFloat setting(void) const { return m_setting; }
    void setSetting(HydFloat setting) { m_setting = setting; }
    
    ValveType type(void) const { return m_type; }
    void setType(ValveType type) { m_type = type; }
    
    ValveFixedStatus fixedStatus(void) const { return m_fixedStatus; }
    void setFixedStatus(ValveFixedStatus fixed_status) { m_fixedStatus = fixed_status; }

    void
    clear(void) override
    {
        m_quality.clear();
        m_flow.clear();
        m_status.clear();
        m_velocity.clear();
    }
    
private:
    
	HydFloat m_headloss;
	HydFloat m_setting;
	ValveType m_type;
	ValveFixedStatus m_fixedStatus;

};



#endif
