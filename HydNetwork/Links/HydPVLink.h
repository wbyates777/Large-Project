/* HydPVLink 07/02/2021

 $$$$$$$$$$$$$$$$$$$
 $   HydPVLink.h   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDPVLINK_H_
#define _HYDPVLINK_H_

#ifndef _HYDLINK_H_
#include "HydLink.h"
#endif

class HydPVLink : public HydLink
{
public:
	HydPVLink(void): HydLink(), 
                     m_diameter(0.0), 
                     m_lossCoeff(0.0),
                     m_diameter3DScale(0.0), 
                     m_velocity() {}
    
    HydPVLink(const std::string& hid, HydType compType) : HydLink(hid, compType), 
                                                         m_diameter(0.0), 
                                                         m_lossCoeff(0.0),
                                                         m_diameter3DScale(0.0), 
                                                         m_velocity() {}
    
    virtual ~HydPVLink(void) override=default;

    HydFloat diameter(void) const { return m_diameter; }
    void setDiameter(HydFloat diameter) { m_diameter = diameter; }
	
    HydFloat lossCoeff(void) const { return m_lossCoeff; }
    void setLossCoeff(HydFloat loss_coeff) { m_lossCoeff = loss_coeff; }

    const std::vector<HydFloat>& velocity(void) const { return m_velocity; }
    void addVelocity( HydFloat velocity ) { m_velocity.push_back(velocity); }
    void setVelocity( HydFloat velocity ) { m_velocity = std::vector<HydFloat>(1,velocity); }
    void setVelocity(const std::vector<HydFloat>& velocity) { m_velocity = velocity; }
    HydFloat maxVelocity(void) const { return *std::max_element(m_velocity.begin(), m_velocity.end()); }
    HydFloat minVelocity(void) const { return *std::min_element(m_velocity.begin(), m_velocity.end()); }
    
    HydFloat diameter3DScale(void) const { return m_diameter3DScale; }
    void setDiameter3DScale(HydFloat diameter_3d_scale) { m_diameter3DScale = diameter_3d_scale; }

    //std::vector<bool>& diametersActive(void) { return m_vDiametersActive; }

protected:
    
	HydFloat m_diameter;
	HydFloat m_lossCoeff;
    HydFloat m_diameter3DScale;
	std::vector<HydFloat> m_velocity;

    //std::vector<bool>     m_vDiametersActive;
};


#endif
