/* HH 22/06/2017

 $$$$$$$$$$$$
 $   HH.h   $
 $$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Abstract base class interface for hyper-heuristics
 
 This class also takes care of logging the hyper-heuristic's state during a run
 
 Assumes that the objective function to be MINIMISED is positive and non-zero
 
*/


#ifndef __HH_H__
#define __HH_H__

#ifndef __HHSTATE_H__
#include "HHState.h"
#endif

#ifndef __HHPROBLEM_H__
#include "HHProblem.h"
#endif

#ifndef __HHFUNC_H__
#include "HHFunc.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

class HH
{
public:

    enum SolType { CUR_SOL = 0, NEW_SOL = 1, BEST_SOL = 2, CROSS_SOL = 3 };
    
    HH( void );
    HH( const std::string& n );
    virtual ~HH( void );

    /// The hyper-heuristic's name
    const std::string&
    name( void ) const { return m_name; }
    
    /// Set the hyper-heuristic's name
    void
    name( const std::string& n ) { m_name = n; }
    
    /// Initialise the hyper-heuristic for this problem
    virtual void
    initialise( HHProblem& problem, 
                const HHObjFunc& f = HHFunc::volume, 
                const std::vector<int>& omitLLH = std::vector<int>() ) = 0;

    /// Optimise the problem for iters iterations
    virtual double
    solve( int iters = 1 ) = 0;
    
    // To Be Added
    // virtual const std::string&
    // solution( const std::string& fileName ) const = 0;
    
    /// The random number generator's seed
    virtual int
    seed( void ) const { return -1; }
    
    /// Set the random number generator's seed
    virtual void
    seed( unsigned int ) { /* do nothing */ ; }
    
    /// The final logarithmic return for this run
    double
    finalLogReturn( void ) const;
    
    /// The final decimal return for this run
    double 
    finalDecimalReturn( void ) const;
    
    /// The initial objective function value
    double
    initialObj( void ) const { return (!m_history.empty()) ? m_history[0].cur_obj() : 0.0; }

    /// The besy objective function value
    double
    bestObj( void ) const { return m_best_obj; }
    
    virtual const std::vector<HHObjective>
    bestObjectives( void ) const { return std::vector<HHObjective>(); }
    

    /// Combine or convert objective values into a single value
    ObjType 
    toScalar( const HHObjective& objs ) const { return m_toScalar(objs); }
    
    /// The overall runtime for this run in seconds
    long
    runTime( void ) const { return m_finish -  m_start; }

    /// The number of objective function evaluations
    int
    objEvals( void ) const { return m_evals; }
    
    /// The number of iterations to the minima
    int
    bestIter( void ) const { return m_best_iter; }
    
    /// Get the run history
    const std::vector<HHState>&
    history( void ) const { return m_history; }

    virtual void
    logProblem( int iterations );

    virtual void
    beginRunLog( void );

    void
    updateRunLog( int llh, int param, int accepted, double cur_obj, double new_obj );

    virtual void
    endRunLog( void );

    Logger*
    getLogger( void ) const { return &m_logger; }

protected:
   
    void
    setHeuristics( HHProblem& problem, const std::vector<int>& omitLLH );

    HHProblem *m_problem;
    
    int      m_evals;             //!< The number of objective function evaluations
    int      m_best_iter;         //!< The iteration when the best solution occurred
    int      m_iters;             //!< The number of iterations performed so far
    double   m_best_obj;              //!< The best objective function value found so far
    double   m_cur_obj;           //!< The current objective function value
    double   m_new_obj;           //!< The new objective function value
    std::time_t   m_start;        //!< The start time for a run
    std::time_t   m_finish;       //!< The finishing time for a run
    std::clock_t  m_llh_start;    //!< The start time for a low level heuristic
    std::clock_t  m_llh_finish;   //!< The finishing time for a low level heuristic
    std::string m_name;
    std::vector<int> m_heuristics;
    std::vector<int> m_usage;
    std::vector<std::string> m_llhNames;
    std::vector<HHState> m_history;
    
    HHObjFunc m_toScalar;    //!< Combines the objective values into a single value 

    mutable Logger m_logger;
};
    



#endif


