/* HydNetwork 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydNetwork.cpp   $
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

#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <cmath>
#include <cstdlib> // for std::rand

HydNetwork::HydNetwork(void)
{
	m_hydraulicOptions = new HydHydraulicOptions();
	m_qualityOptions   = new HydQualityOptions();
	m_reactionOptions  = new HydReactionOptions();
	m_timeOptions      = new HydTimeOptions();
	m_energyOptions    = new HydEnergyOptions();
    

    m_minDiam = 0.0;
    m_maxDiam = 0.0;
    
    m_minTHeadDeficit = 0.0;
    m_maxTHeadDeficit = 0.0;
}
 
HydNetwork::~HydNetwork(void)
{
	delete m_hydraulicOptions;
	delete m_qualityOptions;
	delete m_reactionOptions;
	delete m_timeOptions;
	delete m_energyOptions;
    
    m_hydraulicOptions = nullptr;
    m_qualityOptions   = nullptr;
    m_reactionOptions  = nullptr;
    m_timeOptions      = nullptr;
    m_energyOptions    = nullptr;

	while (!m_vComponents.empty())
	{
		delete m_vComponents.back();
		m_vComponents.pop_back();
	}
}

void
HydNetwork::clear(void)
// clear the dynamic values generated by a simulation run
{
    m_vTimePoints.clear();
    m_cost.clear();
    m_headDeficit.clear();
    m_resilience.clear();
    
    for (int i = 0; i < m_vComponents.size(); ++i)
        m_vComponents[i]->clear();
}

bool 
HydNetwork::addComponent(HydNetComp* comp)
{
	//if (componentPresent(comp))
	//	return false;
 
    assert(!componentPresent(comp));
    
    // give this component a unique index 
    comp->setPos((int) m_vComponents.size());
    m_vComponents.push_back(comp);
    
    // networks consist mainly of pipes and junctions so they get checked first
    switch(comp->hydType())
    {
        case HydType::Pipe:      
            m_vPipes.push_back(static_cast<HydPipe*>(comp));
            m_pipeIndex[comp->ID()] = static_cast<HydPipe*>(comp);
            m_vLinks.push_back(static_cast<HydPipe*>(comp));
            return true;      
            break;
        case HydType::Junction:  
            m_vJunctions.push_back(static_cast<HydJunction*>(comp));
            m_junctionIndex[comp->ID()] = static_cast<HydJunction*>(comp);
            m_vNodes.push_back(static_cast<HydJunction*>(comp));
            return true;
            break;
        case HydType::Pump:      
            m_vPumps.push_back(static_cast<HydPump*>(comp));
            m_pumpIndex[comp->ID()] = static_cast<HydPump*>(comp);
            m_vLinks.push_back(static_cast<HydPump*>(comp));
            return true;     
            break;
        case HydType::Valve:     
            m_vValves.push_back(static_cast<HydValve*>(comp));
            m_valveIndex[comp->ID()] = static_cast<HydValve*>(comp);
            m_vLinks.push_back(static_cast<HydValve*>(comp));
            return true;     
            break;
        case HydType::Tank:      
            m_vTanks.push_back(static_cast<HydTank*>(comp));
            m_tankIndex[comp->ID()] = static_cast<HydTank*>(comp);
            m_vNodes.push_back(static_cast<HydTank*>(comp));
            return true;      
            break;
        case HydType::Reservoir: 
            m_vReservoirs.push_back(static_cast<HydReservoir*>(comp));
            m_reservoirIndex[comp->ID()] = static_cast<HydReservoir*>(comp);
            m_vNodes.push_back(static_cast<HydReservoir*>(comp));
            return true; 
            break;
        default:
            std::cout << "HydNetwork::addComponent - error unknown HydType" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
       
    // error 
    return false;
}

HydNetComp* 
HydNetwork::findComponent(const std::string& comp_id, HydType comp_type) const
{
    // networks consist mainly of pipes and junctions so they get checked first
    switch(comp_type)
    {
        case HydType::Pipe:      return find(comp_id, m_pipeIndex);      break;
        case HydType::Junction:  return find(comp_id, m_junctionIndex);  break;
        case HydType::Pump:      return find(comp_id, m_pumpIndex);      break;
        case HydType::Valve:     return find(comp_id, m_valveIndex);     break;
        case HydType::Tank:      return find(comp_id, m_tankIndex);      break;
        case HydType::Reservoir: return find(comp_id, m_reservoirIndex); break;

        default:
            std::cout << "HydNetwork::findComponent - error unknown HydType" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    
    // error
    return nullptr;
}

HydNode* 
HydNetwork::findNode(const std::string& node_id) const
{
    HydNetComp *comp = nullptr;
    
    if ((comp = findComponent(node_id, HydType::Junction)) == nullptr) 
        if ((comp = findComponent(node_id, HydType::Reservoir)) == nullptr)  
            if ((comp = findComponent(node_id, HydType::Tank)) == nullptr)
                return nullptr;

    return static_cast<HydNode*>(comp);
}

HydLink* 
HydNetwork::findLink(const std::string& link_id) const
{
    HydNetComp *comp = nullptr;
    
    if ((comp = findComponent(link_id, HydType::Pipe)) == nullptr) 
        if ((comp = findComponent(link_id, HydType::Pump)) == nullptr)  
            if ((comp = findComponent(link_id, HydType::Valve)) == nullptr)
                return nullptr;

    return static_cast<HydLink*>(comp);
}

std::vector<HydLink*>
HydNetwork::findLinksToNode(const std::string& node_id) const
{
    std::vector<HydLink*> links;
    for (int i = 0; i < m_vLinks.size(); ++i)
        if (m_vLinks[i]->toNodeID() == node_id || m_vLinks[i]->fromNodeID() == node_id)
            links.push_back( m_vLinks[i] );

    return links;
}

std::vector<HydNode*>
HydNetwork::findNeighboursForNode(const std::string& node_id) const
{
    std::vector<HydNode*> nodes;
    std::vector<HydLink*> links = findLinksToNode(node_id);
    for (int j = 0; j < links.size(); ++j)
    {
        std::string nodeID = (links[j]->toNodeID() == node_id) ? links[j]->fromNodeID() : links[j]->toNodeID(); 
        nodes.push_back(findNode(nodeID)); 
    }
    return nodes;
}
    
//
//
//
  

void 
HydNetwork::setObjectives(const std::vector<HydFloat>& cost, 
                          const std::vector<HydFloat>& headDeficit)
{
    m_cost = cost;
    m_headDeficit = headDeficit;
}

void 
HydNetwork::setObjectives(const std::vector<HydFloat>& cost, 
                          const std::vector<HydFloat>& headDeficit, 
                          const std::vector<HydFloat>& resilience)
{
    m_cost = cost;
    m_headDeficit = headDeficit;
    m_resilience = resilience;
}

void 
HydNetwork::setObjectives(HydFloat cost, HydFloat headDeficit)
{
    m_cost.resize(1);
    m_headDeficit.resize(1);
    
    m_cost[0] = cost;
    m_headDeficit[0] = headDeficit;
}

void 
HydNetwork::setObjectives(HydFloat cost, HydFloat headDeficit, HydFloat resilience)
{
    m_cost.resize(1);
    m_headDeficit.resize(1);
    m_resilience.resize(1);
    
    m_cost[0] = cost;
    m_headDeficit[0] = headDeficit;
    m_resilience[0] = resilience;
}

void 
HydNetwork::addObjectives(HydFloat cost, HydFloat headDeficit)
{
    m_cost.push_back(cost);
    m_headDeficit.push_back(headDeficit);
}

void 
HydNetwork::addObjectives(HydFloat cost, HydFloat headDeficit, HydFloat resilience)
{
    m_cost.push_back(cost);
    m_headDeficit.push_back(headDeficit);
    m_resilience.push_back(resilience);
}




/*
void HydNetwork::initActiveDiameters(void)
{
    for (int i = 0; i < m_vPipes.size(); ++i)
    {
        m_vPipes[i]->diametersActive().clear();
        for (int j = 0; j < m_vAvailableDiameters.size(); ++j)
        {
            m_vPipes[i]->diametersActive().push_back(true);
        }
    }
}
*/

void 
HydNetwork::setRandomDiametersFromAvailable(void)
{
    for (int i = 0; i < m_vPipes.size(); ++i)
    {
        const HydOptions& diams = options(HydType::Pipe, m_vPipes[i]->group());
        int index = std::rand() % diams.size();
        m_vPipes[i]->setDiameter(diams.value(index));
    }
}

//
//
//

HydFloat 
HydNetwork::calculateMeanPipeLength(void)
{
    assert(!m_vPipes.empty());
    
    HydFloat pipeLengthSum = 0.0;

    HydVertex p1, p2;
    std::string ID;

    for (int i = 0; i < m_vPipes.size(); ++i)
    {
        ID = m_vPipes[i]->fromNodeID();
        HydNode* networkNode = findNode(ID);
        if (networkNode != nullptr)
            p1 = networkNode->coords();

        ID = m_vPipes[i]->toNodeID();
        networkNode = findNode(ID);
        if (networkNode != nullptr)
            p2 = networkNode->coords();

        HydVertex d = p1 - p2;
        HydFloat pipeLength = std::sqrt((d.x * d.x) + (d.y * d.y) + (d.z * d.z));
        pipeLengthSum += pipeLength;
    }

    HydFloat meanLength = pipeLengthSum / m_vPipes.size();

    return meanLength;
}

void
HydNetwork::calculateMinMaxPipeDiameter(void)
{
    std::map<HydFloat,int> availDiamsMap;
    
    for (auto i = m_options.begin(); i != m_options.end(); ++i)
    {
        const HydOptions& options = i->second;
        if (options.hydType() == HydType::Pipe)
            for (int j = 0; j < options.values().size(); ++j)
                availDiamsMap[options.value(j)];
    }
    
    std::vector<HydFloat> availDiams = util::toKeyVector(availDiamsMap);
    
    // Find min max available pipe diameters...
    m_minDiam = std::numeric_limits<HydFloat>::max();
    m_maxDiam = -std::numeric_limits<HydFloat>::max();

    for (int i = 0; i < availDiams.size(); ++i)
    {
        if (availDiams[i] > PIPE_OFF)
        {
            m_minDiam = std::min(m_minDiam, availDiams[i]);
            m_maxDiam = std::max(m_maxDiam, availDiams[i]);
        }
    }
}

void 
HydNetwork::normaliseCoordinates(HydFloat x_size, HydFloat y_size, HydFloat z_size)
{
    HydVertex nodeMin, nodeMax;
    
    getNodeMinMax(nodeMin, nodeMax);

    HydVertex CurrentRange = (nodeMax - nodeMin);

	// Uniform scale network to fit x_size && y_size
	// TODO: - rotate 90 if x_size > y_size && YCurrentRange > XCurrentRange and vice versa...
    
	HydFloat XYScaleFactor;

	if (CurrentRange.x >= CurrentRange.y)
		XYScaleFactor = x_size / CurrentRange.x;
	else XYScaleFactor = y_size / CurrentRange.y;

	HydFloat ZScaleFactor = z_size / CurrentRange.z;

    HydVertex NewRange = CurrentRange * XYScaleFactor;
    NewRange.z = CurrentRange.z * ZScaleFactor;

	for (int i = 0; i < m_vNodes.size(); ++i)
	{
        HydVertex normValues = (m_vNodes[i]->coords() - nodeMin) / CurrentRange;
        m_vNodes[i]->coords() = (normValues * NewRange);
	}

	for (int i = 0; i < m_vLinks.size(); ++i)
	{
		for (int j = 0; j < m_vLinks[i]->coords().size(); ++j)
		{
            HydFloat normValue = (m_vLinks[i]->coords(j).x - nodeMin.x) / CurrentRange.x;
            m_vLinks[i]->coords(j).x = normValue * NewRange.x;

			normValue = (m_vLinks[i]->coords(j).y - nodeMin.y) / CurrentRange.y;
            m_vLinks[i]->coords(j).y = normValue * NewRange.y;
		}
	}

	mirrorNetworkX();

	centraliseNetwork();
}

void 
HydNetwork::getNodeMinMax(HydVertex& min, HydVertex& max)
// also set z-coord for junctions, reservoirs, and tanks
// this used to be done in a separate function - by doing it here
// we save a pass through the network structure
{
    min  = std::numeric_limits<HydFloat>::max();
    max  = -std::numeric_limits<HydFloat>::max();

    for (int i = 0; i < m_vJunctions.size(); ++i)
    {
        m_vJunctions[i]->coords().z = m_vJunctions[i]->baseElevation();
        getMinMaxXYZ( min, max, m_vJunctions[i]->coords() );
    }

    for (int i = 0; i < m_vReservoirs.size(); ++i)
    {
        m_vReservoirs[i]->coords().z = m_vReservoirs[i]->totalHead();
        getMinMaxXYZ( min, max, m_vReservoirs[i]->coords() );
    }

    for (int i = 0; i < m_vTanks.size(); ++i)
    {
        m_vTanks[i]->coords().z = m_vTanks[i]->baseElevation();
        getMinMaxXYZ( min, max, m_vTanks[i]->coords() );
    }

    for (int i = 0; i < m_vLinks.size(); ++i)
        for (int j = 0; j < m_vLinks[i]->coords().size(); ++j)
            getMinMaxXY( min, max, m_vLinks[i]->coords(j) );
}

void 
HydNetwork::mirrorNetworkX(void)
{
	// find NodeXmin...
	HydFloat NodeXMin = std::numeric_limits<HydFloat>::max();

	for (int i = 0; i < m_vNodes.size(); ++i)
        NodeXMin = std::min(NodeXMin, m_vNodes[i]->coords().x);  
    
	for (int i = 0; i < m_vLinks.size(); ++i)
		for (int j = 0; j < m_vLinks[i]->coords().size(); ++j)
            NodeXMin = std::min(NodeXMin, m_vLinks[i]->coords(j).x); 

	// flip
	for (int i = 0; i < m_vNodes.size(); ++i)
        m_vNodes[i]->coords().x = NodeXMin - m_vNodes[i]->coords().x;

	for (int i = 0; i < m_vLinks.size(); ++i)
		for (int j = 0; j < m_vLinks[i]->coords().size(); ++j)
            m_vLinks[i]->coords(j).x = NodeXMin - m_vLinks[i]->coords(j).x;
}


void 
HydNetwork::centraliseNetwork(void)
{
	// Find XY min max
    HydVertex min(std::numeric_limits<HydFloat>::max());
    HydVertex max(-std::numeric_limits<HydFloat>::max());

    for (int i = 0; i < m_vNodes.size(); ++i)
        getMinMaxXY( min, max, m_vNodes[i]->coords() );
    
	for (int i = 0; i < m_vLinks.size(); ++i)
        for (int j = 0; j < m_vLinks[i]->coords().size(); ++j)
            getMinMaxXY( min, max, m_vLinks[i]->coords(j) );

    // set z-coord to zero so it is unaffected by shift
    HydVertex Midpoint(min.x + (max.x - min.x)/2.0f, min.y + (max.y - min.y)/2.0f, 0.0);
    
	for (int i = 0; i < m_vNodes.size(); ++i)
        m_vNodes[i]->coords() -= Midpoint;

	for (int i = 0; i < m_vLinks.size(); ++i)
		for (int j = 0; j < m_vLinks[i]->coords().size(); ++j)
            m_vLinks[i]->coords(j) -= Midpoint;
}


