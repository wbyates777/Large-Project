/* KBHeuristic 17/03/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   KBHeuristic.h   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Pipe-Smoothing and Bottleneck heuristic taken from 
 "Knowledge-based multi-objective genetic algorithms for the design of water distribution networks"
 Johns, M; Keedwell, E; Savic, D
 Journal of Hydroinformatics (2020) 22 (2): 402–422.
 
 If nodes have more than one link (i.e New York Tunnels) the heursitic will pick one at random
 

 TODO: convert to work with HydNetwork - it was written to work with EPANET
 TODO: the change requires constructing the graph with HydNetComp pos() which is unique
 
 TODO: add some logging
*/


#ifndef __KBHEURISTIC_H__
#define __KBHEURISTIC_H__

#include <string>
#include <vector>
#include <map>


#ifndef __HHTYPES_H__
#include "HHTypes.h"
#endif

#ifndef __HYDPROBLEM_H__
#include "HydProblem.h"
#endif

#ifndef __HYDGRAPH_H__
#include "HydGraph.h"
#endif

#ifndef __URAND_H__
#include "URand.h"
#endif


class HydEPANET;
struct Project;

class KBHeuristic
{
public:
    KBHeuristic( void )=delete;
    KBHeuristic( URand& ran ) : m_ran(ran) {} 
    ~KBHeuristic( void )=default;
    
    void
    init( HydProblem* problem );
    

    void
    kb_bottleneck( HHSolution& solution, double param );
    
    void
    kb_pipesmooth( HHSolution& solution, double param );
    
    void
    kb_block_creep( HHSolution& solution, double param );

private:

    KBHeuristic( const KBHeuristic& )=delete;

    KBHeuristic&
    operator=( const KBHeuristic& )=delete;
    
    
    //!<  return a random index [0,...,N-1]
    int rndInt( int N ) { return (int) (m_ran.ran() * N); }
   
    //!< return a random component type (that has a non-zero number of components)
    int rndType( void ) { return m_problem->compType()[rndInt(m_problem->compType().size())]; }
   
    //!< return a random element of component type t
    int rndType( int t ) { return m_problem->compTypeIdx()[t][rndInt(m_problem->compTypeIdx()[t].size())]; }
    
    int
    roulette( int num );
    
    int
    roulette( const std::vector<double>& obj_vals ); 
    
    void 
    shuffle_edges(std::vector<Edge>& edges );
    

    HydProblem* m_problem;      // we do not own this memory
    HydEPANET* m_epanet;        // we do not own this memory
    std::vector<double> m_head_diff;
    std::vector<int> m_nodes; 
    HydGraph  m_graph;
    URand& m_ran;

    mutable Logger m_logger;

};

#endif


