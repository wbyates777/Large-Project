/* HydPipe 07/02/2021

 $$$$$$$$$$$$$$$$$
 $   HydPipe.h   $
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

#ifndef _HYDPIPE_H_
#define _HYDPIPE_H_

#ifndef _HYDPVLINK_H_
#include "HydPVLink.h"
#endif

class HydPipe : public HydPVLink
{
public:
    
    HydPipe(void) : HydPVLink("NONE", HydType::Pipe),
                    m_length(0.0),
                    m_roughness(0.0),
                    m_bulkCoeff(0.0),
                    m_wallCoeff(0.0),
                    m_influence(0.0),
                    m_unitHeadloss(),
                    m_frictionFactor(),
                    m_reactionRate(),
                    m_initialStatus(PipeStatus::Open), m_mutationCount(0) {}  

    HydPipe(const std::string& hid): HydPVLink(hid, HydType::Pipe),
                                    m_length(0.0),
                                    m_roughness(0.0),
                                    m_bulkCoeff(0.0),
                                    m_wallCoeff(0.0),
                                    m_influence(0.0),
                                    m_unitHeadloss(),
                                    m_frictionFactor(),
                                    m_reactionRate(),
                                    m_initialStatus(PipeStatus::Open), m_mutationCount(0) {} 

    HydFloat length(void) const { return m_length; }
    void setLength(HydFloat length) { m_length = length; }
    
    void setInitialStatus(PipeStatus initial_status) { m_initialStatus = initial_status; }
    PipeStatus initialStatus(void) const { return m_initialStatus; }
    
    HydFloat roughness(void) const { return m_roughness; }
    void setRoughness(HydFloat roughness) { m_roughness = roughness; }
    
    HydFloat bulkCoeff(void) const { return m_bulkCoeff; }
    void setBulkCoeff(HydFloat bulk_coeff) { m_bulkCoeff = bulk_coeff; }
    
    HydFloat wallCoeff(void) const { return m_wallCoeff; }
    void setWallCoeff(HydFloat wall_coeff) { m_wallCoeff = wall_coeff; }
    
    HydFloat influence(void) const { return m_influence; }
    void setInfluence(HydFloat influence ) { m_influence = influence; }
    
    const std::vector<HydFloat>& unitHeadloss(void) const { return m_unitHeadloss; }
    void addUnitHeadLoss( HydFloat headLoss ) { m_unitHeadloss.push_back(headLoss); }
    void setUnitHeadLoss( HydFloat headLoss ) { m_unitHeadloss = std::vector<HydFloat>(1,headLoss); }
    void setUnitHeadLoss(const std::vector<HydFloat>& unit_headloss) { m_unitHeadloss = unit_headloss; }
	
    const std::vector<HydFloat>& frictionFactor(void) const { return m_frictionFactor; }
    void setFrictionFactor(const std::vector<HydFloat>& friction_factors) { m_frictionFactor = friction_factors; }
    void setFrictionFactor(HydFloat friction_factor) { m_frictionFactor = std::vector<HydFloat>(1,friction_factor); }
    void addFrictionFactor(HydFloat friction_factor) { m_frictionFactor.push_back(friction_factor); }
    
    const std::vector<HydFloat>& reactionRate(void) const { return m_reactionRate; }
    void setReactionRate(const std::vector<HydFloat>& reaction_rates) { m_reactionRate = reaction_rates; }
    void setReactionRate(HydFloat reaction_rates) { m_reactionRate = std::vector<HydFloat>(1,reaction_rates); }
    void addReactionRate(HydFloat reaction_rate) { m_reactionRate.push_back(reaction_rate); }
    
    void setMutationCount(int mutation_count) { m_mutationCount = mutation_count; }
    int mutationCount() const { return m_mutationCount; }
    
    void
    clear(void) override
    {
        m_quality.clear();
        
        m_flow.clear();
        m_status.clear();
        m_velocity.clear();
        
        m_unitHeadloss.clear();
        m_frictionFactor.clear();
        m_reactionRate.clear();
    }
    
private:
    
	HydFloat m_length;
	HydFloat m_roughness;
	HydFloat m_bulkCoeff;
	HydFloat m_wallCoeff;
    HydFloat m_influence;
	std::vector<HydFloat> m_unitHeadloss;
	std::vector<HydFloat> m_frictionFactor;
	std::vector<HydFloat> m_reactionRate;
	PipeStatus m_initialStatus;	
    
    int m_mutationCount;
};


#endif
