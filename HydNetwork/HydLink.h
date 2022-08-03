/* HydLink 07/02/2021

 $$$$$$$$$$$$$$$$$
 $   HydLink.h   $
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

#ifndef _HYDLINK_H_
#define _HYDLINK_H_


#ifndef _HydNetComp_H_
#include "HydNetComp.h"
#endif

#ifndef __HYDVERTEX_H__
#include "HydVertex.h"
#endif

#include <string>
#include <vector>
#include <cmath>



class HydLink : public HydNetComp
{
public:
	HydLink(void) : HydNetComp(),
                    m_fromNode(-1),
                    m_toNode(-1),
                    m_sFromNode(),
                    m_sToNode(),
                    m_flow(),
                    m_status(),
                    m_vHydVertexs() {}
    
    HydLink(const std::string& hid, HydType compType) : HydNetComp(hid, compType),
                                                        m_fromNode(-1),
                                                        m_toNode(-1),
                                                        m_sFromNode(),
                                                        m_sToNode(),
                                                        m_flow(),
                                                        m_status(),
                                                        m_vHydVertexs() {}
    
    virtual ~HydLink(void) override=default;

    int fromNode(void) const { return m_fromNode; }
    void setFromNode(int from_node) { m_fromNode = from_node; }
    const std::string& fromNodeID(void) const { return m_sFromNode; }
    void setFromNodeID(const std::string& from_node) { m_sFromNode = from_node; }
    
    int toNode(void) const { return m_toNode; }
    void setToNode(int to_node) { m_toNode = to_node; }
    const std::string& toNodeID(void) const { return m_sToNode; }
    void setToNodeID(const std::string& to_node)  { m_sToNode = to_node; }

    const std::vector<HydFloat>& flow(void) const { return m_flow; }
    void setFlow(HydFloat flow) { m_flow = std::vector<HydFloat>(1,flow); }
    void addFlow(HydFloat flow) { m_flow.push_back(flow); }
    void setFlow(const std::vector<HydFloat> &flow) { m_flow = flow; }
    
    // we want the max/min absolute flow
    HydFloat maxFlow(void) const 
    { 
        auto comp = [](HydFloat a, HydFloat b)->bool { return std::fabs(a) < std::fabs(b); };
        return std::fabs(*std::max_element(m_flow.begin(), m_flow.end(), comp)); 
    }
    
    HydFloat minFlow(void) const 
    { 
        auto comp = [](HydFloat a, HydFloat b)->bool { return std::fabs(a) < std::fabs(b); };
        return std::fabs(*std::min_element(m_flow.begin(), m_flow.end(), comp));
    }
    
    const std::vector<LinkStatusOption>& status(void) const { return m_status; }
    void addStatus(LinkStatusOption status) { m_status.push_back(status); }
    void setStatus(LinkStatusOption status) { m_status = std::vector<LinkStatusOption>(1,status); }
    void setStatus(const std::vector<LinkStatusOption> & status) { m_status = status; }

    const std::vector<HydVertex>& coords(void) const { return m_vHydVertexs; }
    const HydVertex& coords(int i) const { return m_vHydVertexs[i]; }
    HydVertex& coords(int i) { return m_vHydVertexs[i]; }
    void addCoords(HydVertex& c) { m_vHydVertexs.push_back(c); }
    void setCoords(const std::vector<HydVertex>& cl) { m_vHydVertexs = cl; }

    
protected:
    
	int m_fromNode;
	int m_toNode;

	std::string m_sFromNode;
	std::string m_sToNode;

	std::vector<HydFloat> m_flow;
	std::vector<LinkStatusOption> m_status;

	std::vector<HydVertex> m_vHydVertexs;
};

#endif
