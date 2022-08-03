/* HydObjective 01/03/2021

 $$$$$$$$$$$$$$$$$$$$$$$$
 $   HydObjective.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HYDOBJECTIVE_H__
#include "HydObjective.h"
#endif

#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif


std::pair<HydFloat,HydFloat>
HydObjective::getMinMaxCost( HydNetwork* hydNet )
{
    HydFloat max_cost = 0.0;
    HydFloat min_cost = 0.0; 
    
    const std::vector<HydPipe*>& pipes = hydNet->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
        {
            HydOptions options = hydNet->options(HydType::Pipe, pipe->group());
            min_cost += options.min_cost() * pipe->length();
            max_cost += options.max_cost() * pipe->length();
        }
    }
    return std::pair<HydFloat,HydFloat>(min_cost, max_cost);
}

HydFloat
HydObjective::cost( HydNetwork* hydNet )
// calculated on HydNet
{
    // assumes diameter in HydNet is updated by HydEPANET
    HydFloat total_cost = 0.0;
    
    const std::vector<HydPipe*>& pipes = hydNet->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        
        if (pipe->isDecisionVariable())
        {
            HydOptions options = hydNet->options(HydType::Pipe, pipe->group());
            for (int j = 0; j < options.size(); ++j)
            {
                HydFloat diam = options.value(j); // anycode
                if (std::fabs(pipe->diameter() - diam) < 1E-2) 
                {
                    total_cost += options.cost(j) * pipe->length();
                    break;
                }
            }
        }
    
    }
    
    // not checked/tested
    /*const std::vector<HydValve*>& valves = hydNet->valves();
    for (auto i = valves.begin(); i != valves.end(); ++i)
    {
        HydValve* valve = *i;
        if (valve->isDecisionVariable())
        {
            HydOptions options = hydNet->options(HydType::Valve, valve->group());
            
            for (int j = 0; j < options.size(); ++j)
            {
                //int status = std::static_cast<int>(m_options[idx].option(j)); 
                if (valve->status()[time_frame] == LinkStatusOption::Open)
                    total_cost += options.cost(0);
                else total_cost += options.cost(1);
            }
        }
    }*/
   
    return total_cost;
}

HydFloat
HydObjective::headDeficit( HydNetwork* hydNet, int scenario_index )
// calculated on HydNet
{
    HydFloat total_deficit = 0.0;
    HydFloat total_deficit_neg_pressure = 0.0;
    
    const std::vector<HydJunction*>& junctions = hydNet->junctions();
    for (auto i = junctions.begin(); i != junctions.end(); ++i)
    {
        HydJunction* junction = *i;
        if (junction->isDecisionVariable())
        {
            HydFloat deficit = junction->minHead()[scenario_index] - junction->totalHead()[scenario_index];
            
            HydFloat excess = junction->totalHead()[scenario_index] - junction->minHead()[scenario_index];

            if (deficit < 0.0)
                deficit = 0.0;
            else excess = 0.0;
            
            // Note this sets a variable
            junction->addHeadDeficit(deficit);
            junction->addHeadExcess(excess);
            //
            
            if (deficit > 0.0)
            {
                total_deficit += deficit;
                if (junction->pressure()[scenario_index] < 0.0)
                    total_deficit_neg_pressure += (junction->minHead()[scenario_index] - junction->baseElevation());
                else  total_deficit_neg_pressure += deficit;
            }
        }
    }
    
    return total_deficit;
}


HydFloat
HydObjective::resilience( HydNetwork* hydNet, int scenario_index )
// this works only with HydNetwork - we havent checked the calculation
// check we have values in vector variables
{

    HydFloat junction_dissipation = 0.0;
    HydFloat junction_minimum_power = 0.0;
    
    const std::vector<HydJunction*>& junctions = hydNet->junctions();
    
    assert(scenario_index < junctions[0]->minHead().size());
    
    for (auto i = junctions.begin(); i != junctions.end(); ++i)
    {
        HydJunction* junction = *i;
        
        std::vector<HydFloat> pipe_diams; // diameters of pipes connected to this junction
        
        const std::vector<HydPipe*>& pipes = hydNet->pipes();
        for (auto j = pipes.begin(); j != pipes.end(); ++j)
        {
            HydPipe* pipe = *j;
            if (pipe->fromNodeID() == junction->ID() || pipe->toNodeID() == junction->ID())
                pipe_diams.push_back(pipe->diameter());
        }
        
        HydFloat diameter_uniformity = 0.0;
        if (pipe_diams.size() > 0)
        {
            HydFloat sum = util::sum(pipe_diams);
            HydFloat max = *std::max_element(pipe_diams.begin(), pipe_diams.end());
            diameter_uniformity = sum / (pipe_diams.size() * max);
        }
        else
        {
            std::cout << "ERROR orphan junction" << std::endl;
        }
        
        junction_dissipation += diameter_uniformity * junction->actualDemand()[scenario_index] * (junction->totalHead()[scenario_index] - junction->minHead()[scenario_index]);
        junction_minimum_power += junction->actualDemand()[scenario_index] * junction->minHead()[scenario_index];
    }
    
    HydFloat reservoir_supply_energy = 0.0;
    const std::vector<HydReservoir*>& reservoirs = hydNet->reservoirs();
    for (auto i = reservoirs.begin(); i != reservoirs.end(); ++i)
    {
        HydReservoir* reservoir = *i;
        reservoir_supply_energy += std::fabs(reservoir->netInflow()[scenario_index] * reservoir->elevation());
    }
    
    HydFloat resil = junction_dissipation / (reservoir_supply_energy - junction_minimum_power);
    //assert(!std::isnan(resil));

    return resil;
}
