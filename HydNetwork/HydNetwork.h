/* HydNetwork 07/02/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydNetwork.h   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDNETWORK_H_
#define _HYDNETWORK_H_

#ifndef _HYDJUNCTION_H_
#include "HydJunction.h"
#endif

#ifndef _HYDPIPE_H_
#include "HydPipe.h"
#endif

#ifndef _HYDRESERVOIR_H_
#include "HydReservoir.h"
#endif

#ifndef _HYDTANK_H_
#include "HydTank.h"
#endif

#ifndef _HYDPUMP_H_
#include "HydPump.h"
#endif

#ifndef _HYDVALVE_H_
#include "HydValve.h"
#endif

#ifndef _HYDHYDRAULICOPTIONS_H_
#include "HydHydraulicOptions.h"
#endif

#ifndef _HYDQUALITYOPTIONS_H_
#include "HydQualityOptions.h"
#endif

#ifndef _HYDREACTIONOPTIONS_H_
#include "HydReactionOptions.h"
#endif

#ifndef _HYDTIMEOPTIONS_H_
#include "HydTimeOptions.h"
#endif

#ifndef _HYDENERGYOPTIONS_H_
#include "HydEnergyOptions.h"
#endif

#ifndef __HYDDVARIABLE_H__
#include "HydDVariable.h"
#endif

#ifndef __HYDOPTIONS_H__
#include "HydOptions.h"
#endif

#ifndef __HYDCONSTRAINT_H__
#include "HydConstraint.h"
#endif

#ifndef __HYDLOOKAHEAD_H__
#include "HydLookAhead.h"
#endif

#include <map>

class HydNetwork
{
public:
    
	HydNetwork(void);
	~HydNetwork(void);

    std::string name(void) const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    //
    // network component access
    //
    
	bool 
    addComponent(HydNetComp* component);
    
    HydNetComp*
    getComponent(int pos) { return m_vComponents[pos]; }

    HydNetComp* 
    findComponent(const std::string& comp_id, HydType type) const;

	HydNode* 
    findNode(const std::string& node_id) const;
    
    HydLink* 
    findLink(const std::string& link_id) const;
    
    HydPipe* 
    findPipe(const std::string& pipe_id) const { return find(pipe_id, m_pipeIndex); }
    
    HydJunction* 
    findJunction(const std::string& junction_id) const { return find(junction_id, m_junctionIndex); }
    
    HydPump* 
    findPump(const std::string& pump_id) const { return find(pump_id, m_pumpIndex); }
    
    HydValve* 
    findValve(const std::string& valve_id) const { return find(valve_id, m_valveIndex); }
    
    HydReservoir* 
    findReservoir(const std::string& res_id) const { return find(res_id, m_reservoirIndex); }
    
    HydTank* 
    findTank(const std::string& tank_id) const { return find(tank_id, m_tankIndex); }
    
    std::vector<HydLink*>
    findLinksToNode(const std::string& node_id) const;

    std::vector<HydNode*>
    findNeighboursForNode(const std::string& node_id) const;
    
	const std::vector<HydPipe*>& 
    pipes(void) const { return m_vPipes; }
    
    const std::vector<HydPump*>& 
    pumps(void) const { return m_vPumps; }
    
    const std::vector<HydValve*>& 
    valves(void) const { return m_vValves; }
	
    const std::vector<HydJunction*>& 
    junctions(void) const { return m_vJunctions; }
	
    const std::vector<HydReservoir*>& 
    reservoirs(void) const { return m_vReservoirs; }
	
    const std::vector<HydTank*>& 
    tanks(void) const { return m_vTanks; }

    // hydraulic options
    HydHydraulicOptions*
    hydraulicOptions(void) const { return m_hydraulicOptions; }
 
    // this will clear the vectors of hydraulic values generated during a simulation run
    // other scalar network variables (i.e pipe diameters, lengths etc) are not affected
    void
    clear(void);

    //
    // decision variable options - i.e pipe diameters for pipe decision variables
    //
    
    const HydOptions& 
    options( HydType compType, const std::string& group ) 
    { 
        return m_options[to_hydstring(compType) + "_" + group]; 
    }
    
    // key is component type + "_" + component group
    const std::map<std::string, HydOptions>&  
    options( void ) const { return m_options; }
    
    // this will overwrite any existing data
    void
    addOptions( const HydOptions& dopt ) 
    { 
        m_options[to_hydstring(dopt.hydType()) + "_" + dopt.group()] =  dopt; 
    }
    
    void
    setOptions( const std::map<std::string, HydOptions>& options ) 
    { 
        m_options = options; 
    }
    
    //
    // the min and max diameters across the whole network - used for drawing/scaling network
    //
    
    const std::pair<HydFloat,HydFloat>
    minMaxDiameter(void) const 
    { 
        return std::pair<HydFloat,HydFloat>(m_minDiam,m_maxDiam); 
    }

    void
    minDiameter( HydFloat minD ) { m_minDiam = minD; }
    
    void
    maxDiameter( HydFloat maxD ) { m_maxDiam = maxD; }
    
    //
    // theoretical min and max head deficits
    //
    
    const std::pair<HydFloat,HydFloat>
    minMaxTHeadDeficit(void) const 
    { 
        return std::pair<HydFloat,HydFloat>(m_minTHeadDeficit,m_maxTHeadDeficit); 
    }

    void
    minTHeadDeficit( HydFloat minTHead ) { m_minTHeadDeficit = minTHead; }
    
    void
    maxTHeadDeficit( HydFloat maxTHead ) { m_maxTHeadDeficit = maxTHead; }
    
    //
    // objective function values
    //
    
	void setObjectives(HydFloat cost, HydFloat headDeficit);
	void setObjectives(HydFloat cost, HydFloat headDeficit, HydFloat resilience);

    void setObjectives(const std::vector<HydFloat>& cost, 
                       const std::vector<HydFloat>& headDeficit);
    
    void setObjectives(const std::vector<HydFloat>& cost, 
                       const std::vector<HydFloat>& headDeficit, 
                       const std::vector<HydFloat>& resilience);
    
    void addObjectives(HydFloat cost, HydFloat headDeficit);
    void addObjectives(HydFloat cost, HydFloat headDeficit, HydFloat resilience);
    
    HydFloat cost(int scenIdx) const { return m_cost[scenIdx]; }
    HydFloat headDeficit(int scenIdx) const { return m_headDeficit[scenIdx]; }
    HydFloat resilience(int scenIdx) const { return m_resilience[scenIdx]; }
    
    const std::vector<HydFloat>& cost(void) const { return m_cost; }
    const std::vector<HydFloat>& headDeficit(void) const { return m_headDeficit; }
    const std::vector<HydFloat>& resilience(void) const { return m_resilience; }

    //
    // Look Ahead 
    //
    
    const HydLookAhead& 
    lookAhead(void) const { return m_lookAhead; }
    
    void
    setLookAhead(const HydLookAhead& la) { m_lookAhead = la; }

    //
    //
    //
    
    
	// void 
    // initActiveDiameters(void);

    void
    setTimePoints(const std::vector<long>& tps) { m_vTimePoints = tps; }
  
    const std::vector<long>&
    timePoints(void) const { return m_vTimePoints; }
    
    void
    addTimePoints(long tp) { m_vTimePoints.push_back(tp); }
    
	void 
    setRandomDiametersFromAvailable(void);

	HydFloat 
    calculateMeanPipeLength(void);
    
    void
    calculateMinMaxPipeDiameter(void);

    void 
    normaliseCoordinates(HydFloat x_size, HydFloat y_size, HydFloat z_size);
    
private:
    
    HydNetwork( const HydNetwork& )=delete;

    HydNetwork&
    operator=( const HydNetwork& )=delete;
    
   
    bool 
    componentPresent(HydNetComp *comp)
    {
        return (findComponent(comp->ID(), comp->hydType()) != nullptr) ? true : false;
    }
    
    template<typename T> 
    T* find(const std::string& hid, const std::map<std::string, T*>& index) const
    {
        auto comp = index.find(hid);
        return (comp != index.end()) ? comp->second : nullptr;
    }
    
    void
    getMinMaxXY( HydVertex& min, HydVertex& max, const HydVertex& coord )
    {
        max.x = std::max(max.x, coord.x); 
        min.x = std::min(min.x, coord.x);
        max.y = std::max(max.y, coord.y);
        min.y = std::min(min.y, coord.y);
    }
    
    void
    getMinMaxXYZ( HydVertex& min, HydVertex& max, const HydVertex& coord )
    {
        max.x = std::max(max.x, coord.x); 
        min.x = std::min(min.x, coord.x);
        max.y = std::max(max.y, coord.y);
        min.y = std::min(min.y, coord.y);
        max.z = std::max(max.z, coord.z);
        min.z = std::min(min.z, coord.z);
    }
    
    void 
    getNodeMinMax( HydVertex& min, HydVertex& max );
    
    void 
    mirrorNetworkX(void);
    
    void 
    centraliseNetwork(void);
    
    HydHydraulicOptions *m_hydraulicOptions;
    HydQualityOptions   *m_qualityOptions;
    HydReactionOptions  *m_reactionOptions;
    HydTimeOptions      *m_timeOptions;
    HydEnergyOptions    *m_energyOptions;

    HydFloat m_minDiam;
    HydFloat m_maxDiam;
    
    // max and min theoretical head deficits
    HydFloat m_minTHeadDeficit;
    HydFloat m_maxTHeadDeficit;

    std::string m_name;
    
    std::vector<HydFloat> m_cost;
    std::vector<HydFloat> m_headDeficit;
    std::vector<HydFloat> m_resilience;

	std::vector<HydNetComp*>   m_vComponents;
    std::vector<HydLink*>      m_vLinks;
    std::vector<HydNode*>      m_vNodes;
    
	std::vector<HydJunction*>  m_vJunctions;
	std::vector<HydPipe*>      m_vPipes;
	std::vector<HydReservoir*> m_vReservoirs;
	std::vector<HydTank*>      m_vTanks;
	std::vector<HydPump*>      m_vPumps;
	std::vector<HydValve*>     m_vValves;

    std::map<std::string, HydJunction*>  m_junctionIndex;
    std::map<std::string, HydPipe*>      m_pipeIndex;
    std::map<std::string, HydReservoir*> m_reservoirIndex;
    std::map<std::string, HydTank*>      m_tankIndex;
    std::map<std::string, HydPump*>      m_pumpIndex;    
    std::map<std::string, HydValve*>     m_valveIndex;

    std::map<std::string, HydOptions> m_options;
    std::vector<long> m_vTimePoints;
    HydLookAhead m_lookAhead;
};

#endif // !_HYDNETWORK_H_

