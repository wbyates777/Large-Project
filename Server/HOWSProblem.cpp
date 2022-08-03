/* HOWSProblem 13/05/2021

 $$$$$$$$$$$$$$$$$$$$$$$
 $   HOWSProblem.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A water distribution network problem class - load an EPANET network, and 
 a HydScenarios which specifies the network constraints, options and decision variables
 which together define the problem to be worked on/optimised
 
 // see https://www.epa.gov/water-research/epanet
 // see https://github.com/OpenWaterAnalytics/EPANET/wiki/
 // see https://nepis.epa.gov/Adobe/PDF/P1007WWU.pdf
 
*/


#ifndef __HOWSPROBLEM_H__
#include "HOWSProblem.h"
#endif

#ifndef __HYDSCENARIOS_H__
#include "HydScenarios.h"    
#endif

#ifndef __AFILEFINDER_H__
#include "AFileFinder.h"
#endif

extern "C" 
{
#include "types.h"
#include "epanet2_2.h"
}


#include <cmath>
#include <sstream>
#include <algorithm> 
#include <iostream>
#include <functional>


///

const std::vector<std::string> HOWSProblem::m_llhTypes = 
{ "MUTATE", "SWAP", "MUTATE", "RUIN", "SHUFFLE", "CROSSOVER", "BLOCK", "BOTTLENECK", "PIPESMOOTH", "HDH", "CROSSOVER", "CROSSOVER",  }; 






/////////////////////

HOWSProblem::HOWSProblem( const std::string& inst, unsigned int rseed ): HHProblem(), m_param(0.0), m_ran(rseed), m_kbh(m_ran) 
{
    m_logger.getLogLevel( "HOWSProblem" );
    load(inst);
}

HOWSProblem::HOWSProblem( void ): HHProblem(), m_param(0.0), m_ran(87), m_kbh(m_ran) 
{
    m_logger.getLogLevel( "HOWSProblem" );
}


HOWSProblem::~HOWSProblem( void )  
{ 
  //  if (m_problem.getHydNet())
  //      delete m_problem.getHydNet();
  //  m_problem.getHydNet() = nullptr;
}

/// The random number generator's seed
int
HOWSProblem::seed( void ) const 
{ 
    return m_ran.seed(); 
}

/// Set the random number generator's seed
void
HOWSProblem::seed( unsigned long s ) 
{ 
    m_ran.seed((unsigned int) s); 
}

bool
HOWSProblem::compareSolutions( int mem_idx1, int mem_idx2 ) const
{
    for (int i = 0; i < m_problem.dvariables().size(); ++i)
        if (m_solution[mem_idx1][i] != m_solution[mem_idx2][i])
            return false;
    return true;
}

std::string 
HOWSProblem::solutionToString( int mem_idx ) const 
{
    // compute  the cost
    HHObjective out = evalauteObj(m_solution[mem_idx]);
    
    double cost = out[0];
    double head_deficit = out[1];
    double resilience = out[2];
    
    std::ostringstream str;
    str.precision(4);
    str.setf( std::ios::fixed, std::ios::floatfield ); 


    str << "\n";    
    for (int i = 0; i < m_problem.dvariables().size(); ++i)
        str << (int) m_solution[mem_idx][i] << ' ';
    
    str << "\ncost " << cost << "M\nhead deficit " << head_deficit << "\nresilience " << resilience << '\n';
    
    return str.str();
}



std::string  
HOWSProblem::toString( void ) const  
{ 
    return "HOWSProblem";
}

double
HOWSProblem::distance( int mem_idx1, int mem_idx2 ) const
{
    double d = 0.0;
    for (int i = 0; i < m_problem.dvariables().size(); ++i)
    {
        double tmp = (double) (m_solution[mem_idx1][i] - m_solution[mem_idx2][i]);
        d += tmp * tmp;
    }
    return sqrt(d);
}


void 
HOWSProblem::initialiseSolution( int mem_idx )  
{ 
    m_solution[mem_idx].resize(m_problem.dvariables().size(), 0); 
    random(m_solution[mem_idx], 0.0); 
}

void 
HOWSProblem::initialiseSolution( HHSolution& solution )  
{ 
    solution.resize(m_problem.dvariables().size(), 0); 
    random(solution, 0.0); 
}

void 
HOWSProblem::setMemorySize( int mem_num ) 
{ 
    m_solution.clear(); m_solution.resize(mem_num, HHSolution(m_problem.dvariables().size(), 0));     
}

void 
HOWSProblem::copySolution( int mem_idx1, int mem_idx2 )  
{ 
    m_solution[mem_idx2] = m_solution[mem_idx1]; 
    
}


// cost, head_deficit, resiliance
HHObjective 
HOWSProblem::evalauteObj( int mem_idx ) const  
{ 
    return evalauteObj(m_solution[mem_idx]);  // WDN paper
}

std::vector<std::string>
HOWSProblem::getHeuristicTypes( void ) const  
{ 
    return m_llhTypes; 
}

std::string
HOWSProblem::getHeuristicType( int h_id ) const 
{
    if (h_id >= 0 && h_id < m_llhTypes.size())
        return m_llhTypes[h_id];
  
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Unknown low level heuristic type " << h_id;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    exit(EXIT_FAILURE);
}

void 
HOWSProblem::applyHeuristic( int h_id, int mem_idx1, int mem_idx2 ) 
{
    m_solution[mem_idx2] = m_solution[mem_idx1];
    
    switch (h_id)
    {
        case 0 :  mutate1 (m_solution[mem_idx2], m_param); break;
        case 1 :  swap    (m_solution[mem_idx2], m_param); break;
        case 2 :  mutate2 (m_solution[mem_idx2], m_param); break;
        case 3 :  ruin1   (m_solution[mem_idx2], m_param); break;
        case 4 :  shuffle1(m_solution[mem_idx2], m_param); break;
        
        // 5 is reserved for 2 point cross over

        case 6 :  m_kbh.kb_block_creep(m_solution[mem_idx2], m_param); break; 
        case 7 :  m_kbh.kb_bottleneck(m_solution[mem_idx2], m_param); break; 
        case 8 :  m_kbh.kb_pipesmooth(m_solution[mem_idx2], m_param); break; 
        case 9 :  hd_heuristic(m_solution[mem_idx2], m_param); break; 

        default:
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Unknown low level heuristic id " << h_id;
                m_logger.logMsg(txt);
                m_logger.flush();
            }
            exit(EXIT_FAILURE);
            break;
    }
}

void 
HOWSProblem::applyHeuristic( int h_id, int mem_idx1, int mem_idx2, int mem_idx3 ) 
{
    switch (h_id)
    {
        case 5 :  crossover2(m_solution[mem_idx1], m_solution[mem_idx2], m_solution[mem_idx3], m_param); break;
        case 10 : crossover1(m_solution[mem_idx1], m_solution[mem_idx2], m_solution[mem_idx3], m_param); break;
        case 11 : crossover3(m_solution[mem_idx1], m_solution[mem_idx2], m_solution[mem_idx3], m_param); break;
        default:
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Unknown low level heuristic id " << h_id;
                m_logger.logMsg(txt);
                m_logger.flush();
            }
            exit(EXIT_FAILURE);
            break;
    }   
}


void 
HOWSProblem::load( const std::string& instance_id )
{
    m_problem.init(&m_epanet);
    m_problem.load(instance_id);

  
    // used for random selection of elements of type t - see def of rndType() in HOWSProblem.h
    m_compTypeIdx = m_problem.compTypeIdx();                 /// decision variable index for each component type
    m_compType    = m_problem.compType();                    /// index for each type of component present

    // get pipe influence
    const std::vector<HydPipe*>& pipes = m_problem.getHydNet()->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
    {
        HydPipe* pipe = *i;
        if (pipe->isDecisionVariable())
            m_pipeInfluence.push_back(pipe->influence()); 
    }
    
    // setup KBH
    m_kbh.init(&m_problem);

    // setup HDH
    m_hdh.init( &m_epanet ); 
    std::string fullpath = theFF->findFile("HDH1.txt");
    m_hdh.load(fullpath);

    if (m_logger.level() >= 1) 
    {
        Message txt(1);
        txt << "Loaded problem " << instance_id.substr(0, instance_id.size()-4);
        m_logger.logMsg(txt);
        m_logger.flush();
    }
}

HHObjective
HOWSProblem::evalauteObj( const HHSolution& solution ) const
{
    assert(solution.size() == m_problem.dvariables().size());

    if (!m_oldSolution.empty() && m_oldSolution == solution)
        return m_oldValue;
    
    // update EPANET with the new solution decisions
    for (int i = 0; i < m_problem.dvariables().size(); ++i)
    {
        if (m_problem.dvariables()[i].hydType() == HydType::Pipe)
        {
            HydOptions options = m_problem.getHydNet()->options(HydType::Pipe, m_problem.dvariables()[i].group());
            HydFloat diam = options.value(solution[i]); 
            
            m_epanet.setDiameter( m_problem.dvariables()[i].index(), diam );
        }
        else if (m_problem.dvariables()[i].hydType() == HydType::Valve)
        {
            HydOptions options = m_problem.getHydNet()->options(HydType::Valve, m_problem.dvariables()[i].group());
            int status = options.value(solution[i]); 
            
            if (status > 0)
                m_epanet.setValveStatus( m_problem.dvariables()[i].index(), LinkStatusOption::Open );
            else m_epanet.setValveStatus( m_problem.dvariables()[i].index(), LinkStatusOption::Closed );
        }
        // add other link types here
    }

    // run the simulation
    // this will update diameters, flows, etc in m_problem.getHydNet()
    m_epanet.run();

    // cache this run's values; pick worst numbers over all scenarios
    const std::vector<HydFloat>& cost = m_problem.getHydNet()->cost();
    const std::vector<HydFloat>& headDeficit = m_problem.getHydNet()->headDeficit();
    const std::vector<HydFloat>& resilience = m_problem.getHydNet()->resilience();
    
    // we  want to minimise cost and pressure deficit
    double c = *std::max_element(cost.begin(), cost.end());      
    double h = *std::max_element(headDeficit.begin(), headDeficit.end());  
    // resilence has max value of 1.0, can be negative and is to be maximised
    double r = *std::min_element(resilience.begin(), resilience.end());

    // SSHH  minimises objective values so we use negatove resilence here
    m_oldValue = {c, h, -r };
    m_oldSolution = solution;
    
    return m_oldValue;
}



///
/// Low Level Heuristics
///

void
HOWSProblem::random( HHSolution& solution, double param )
// this creates an entirely random chromosome
{
    for (int i = 0; i < m_problem.dvariables().size(); ++i)
    {
        int j = rndInt(m_problem.options()[i].size());
        solution[i] = j;
    }    
}

void
HOWSProblem::mutate1( HHSolution& solution, double param )
// change only one components option randomly (compare ruin)
{
    int idx1 = rndInt(m_problem.dvariables().size());
    int idx2 = rndInt(m_problem.options()[idx1].size());

    solution[idx1] = idx2;
}

void
HOWSProblem::swap( HHSolution& solution, double param )
// swap two components at random
{
    int typeIdx = rndType();
    int idx1 = rndType(typeIdx);
    int idx2 = rndType(typeIdx);
    std::swap(solution[idx1], solution[idx2]);
}

void
HOWSProblem::mutate2( HHSolution& solution, double param )
// increase or decrease a randomly selected pipe diameter by one pipe size.
{
    // int num = (int) param + 1;
    // for (int i = 0; i < num; ++i)
    {
        int idx1 = rndInt(m_problem.dvariables().size());
        int diam = solution[idx1];
        
        double r = m_ran.ran();
        if (r > 0.5)
            diam++;
        else diam--;
        
        if (diam < 0)
            diam = 0;
        
        if (diam >= m_problem.options()[idx1].size())
            diam = (int) m_problem.options()[idx1].size() - 1;
        
        solution[idx1] = diam;
    }
}

void
HOWSProblem::shuffle1( HHSolution& solution, double param )
// swap several components  
{ 
    int num = (int) param + 1; // shuffle num elements 1..5 
    int typeIdx = rndType();
    
    // num = solution.size() * (num/20.0); // shuffle a percentage of components 
    for (int i = 0; i < num; ++i) 
    {
        // int typeIdx = rndType();
        int idx1 = rndType(typeIdx);
        int idx2 = rndType(typeIdx);
        std::swap(solution[idx1], solution[idx2]);
    }
}

void
HOWSProblem::ruin1( HHSolution& solution, double param )
// ‘ruin’ several pipes and rebuild randomly. 
{
    int num = (int) param + 1; // ruin num elements 1..5 
    //  num = solution.size() * (num/15.0); // ruin a percentage of components 
    
    for (int i = 0; i < num; ++i)
    {
        int k = rndInt(m_problem.dvariables().size());
        int opt = rndInt(m_problem.options()[k].size());
        solution[k] = opt;
    } 
}



void
HOWSProblem::crossover2( const HHSolution& s1, const HHSolution& s2, HHSolution& s3, double param )
// two point
{
    int cut1 = rndInt(m_problem.dvariables().size());
    int cut2 = rndInt(m_problem.dvariables().size());
    
    if (cut1 > cut2)
        std::swap(cut1, cut2);
    
    s3 = s1;
    for (int i = cut1; i < cut2; ++i)
        s3[i] = s2[i];
}

void
HOWSProblem::crossover1( const HHSolution& s1, const HHSolution& s2, HHSolution& s3, double param )
// single point
{
    int cut = rndInt(m_problem.dvariables().size());

    s3 = s1;
    
    if (m_ran.ran() > 0.5)
    {
        for (int i = cut; i < m_problem.dvariables().size(); ++i)
            s3[i] = s2[i];
    }
    else
    {
        for (int i = 0; i < cut; ++i)
            s3[i] = s2[i];
    }
}

void
HOWSProblem::crossover3( const HHSolution& s1, const HHSolution& s2, HHSolution& s3, double param )
// uniform
{
    s3 = s1;
    for (int i = 0; i < m_problem.dvariables().size(); ++i)
    {
        if (m_ran.ran() > 0.5)
            s3[i] = s2[i];
    }
}

int
HOWSProblem::roulette( const std::vector<double>& obj_vals )
// roulette wheel selection
{          
    if (obj_vals.empty())
        return -1;
    
    if (obj_vals.size() == 1)
        return 0;
    
    double total = 0.0;
    for (int i = 0; i < obj_vals.size(); ++i) 
        total += obj_vals[i];

    double prob  = m_ran.ran() * total;   
    double sum   = 0.0;

    for (int i = 0; i < obj_vals.size(); ++i) 
    {
        sum += obj_vals[i];
        if (sum > prob)
            return i;
    }
    
    return -1;
}

void
HOWSProblem::hd_heuristic( HHSolution& solution, double param )
{
    int idx = roulette( m_pipeInfluence );

    std::string ID = m_problem.dvariables()[idx].ID();
    bool up = m_hdh.predict(ID);
    
    int diam = solution[idx];
        
    if (up)
        diam++;
    else diam--;
    
    if (diam < 0)
        diam = 0;
    
    if (diam >= m_problem.options()[idx].size())
        diam = (int) m_problem.options()[idx].size() - 1;
    
    solution[idx] = diam;
}

//


