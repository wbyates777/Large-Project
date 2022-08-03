/* HHProblem 02/06/2017

 $$$$$$$$$$$$$$$$$
 $   HHProblem.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Abstract base class interface for a problem (domain)
 
 Each new problem must implement these methods
 
*/


#ifndef __HHPROBLEM_H__
#define __HHPROBLEM_H__

#ifndef __HHTYPES_H__
#include "HHTypes.h"
#endif

#include <string>
#include <vector>


class Logger;


class HHProblem
{
public:
    
    HHProblem( void )=default;
    virtual ~HHProblem( void )=default;

    virtual void
    seed( unsigned long s ) = 0;
    
    virtual bool
    check( void ) const { return true; }
    
    virtual void 
    load( const std::string& instance_id )  = 0; 
    
    virtual int
    getNumberOfInstances( void ) const = 0;
    
    virtual std::vector<std::string>
    getHeuristicTypes( void ) const  = 0;
    
    virtual std::string
    getHeuristicType( int i ) const = 0;
    
    virtual std::vector<int> 
    getHeuristicsThatUseParam( void ) const = 0;
    
    virtual void 
    setHeuristicParam( double d ) = 0;
    
    virtual double 
    getHeuristicParam( void ) const = 0;
    
    virtual int 
    getNumberOfHeuristics( void ) const = 0;
    
    virtual void 
    setMemorySize( int mem_idx ) = 0;
    
    virtual void 
    initialiseSolution( int mem_idx ) = 0;
    
    virtual void 
    applyHeuristic( int h_id, int mem_idx1, int mem_idx2 ) = 0;
    
    virtual void 
    applyHeuristic( int h_id, int mem_idx1, int mem_idx2, int mem_idx3 ) = 0;

    
    virtual void 
    copySolution( int mem_idx1, int mem_idx2 ) = 0;
    
    virtual HHObjective
    evalauteObj( int mem_idx ) const = 0;
    
    virtual std::string 
    toString( void ) const = 0;
    
    virtual std::string 
    solutionToString( int mem_idx ) const = 0;
    
    virtual bool
    compareSolutions( int mem_idx1, int mem_idx2 ) const = 0; 
    
    
    //
    // Non HyFlex interface members
    //
    
    virtual HHSolution&
    getSolution( int mem_idx ) { return m_solution[mem_idx]; }
    
    virtual void
    setSolution( const HHSolution& sol, int mem_idx ) { m_solution[mem_idx] =  sol; }
    
    virtual double
    distance( int mem_idx1, int mem_idx2 ) const { return 0.0; }


    virtual Logger*
    getLogger( void ) const  { return nullptr; }
    
protected:

    std::vector<HHSolution> m_solution;
};



#endif


