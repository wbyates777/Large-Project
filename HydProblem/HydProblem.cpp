/* HydProblem 18/03/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydProblem.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class loads HydScenarios which defines the optimisation problem
 that is options, constrains, and decision  variable definitions
 
 The class is complicated because KBH works at an EPANET level
 while HDH works at the HydNetwork level - it would be better if both worked with HydNetwork
 
 TODO: Instead of using EPANET indexes which are only unique for links or nodes - but not both
 TODO: rewrite using HydNetComp::pos() which is unique across all components
 TODO: see comments in HOWSProblem.h
*/


#ifndef __HYDPROBLEM_H__
#include "HydProblem.h"
#endif

#ifndef __HYDEPANET_H__
#include "HydEPANET.h"
#endif

#ifndef __HYDSCENARIOS_H__
#include "HydScenarios.h"
#endif


extern "C" {
#include "types.h"
#include "funcs.h"
#include "epanet2_2.h"
}
/*
double 
HydProblem::pressure( int node_idx ) const
{ 
    double p;
    EN_getnodevalue(m_epanet->getProject(), node_idx, EN_PRESSURE, &p);
    return p;
}

double 
HydProblem::head( int node_idx ) const 
{ 
    double h;
    EN_getnodevalue(m_epanet->getProject(), node_idx, EN_HEAD, &h);
    return h;   
} 

double 
HydProblem::diameter( int link_idx ) const
{
    double d;
    EN_getlinkvalue(m_epanet->getProject(), link_idx, EN_DIAMETER, &d);
    return d; 
}

void 
HydProblem::setDiameter( int link_idx, double diameter ) const
{
    EN_setlinkvalue(m_epanet->getProject(), link_idx, EN_DIAMETER, diameter);
}

double 
HydProblem::length( int link_idx ) const
{
    double d;
    EN_getlinkvalue(m_epanet->getProject(), link_idx, EN_LENGTH, &d);
    return d; 
}

LinkStatusOption 
HydProblem::valveStatus( int link_idx ) const
{
    double d;
    EN_getlinkvalue(m_epanet->getProject(), link_idx, EN_STATUS, &d);
    return (d > 0.0) ?  LinkStatusOption::Open : LinkStatusOption::Closed;
}

void
HydProblem::setValveStatus( int link_idx, LinkStatusOption status ) const
{
    double d = (status == LinkStatusOption::Open) ? 1.0 : 0.0;
    EN_setlinkvalue(m_epanet->getProject(), link_idx, EN_STATUS, d);
}
*/


HydProblem::HydProblem( void ): m_epanet(nullptr), 
                                m_hydNet(nullptr), 
                                m_dvariables(),  
                                m_options(), 
                                m_constraints(),
                                m_compTypeIdx(),
                                m_compType(),
                                m_dvar2spec(),
                                m_cons2spec()
{
    m_logger.getLogLevel( "HydProblem" );
}

HydProblem::~HydProblem( void ) 
{
    // we do not own this memory
    m_epanet =  nullptr;  
    m_hydNet =  nullptr; 
}

void 
HydProblem::load( const std::string& name )
{
    HydScenarios spec;
    
    // i.e  name = "WDNetwork/hanoi";
    // use EPANET to load the network and construct our representation
    m_hydNet = m_epanet->load( name );

    // load our problem definition
    std::string prb_name = name.substr(0, name.size()-3);
    prb_name += "prb";
    
    bool ok = spec.load( prb_name );
    if (!ok)
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Unable to read problem file " << prb_name << " (see file report.txt)";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        exit(EXIT_FAILURE);
    }
    
    // decision variables - link components - usually pipes, but could be pumps or valves
    m_compTypeIdx.clear();
    int numTypes = 0;
    for (int i = 0; i < spec.decisions().size(); ++i)
        numTypes = std::max(numTypes, (int) spec.decisions()[i].hydType());
    m_compTypeIdx.resize(numTypes + 1);
    
    m_dvariables.clear();
    for (int i = 0; i < spec.decisions().size(); ++i)
    {
        // convert ID to index
        std::string ID = spec.decisions()[i].ID();
        HydNetComp* comp = m_hydNet->findLink( ID );
        
        if (comp)
        {
            HydDVariable d = spec.decisions()[i];
            d.setIndex(comp->index());

            m_compTypeIdx[static_cast<int>(d.hydType())].push_back( (int) m_dvariables.size() );
            m_dvariables.push_back(d);

            comp->setIsDecisionVariable( true );
            comp->setGroup(d.group());
        }
        else
        {
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Error: unknown link decision ID " << ID << " in " << prb_name;
                m_logger.logMsg(txt);
                m_logger.flush();
            }
        }
    }
    
    // construct an index to the types we have loaded
    m_compType.clear();
    for (int i = 0; i < m_compTypeIdx.size(); ++i)
        if (!m_compTypeIdx[i].empty())
            m_compType.push_back(i);
    
    // options - usually pipe diameters, but could be pump or valve settings 
    m_hydNet->setOptions(spec.options());
    
    m_hydNet->calculateMinMaxPipeDiameter();

    
    // this is needed for KBH
    m_options.clear();
    for (int i = 0; i < spec.decisions().size(); ++i)
    {
        std::string group = spec.decisions()[i].group();
        HydType hydType   = spec.decisions()[i].hydType();
        std::string key   = to_hydstring(hydType) + "_" + group;

        auto fidx = spec.options().find(key);
        if (fidx != spec.options().end())
        {
            m_options.push_back( fidx->second );
        }
        else
        {
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Error: unknown options for decision ID " << spec.decisions()[i].ID() << " in " << prb_name;
                m_logger.logMsg(txt);
                m_logger.flush();
            }
        }
    }
    
    // constraints - usually head pressure applied to junctions
    m_constraints.clear();
    for (int i = 0; i < spec.constraints().size(); ++i)
    {
        std::string ID = spec.constraints()[i].ID();
        HydNetComp* comp = m_hydNet->findNode( ID );
        
        if (comp)
        {
            HydConstraint c = spec.constraints()[i];
            c.setIndex(comp->index());
            
            m_constraints.push_back(c);
            comp->setIsDecisionVariable( true );
            
            if (comp->hydType() == HydType::Junction)
                static_cast<HydJunction*>(comp)->setMinHead( c.values() );
        }
        else
        {
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Error: unknown node constraint ID " << ID << " in " << prb_name;
                m_logger.logMsg(txt);
                m_logger.flush();
            }
        }
    }
    
    // set up a mapping from network decision variables (usually pipe_ids) 
    // to the HHSolution vector indexed [0,...,num_of_dv]
    // -1 indicated that the network id is not a decsion variable
    int numLinks;
    EN_getcount(m_epanet->getProject(), EN_LINKCOUNT, &numLinks);
    m_dvar2spec.resize(numLinks + 1, -1);
    for (int i = 0; i < m_dvariables.size(); ++i)
        m_dvar2spec[m_dvariables[i].index()] = i;  
    
    // set up a mapping from network constraint variables (usually junction_ids) 
    // to the HOWSProblem vector of constraints  indexed [0,...,num_of_cons]
    // -1 indicated that the network id is not constrained
    int numNode;
    EN_getcount(m_epanet->getProject(), EN_NODECOUNT, &numNode);
    m_cons2spec.resize(numNode + 1, -1);
    for (int i = 0; i < m_constraints.size(); ++i)
        m_cons2spec[m_constraints[i].index()] = i;  

    // used by HDH
    calcStaticValues();

    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Loaded problem file " << prb_name;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    if (m_logger.level() >= 2)
    {
        Message txt(1);
        txt << "Network problem loaded with " << m_constraints.size() << " constraints and " << m_dvariables.size() << " decision variables";
        m_logger.logMsg(txt);
        m_logger.flush();
    }
}

void
HydProblem::calcStaticValues( void )
{
    // set all pipes to max diameter and run simulation
    for (int i = 0; i < m_dvariables.size(); ++i)
    {
        if (m_dvariables[i].hydType() == HydType::Pipe)
        {
            HydOptions options = getHydNet()->options(HydType::Pipe, m_dvariables[i].group());
            m_epanet->setDiameter( m_dvariables[i].index(), options.max_option() );
        }
    }

    m_epanet->run();

    //
    //  calculate pipe influence
    //
    std::vector<HydFloat> pipe_flows;
    const std::vector<HydPipe*>& pipes = getHydNet()->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
            pipe_flows.push_back( pipe->maxFlow() );  // we want the max absolute flow 
    }

    HydFloat min_pipe_flow = *std::min_element(pipe_flows.begin(), pipe_flows.end());
    HydFloat max_pipe_flow = *std::max_element(pipe_flows.begin(), pipe_flows.end());

    // normalise flows and set as influence
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
        {
            // use the max absolute flow
            HydFloat influence = (pipe->maxFlow() - min_pipe_flow) / (max_pipe_flow - min_pipe_flow);
            pipe->setInfluence(influence); 
        }
    }
    
    //
    // calculate theoretical min and max head deficit
    //
    int scenario_index = 0; // WARNING: should be caclculated over all scenarios?
    HydFloat head_deficit = 0;
    std::vector<HydFloat> head_excess;

    const std::vector<HydJunction*>& junctions = m_hydNet->junctions();
    for (auto i = junctions.begin(); i != junctions.end(); ++i)
    {
        HydJunction* junction = *i;
        if (junction->isDecisionVariable())
        {
            HydFloat min_head = junction->minHead()[scenario_index];
            HydFloat min_total_head = junction->minTotalHead();
            
            head_excess.push_back(min_head - min_total_head); 
            
            if (min_head - junction->baseElevation() > head_deficit)
                head_deficit = min_head - junction->baseElevation();
            else head_excess.push_back(-min_total_head);
        }
    }

    m_hydNet->minTHeadDeficit( *std::min_element(head_excess.begin(), head_excess.end()) );
    m_hydNet->maxTHeadDeficit( head_deficit );
}

//


