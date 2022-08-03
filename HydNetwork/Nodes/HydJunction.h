/* HydJunction 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   HydJunction.h   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDJUNCTION_H_
#define _HYDJUNCTION_H_

#ifndef _HYDNODE_H_
#include "HydNode.h"
#endif


#ifndef _HYDDEMANDCAT_H_
#include "HydDemandCat.h"
#endif

#include <vector>

class HydJunction : public HydNode
{
public:
    HydJunction(void) : HydNode("NONE", HydType::Junction),
                        m_baseElevation(0.0),
                        m_baseDemand(0.0),
                        m_emitterCoeff(0.0),
                        m_sDemandPatternID(),
                        m_vDemandCategories(),
                        m_headDeficit(),
                        m_headExcess(),
                        m_totalHead(),
                        m_minHead(),
                        m_actualDemand() {}
    
	HydJunction(const std::string& hid): HydNode(hid, HydType::Junction),
                                        m_baseElevation(0.0),
                                        m_baseDemand(0.0),
                                        m_emitterCoeff(0.0),
                                        m_sDemandPatternID(),
                                        m_vDemandCategories(),
                                        m_headDeficit(),
                                        m_headExcess(),
                                        m_totalHead(),
                                        m_minHead(),
                                        m_actualDemand() {}


    HydFloat baseElevation(void) const { return m_baseElevation; }
    void setBaseElevation(HydFloat base_elevation) { m_baseElevation = base_elevation; }
    
    HydFloat baseDemand(void) const { return m_baseDemand; }
    void setBaseDemand(HydFloat base_demand) { m_baseDemand = base_demand; }
    
    HydFloat emitterCoeff(void) const { return m_emitterCoeff; }
    void setEmitterCoeff(HydFloat emitter_coeff) { m_emitterCoeff = emitter_coeff; }

    const std::vector<HydFloat>&  headDeficit(void) const { return m_headDeficit; }
    void addHeadDeficit( HydFloat head_deficit ) { m_headDeficit.push_back(head_deficit); }
    void setHeadDeficit( HydFloat head_deficit ) { m_headDeficit = std::vector<HydFloat>(1, head_deficit); }
    void setHeadDeficit(const std::vector<HydFloat>& head_deficit) { m_headDeficit = head_deficit; }
    
    const std::vector<HydFloat>&  headExcess(void) const { return m_headExcess; }
    void addHeadExcess( HydFloat head_excess ) { m_headExcess.push_back(head_excess); }
    void setHeadExcess( HydFloat head_excess ) { m_headExcess = std::vector<HydFloat>(1, head_excess); }
    void setHeadExcess(const std::vector<HydFloat>& head_excess) { m_headExcess = head_excess; }
    
    const std::vector<HydFloat>& minHead(void) const { return m_minHead; }
    void addMinHead(HydFloat minimum_head) { m_minHead.push_back(minimum_head); }
    void setMinHead(HydFloat minimum_head) { m_minHead = std::vector<HydFloat>(1, minimum_head); }
    void setMinHead(const std::vector<HydFloat>& minimum_head) { m_minHead = minimum_head; }
    
    const std::string& demandPattern(void) const { return m_sDemandPatternID; }
    void setDemandPattern(const std::string& demand_pattern_id) { m_sDemandPatternID = demand_pattern_id; }

    const std::vector<HydFloat>&  totalHead(void) const { return m_totalHead; }
    void addTotalHead( HydFloat total_head ) { m_totalHead.push_back(total_head); }
    void setTotalHead( HydFloat total_head ) { m_totalHead = std::vector<HydFloat>(1, total_head); }
    void setTotalHead(const std::vector<HydFloat>& total_head) { m_totalHead = total_head; }
    HydFloat maxTotalHead(void) const { return *std::max_element(m_totalHead.begin(), m_totalHead.end()); }
    HydFloat minTotalHead(void) const { return *std::min_element(m_totalHead.begin(), m_totalHead.end()); }

	// add/remove demand category
	void addDemandCategory(HydFloat base_demand, const std::string& time_pattern, const std::string& category)
    {
        HydDemandCat newDemandCat(base_demand, time_pattern, category);
        m_vDemandCategories.push_back(newDemandCat);
    }
    
    const std::vector<HydDemandCat>& demandCategories(void) const { return m_vDemandCategories; }
    void removeDemandCategory(int catorgory_index) { m_vDemandCategories.erase(m_vDemandCategories.begin() + catorgory_index); }
    
    const std::vector<HydFloat>& actualDemand(void) const { return m_actualDemand; }
    void addActualDemand( HydFloat actual_demand ) { m_actualDemand.push_back(actual_demand); }
    void setActualDemand( HydFloat actual_demand ) { m_actualDemand = std::vector<HydFloat>(1, actual_demand); }
    void setActualDemand(const std::vector<HydFloat>& actual_demand)  { m_actualDemand = actual_demand; }
    
    void
    clear(void) override
    {
        m_pressure.clear();
        m_quality.clear();
        m_headDeficit.clear();
        m_headExcess.clear();
        m_totalHead.clear();
        m_actualDemand.clear();
    }
    
private:

	HydFloat m_baseElevation;
	HydFloat m_baseDemand;
	HydFloat m_emitterCoeff;

    std::string m_sDemandPatternID;
    std::vector<HydDemandCat> m_vDemandCategories;
    std::vector<HydFloat> m_headDeficit;
    std::vector<HydFloat> m_headExcess;
    std::vector<HydFloat> m_totalHead;
    std::vector<HydFloat> m_minHead;
    std::vector<HydFloat> m_actualDemand;
};


#endif
