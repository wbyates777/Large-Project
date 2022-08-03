/* HydStorageNode 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$
 $   HydStorageNode.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDSTORAGENODE_H_
#define _HYDSTORAGENODE_H_

#ifndef _HYDNODE_H_
#include "HydNode.h"
#endif

class HydStorageNode : public HydNode
{
public:
    
    HydStorageNode(void) : HydNode(), m_elevation(0.0), m_netInflow() {} 
    HydStorageNode(const std::string& hid, HydType compType): HydNode(hid, compType), m_elevation(0.0), m_netInflow() {}
    virtual ~HydStorageNode(void) override=default;
    
    HydFloat elevation(void) const { return m_elevation; }
    void setElevation(HydFloat elevation) { m_elevation = elevation; }
    
    const std::vector<HydFloat>& netInflow(void) const { return m_netInflow; }
    void setNetInflow(const std::vector<HydFloat>& net_inflow) { m_netInflow = net_inflow; }
    void addNetInflow( HydFloat net_inflow ) { m_netInflow.push_back(net_inflow); }
    void setNetInflow( HydFloat net_inflow ) { m_netInflow = std::vector<HydFloat>(1,net_inflow); }

protected:
    
	HydFloat m_elevation;
    std::vector<HydFloat> m_netInflow;
};



#endif
