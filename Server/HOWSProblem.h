/* HOWSProblem 13/05/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   HOWSProblem.h   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 A water distribution network problem class - load an EPANET network, and 
 a HydScenarios which specifies the network constraints, options and decision variables
 which together define the problem to be worked on/optimised
 
 TODO: This class also contains the low-level heuristics used by SSHH
 TODO: I'm not happy with this 'design' - the LLHs ought to be separate but the code 'turned out' like this
 TODO: because KBH and HDH need access to EPANET and HydNetwork respectivly - KBH should be rewritten so that 
 TODO: it only depends on HydNetComp::pos() - this will improve encapsulation and make it easy to split into distinct classes

*/


#ifndef __HOWSPROBLEM_H__
#define __HOWSPROBLEM_H__

#ifndef __HHPROBLEM_H__
#include "HHProblem.h"
#endif

#ifndef __HYDPROBLEM_H__
#include "HydProblem.h"
#endif

#ifndef __HYDEPANET_H__
#include "HydEPANET.h"
#endif

#ifndef __HDHEURISTIC_H__
#include "HDHeuristic.h"
#endif

#ifndef __KBHEURISTIC_H__
#include "KBHeuristic.h"
#endif

#ifndef __URAND_H__
#include "URand.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <vector>
#include <cmath>

class HOWSProblem : public HHProblem 
{
public:
 
    //
    // SSHH interface (inherited from HHProblem)
    //
    
    HOWSProblem( void );
    HOWSProblem( const std::string& inst, unsigned int rseed = 87 );
    ~HOWSProblem( void ) override;

    /// The random number generator's seed
    int
    seed( void ) const;
    
    /// Set the random number generator's seed
    void
    seed( unsigned long s ) override;
    
    int
    getNumberOfInstances( void ) const override { return 1; }
    
    int 
    getNumberOfHeuristics( void ) const override { return 12; }; 
    
    std::vector<std::string>
    getHeuristicTypes( void ) const  override;
    
    std::string
    getHeuristicType( int i ) const override;
    
    std::vector<int> 
    getHeuristicsThatUseParam( void ) const  override { return std::vector<int>({1,3,4,8}); };
    
    void 
    setHeuristicParam( double d )  override { m_param = d; } 
    
    double 
    getHeuristicParam( void ) const override { return m_param; } 
    
    void 
    setMemorySize( int mem_num ) override;
    
    void 
    initialiseSolution( int mem_idx ) override;
    
    void 
    applyHeuristic( int h_id, int mem_idx1, int mem_idx2 ) override;
    
    void 
    applyHeuristic( int h_id, int mem_idx1, int mem_idx2, int mem_idx3 ) override;
    
    void 
    load( const std::string& instance_id )  override;
    
    void 
    copySolution( int mem_idx1, int mem_idx2 ) override;
    
    // cost, head_deficit, resiliance
    HHObjective 
    evalauteObj( int mem_idx ) const override ;
    
    std::string  
    toString( void ) const override;
    
    std::string 
    solutionToString( int mem_idx ) const override;
    
    bool
    compareSolutions( int mem_idx1, int mem_idx2 ) const override; 
    
    //  The Euclidian distance
    double
    distance( int mem_idx1, int mem_idx2 ) const override;

    
    //
    // HOWSServer interface
    //
    
   // HydProblem& 
   // getHydProblem( void ) { return m_problem; }
    
   // HydEPANET& 
   // getHydEPANET( void ) { return m_epanet; }
    
    
    HydNetwork* 
    getHydNet( void ) { return m_problem.getHydNet(); }
    
    //!<  Evaluate a solution
    HHObjective
    evalauteObj( const HHSolution& solution ) const;
    
    void 
    initialiseSolution( HHSolution& solution );


    /// map an EPANET index to a solution index where possible, otherwise return -1
    const std::vector<int>&  
    dvar2spec(void) const { return m_problem.dvar2spec(); }
    
protected:
    
    //
    // random selection methods
    //
    
     //!<  return a random index [0,...,N-1]
    int rndInt( long N ) { return (int) (m_ran.ran() * N); }
    
    //!< return a random component type (that has a non-zero number of components)
    int rndType( void ) { return m_compType[rndInt(m_compType.size())]; }
    
    //!< return a random element of component type t
    int rndType( int t ) { return m_compTypeIdx[t][rndInt(m_compTypeIdx[t].size())]; }
    
    //!< classical roulette wheel selection based on positive obj_vals
    int roulette( const std::vector<double>& obj_vals );
    
    
    //
    // low level heuristics
    //
    
    void
    random( HHSolution& solution, double param = 0.0 );
    
    void
    mutate1( HHSolution& solution, double param = 0.0 );
    
    void
    swap( HHSolution& solution, double param = 0.0 );
    
    void
    mutate2( HHSolution& solution, double param = 0.0 );
        
    void
    shuffle1( HHSolution& solution, double param = 0.0 );
    
    void
    ruin1( HHSolution& solution, double param = 0.0 );

    void
    crossover1( const HHSolution& s1, const HHSolution& s2, HHSolution& s3, double param = 0.0 );
    
    void
    crossover2( const HHSolution& s1, const HHSolution& s2, HHSolution& s3, double param = 0.0 );
    
    void
    crossover3( const HHSolution& s1, const HHSolution& s2, HHSolution& s3, double param = 0.0 );
    
    void
    hd_heuristic( HHSolution& solution, double param = 0.0 );
    
    /// end of low level heuristics      


    
    double m_param;                                 /// low level heuritsic parameter (if required)
    
    std::vector<std::vector<int>> m_compTypeIdx;    /// decision variable index for each component type
    std::vector<int> m_compType;                    /// index for each type of component present
    
    std::vector<double> m_pipeInfluence;
    
    mutable HHSolution  m_oldSolution;              /// last solution cached 
    mutable HHObjective m_oldValue;                 /// last solution's objective functions cached

    KBHeuristic m_kbh;                              /// Knowledge Based Heuristics
    HDHeuristic m_hdh;                              /// Human Derived Heuristics
    
    HydProblem  m_problem;                          /// The decision variables, options and constraints
    HydEPANET   m_epanet;                           /// The haudralic simulator which loads/runs a network
    URand m_ran;
    
    mutable Logger m_logger;
    
    static const std::vector<std::string> m_llhTypes;
};

#endif


 
