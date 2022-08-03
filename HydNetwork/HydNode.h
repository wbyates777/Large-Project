/* HydNode 07/02/2021

 $$$$$$$$$$$$$$$$$
 $   HydNode.h   $
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

#ifndef _HYDNODE_H_
#define _HYDNODE_H_

#ifndef _HydNetComp_H_
#include "HydNetComp.h"
#endif

#ifndef _HYDSOURCEQUALITY_H_
#include "HydSourceQuality.h"
#endif

#ifndef __HYDVERTEX_H__
#include "HydVertex.h"
#endif

class HydNode : public HydNetComp
{
public:

    HydNode(void) : HydNetComp(),
                    m_scale3D(0.0),
                    m_initialQuality(0.0),
                    m_vertex(),
                    m_sourceQuality(),
                    m_pressure() {}
    
    HydNode(const std::string& hid, HydType compType) : HydNetComp(hid, compType),
                                                        m_scale3D(0.0),
                                                        m_initialQuality(0.0),
                                                        m_vertex(),
                                                        m_sourceQuality(),
                                                        m_pressure() {}
    
    virtual ~HydNode(void) override=default;
    
    const std::vector<HydFloat>& pressure(void) const { return m_pressure; }
    void addPressure(const HydFloat pressure) { m_pressure.push_back(pressure); } 
    void setPressure(const HydFloat pressure) { m_pressure = std::vector<HydFloat>(1,pressure); } 
    void setPressure(const std::vector<HydFloat>& pressure) { m_pressure = pressure; }
    HydFloat maxPressure(void) const { return *std::max_element(m_pressure.begin(), m_pressure.end()); }
    HydFloat minPressure(void) const { return *std::min_element(m_pressure.begin(), m_pressure.end()); }
    
    HydSourceQuality&  sourceQuality(void) { return m_sourceQuality; }
    HydFloat initialQuality(void) const { return m_initialQuality; }
    void setInitialQuality(HydFloat initial_quality) { m_initialQuality = initial_quality; }
    
    const HydVertex& coords(void) const { return m_vertex; }
    HydVertex& coords(void) { return m_vertex; }
    void setCoords(const HydVertex& vertex)  { m_vertex = vertex; }
    
    HydFloat scale3D(void) const { return m_scale3D; }
    void setScale3D(HydFloat scale_3d) { m_scale3D = scale_3d; }
    
protected:
 
	HydFloat m_scale3D;
	HydFloat m_initialQuality;
    HydVertex m_vertex;
    
    HydSourceQuality m_sourceQuality;
    std::vector<HydFloat> m_pressure;
};


#endif
