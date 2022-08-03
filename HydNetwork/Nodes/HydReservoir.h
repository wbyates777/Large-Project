/* HydReservoir 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydReservoir.h   $
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

#ifndef _HYDRESERVOIR_H_
#define _HYDRESERVOIR_H_

#ifndef _HYDSTORAGENODE_H_
#include "HydStorageNode.h"
#endif

class HydReservoir : public HydStorageNode
{
public:
    HydReservoir(void): HydStorageNode("NONE", HydType::Reservoir), m_totalHead(0.0) {}
	HydReservoir(const std::string& hid): HydStorageNode(hid, HydType::Reservoir), m_totalHead(0.0) {}

    HydFloat totalHead(void) const { return m_totalHead; }
    void setTotalHead(HydFloat total_head) { m_totalHead = total_head; }

    void
    clear(void) override
    {
        m_pressure.clear();
        m_netInflow.clear();
        m_quality.clear();
    }
    
private:
    
	HydFloat m_totalHead;

};


#endif
