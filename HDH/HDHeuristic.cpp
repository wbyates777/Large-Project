/* HDHeuristic 02/03/2021

 $$$$$$$$$$$$$$$$$$$$$$$
 $   HDHeuristic.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 This class can stand alone for training and saving random forests
 see the commented out main() in the .cpp file
 
 It also integrates with SSHH as a heuristic - in this case set 
 "sessions" = [] in the config file, and HDHeuristic will load
 a pregenerated forest
*/


#ifndef __HDHEURISTIC_H__
#include "HDHeuristic.h"
#endif

#ifndef __HYDEPANET_H__
#include "HydEPANET.h"
#endif

#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif

#ifndef __HYDOBJECTIVE_H__
#include "HydObjective.h"
#endif

#ifndef __HYDSCENARIOS_H__
#include "HydScenarios.h"    
#endif

#ifndef __HYDPROBLEM_H__
#include "HydProblem.h"
#endif

#ifndef __HYDSESSION_H__
#include "HydSession.h"
#endif

#ifndef __AFILEFINDER_H__
#include "AFileFinder.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <memory>

HDHeuristic::HDHeuristic( void ) :  m_hydNet(nullptr),
                                    m_epanet(nullptr),
                                    m_own_epanet(0),
                                    m_processedInputData(),
                                    m_processedTargetData(),
                                    m_rforest()

{
    m_logger.getLogLevel( "HDHeuristic" );
}

HDHeuristic::~HDHeuristic( void ) 
{
    if (m_own_epanet)
    {
        delete m_hydNet;
        delete m_epanet;
    }
    
    m_epanet = nullptr;
    m_hydNet = nullptr;
}

void
HDHeuristic::init( HydEPANET* epanet )
{
    if (epanet)
    {
        if (m_epanet && m_own_epanet == 1)
            delete m_epanet;
        
        if (m_hydNet && m_own_epanet == 1)
            delete m_hydNet;

        m_own_epanet = 0;
        m_epanet = epanet;
        m_hydNet = m_epanet->getHydNet();
    }
    else
    {
        if (m_epanet && m_own_epanet == 1)
            delete m_epanet;
        
        if (m_hydNet && m_own_epanet == 1)
            delete m_hydNet;
        
        m_own_epanet = 1;
        m_epanet = new HydEPANET;
        m_hydNet = nullptr;
    }
    
}

void
HDHeuristic::set( const HDHConfig& cfg )
{
    m_config = cfg;
    if( m_config.sessions().empty() )
    {
        std::string fullpath = theFF->findFile(m_config.name() + ".forest");
        m_rforest.load(fullpath); 
    }
}

void 
HDHeuristic::load( const std::string& name ) 
{ 
    m_config.loadJSON(name);
    m_rforest.init(m_config);
    if( m_config.sessions().empty() )
    {
        std::string fullpath = theFF->findFile(m_config.name() + ".forest");
        m_rforest.load(fullpath); 
    }
}

void 
HDHeuristic::save( const std::string& name ) 
{ 
    if (name.empty())
        m_rforest.save(m_config.name()); 
    else m_rforest.save(name); 
}



bool
HDHeuristic::predict( const std::string& pipe_id )
{
    HydPipe* pipe = m_hydNet->findPipe( pipe_id );
    
    if (!pipe)
        return false;
    
    std::vector<HydFloat> nvs = getNormValues(pipe_id); 

    // upstream and down stream deficits
    std::pair<HydFloat, HydFloat> updown_def = getNormUpDownNodeDeficit(pipe_id);
    HydFloat up_deficit   = updown_def.first;
    HydFloat down_deficit = updown_def.second; 
    
    //  { "Diameter", "Velocity", "Up_Deficit", "Down_Deficit", "Influence", "Flow", "Length" };
    std::vector<double> input_data = { 
        nvs[Diameter], nvs[Velocity], up_deficit, down_deficit, nvs[Influence], nvs[Flow], nvs[Length] 
    };
    
    std::vector<double> out =  m_rforest.predict(input_data); 
    
    if (!out.empty())
        return (out[0] == 1.0) ? true : false;
    
    return false;
}



void
HDHeuristic::learn(void) 
{
    m_processedInputData.clear();
    m_processedTargetData.clear();
    
    HydProblem problem;
    problem.init(m_epanet);
    
    const std::vector<std::string>& sessions = m_config.sessions();
    for (int i = 0; i < sessions.size(); ++i)
    {
        HydSession session;
        std::string sname = theFF->findFile(sessions[i]);
        session.loadJSON(sname);

        std::string pname = theFF->findFile(session.networkName());
        problem.load(pname);
        
        if (m_hydNet)
            delete m_hydNet;
        m_hydNet = problem.getHydNet();
        
        process_interactions(session);
        
        if (m_logger.level() >= 2)
        {
            Message txt(1);
            txt << m_processedInputData.size() << " training patterns generated";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    
    m_rforest.init(m_config);
    m_rforest.learn(Matrix<double>(m_processedInputData), Matrix<double>(m_processedTargetData));
}



void        
HDHeuristic::process_interactions(const HydSession& session)
{
    const std::vector<HydInteraction>& NetworkStates = session.interactions();
    std::string changed_pipe_id;
    
    /* self.InputFeatures = [MLFeatures.diameter, MLFeatures.velocity, MLFeatures.upstream_deficit,
     MLFeatures.downstream_deficit, MLFeatures.pipe_influence, MLFeatures.pipe_flow,
     MLFeatures.pipe_length]
     self.Targets = [MLTargets.up_down]*/
    
    HydInteraction previous_state;
    HydInteraction current_state;
    
    for (int i = 0; i < NetworkStates.size(); ++i)
    {
        if (i > 0)
        {
            previous_state = NetworkStates[i-1];
            current_state = NetworkStates[i];
        }
        
        // hydraulic_network.evaluateObjectives(previous_state)
        for (int j = 0; j < previous_state.size(); ++j)
        {
            HydPipe* pipe = m_hydNet->findPipe(previous_state[j].ID());
            if (pipe)
                pipe->setDiameter(previous_state[j].diameter()); 
        }
        run();
        //
        
        // for the pipe (diameters) in the previous network state
        for (int x = 0; x < previous_state.size(); ++x)
        {
            if (previous_state[x].ID() == current_state[x].ID())
            {
                if (previous_state[x].diameter() != current_state[x].diameter())
                {
                    changed_pipe_id = previous_state[x].ID();
                    
                    // training inputs
                    
                    // normed values for diameter, velocity, influemce, flow and length
                    std::vector<HydFloat> nvs = getNormValues(changed_pipe_id); 
       
                    // upstream and down stream deficits
                    std::pair<HydFloat, HydFloat> updown_def = getNormUpDownNodeDeficit(changed_pipe_id);
                    HydFloat up_deficit   = updown_def.first;
                    HydFloat down_deficit = updown_def.second; 
                    
                    // there was code to select features via config - add
                    
                    std::vector<double> input_data = { 
                        nvs[Diameter], nvs[Velocity], up_deficit, down_deficit, nvs[Influence], nvs[Flow], nvs[Length] 
                    };
        
                    m_processedInputData.push_back(input_data);
                    
                    // training output
                    
                    // target output for regression
                    // HydFloat diameter_target = getNormPipeDiameter(changed_pipe_id, current_state[x].diameter());
                    
                    // target output for classification
                    HydFloat up_down_target = -1;
                    
                    HydFloat  changed_pipe_diameter = 0.0;
                    HydPipe* pipe = m_hydNet->findPipe(changed_pipe_id);
                    if (pipe)
                        changed_pipe_diameter = pipe->diameter();
                    
                    if (current_state[x].diameter() > changed_pipe_diameter)
                        up_down_target = 1;
                    else if (current_state[x].diameter() <= changed_pipe_diameter)
                        up_down_target = 0;
    
                    // there was code to select features via config - add
                    std::vector<double> target_data = { up_down_target };
                    
                    m_processedTargetData.push_back(target_data);  
                }   
                
                // pipe_cost = new_network.getPipeCost(changed_pipe_id)
                
                //in_deficit = False
                //if new_network.totalHeadDeficit > 0:
                //    in_deficit = True
                
                //network_cost = new_network.networkCost
                
                //input_vector = [previous_interaction_pipes[x]['diameter'], up_deficit, down_deficit,
                //                velocity]
                //X.append(input_vector)
                //y.append(current_interaction_pipes[x]['diameter'])
            }
            else 
            {
                if (m_logger.level() >= 1)
                {
                    Message txt(1);
                    txt << "Error: previous network state does not match current network state in process interactions";
                    m_logger.logMsg(txt);
                    m_logger.flush();
                }
            }
        }
        
        //for i in range(len(self.ProcessedInputData)):
        //    print(self.ProcessedInputData[i], self.ProcessedTargetData[i])
        
        //hydraulic_network.closeEpanet()
    }
}


std::vector<HydFloat>
HDHeuristic::run( void )
{
    const std::vector<HydPipe*>& pipes = m_hydNet->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
            m_epanet->setDiameter( pipe->index(), pipe->diameter() );
    }

    m_epanet->run();
    
    int scenario_index = 0;
    HydFloat c = HydObjective::cost(m_hydNet) / 1E6;                        // cost in millions
    HydFloat h = HydObjective::headDeficit(m_hydNet, scenario_index);      // calls junction->setHeadDeficit(deficit);
    HydFloat r = HydObjective::resilience(m_hydNet, scenario_index);
    
    return std::vector<HydFloat>({c, h, r});
}



void
HDHeuristic::setPipesToMax(void)
{
    const std::vector<HydPipe*>& pipes = m_hydNet->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
        {
            HydOptions options = m_hydNet->options(HydType::Pipe, pipe->group());
            pipe->setDiameter(options.max_option());
        }
    }
}

void
HDHeuristic::setPipesToMin(void)
{
    const std::vector<HydPipe*>& pipes = m_hydNet->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
        {
            HydOptions options = m_hydNet->options(HydType::Pipe, pipe->group());
            pipe->setDiameter(options.min_option());
        }
    }
}

HydFloat
HDHeuristic::getNormPipeDiameter(const std::string& pipe_id, HydFloat diameter)
{
    return normalise(diameter, m_config.params()[3][0], m_config.params()[3][1]);
}

std::vector<HydFloat>
HDHeuristic::getNormValues(const std::string& pipe_id) 
{
    std::vector<HydFloat>  normalised_values(5, 0.0);
    HydPipe* pipe = m_hydNet->findPipe(pipe_id);
    if (pipe)
    {
        normalised_values[Velocity]  = normalise(pipe->maxVelocity(), m_config.params()[0][0], m_config.params()[0][1]);
        normalised_values[Flow]      = normalise(pipe->maxFlow(),     m_config.params()[1][0], m_config.params()[1][1]);
        normalised_values[Length]    = normalise(pipe->length(),      m_config.params()[2][0], m_config.params()[2][1]);
        normalised_values[Influence] = pipe->influence(); 
        normalised_values[Diameter]  = normalise(pipe->diameter(),    m_config.params()[3][0], m_config.params()[3][1]);
    }

    return normalised_values;
}

std::pair<HydFloat,HydFloat>
HDHeuristic::getNormUpDownNodeDeficit(const std::string& pipe_id)
{
    HydFloat up_head_deficit = 0.0;
    HydFloat down_head_deficit = 0.0;
    
    // theoretical max and min deficit values 
    HydFloat minTHeadDeficit = m_hydNet->minMaxTHeadDeficit().first;
    HydFloat maxTHeadDeficit = m_hydNet->minMaxTHeadDeficit().second;
    
    std::string from_node;
    std::string to_node;
    
    HydPipe* pipe = m_hydNet->findPipe(pipe_id);
    if (pipe)
    {
        HydFloat total_flow = util::sum(pipe->flow());
        
        from_node = pipe->fromNodeID();
        to_node   = pipe->toNodeID();
        
        if (total_flow < 0.0)
            std::swap(from_node,to_node);
    }

    // WARNING: should HDH pick a scenario index at random?
    int scenario_index = 0; 
    const std::vector<HydJunction*>& junctions = m_hydNet->junctions();
    for (auto i = junctions.begin(); i != junctions.end(); ++i)
    {
        HydJunction* junction = *i;
        
        HydFloat min_head       = junction->minHead()[scenario_index];
        HydFloat min_total_head = junction->minTotalHead();
        
        // up head
        if (junction->ID() == from_node)
        {
            if ((min_head - min_total_head) > 0.0)
            {
                if (junction->minPressure() < 0.0)
                    up_head_deficit  = (min_head - junction->baseElevation()) / maxTHeadDeficit;
                else up_head_deficit = (min_head - min_total_head) / maxTHeadDeficit;
            }
            else
            {
                up_head_deficit = ((min_head - min_total_head) - minTHeadDeficit) / (0.0 - minTHeadDeficit);
                up_head_deficit -= 1.0;
            }
        }
        
        // down head
        if (junction->ID() == to_node)
        {
            if ((min_head - min_total_head ) > 0.0)
            {
                if (junction->minPressure() < 0.0)
                    down_head_deficit  = (min_head - junction->baseElevation()) / maxTHeadDeficit;
                else down_head_deficit = (min_head - min_total_head) / maxTHeadDeficit;
            }
            else
            {
                down_head_deficit = ((min_head - min_total_head) - minTHeadDeficit) / (0.0 - minTHeadDeficit);
                down_head_deficit -= 1.0;
            }
        }
    }

    const std::vector<HydReservoir*>& reservoirs = m_hydNet->reservoirs();
    for (auto i = reservoirs.begin(); i != reservoirs.end(); ++i)
    {
        HydReservoir* reservoir = *i;
        
        if (reservoir->ID() == from_node)
            up_head_deficit = -1.0;
        
        if (reservoir->ID() == to_node)
            down_head_deficit = -1.0;
    }

    return std::pair<HydFloat, HydFloat>(up_head_deficit, down_head_deficit);
}

/*

 //
 // use this class in a stand alone manner
 //
int 
main(int argc, const char * argv[]) 
{
    Logger::setLogLevel("HDHeuristic", 3);
    
    HDHeuristic hdh;
    hdh.init();                 // initalise EPANET
    hdh.load("HDH1_Learn.txt"); // config has session name(s)
    hdh.learn();
    hdh.save();
}

*/


