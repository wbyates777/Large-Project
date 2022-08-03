/* SSHH 13/09/2019

 $$$$$$$$$$$$$$
 $   SSHH.h   $
 $$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) W. B. Yates. All rights reserved.
 History:
 
 A Multi-objective Sequence-based Selection Hyper-Heuristic (SSHH) 
 
 Assumes that the objective function to be MINIMISED is positive and non-zero
 
 
 Based on the article:
 
 Ahmed Kheiri and Ed Keedwell (2015) A sequence-based selection           
 hyper-heuristic utilising a hidden Markov model. In Proceedings of the   
 2015 on Genetic and Evolutionary Computation Conference (GECCO '15),     
 pages 417-424, New York, NY, USA. ACM 
 
 Archive class is based on a Python implementation by 
 Dr. David Walker
 Postdoctoral Research Fellow, 
 Computer Science & Centre for Water Systems 
 College of Engineering, Mathematics & Physical Sciences 

 Single objective crossover management strategy taken from AdaptHH by Misir
 
 TODO: crossover heuristics must have a name that begins in 'C' while non-crossover heuristics must not - fix this
 
 */

#ifndef __SSHH_H__
#define __SSHH_H__


#ifndef __HH_H__
#include "HH.h"
#endif

#ifndef __HHPROBLEM_H__
#include "HHProblem.h"
#endif

#ifndef __SSHHSELECTOR_H__
#include "SSHHSelector.h"
#endif

#ifndef __ARCHIVE_H__
#include "Archive.h"
#endif

#ifndef __HHSTATE_H__
#include "HHState.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

class SSHH : public HH
{
public:
    
    enum SolType { CUR_SOL = 0, NEW_SOL = 1, BEST_SOL = 2, CROSS_SOL = 3 };
    
    SSHH( void );
    ~SSHH( void ) override;

    void
    initialise( HHProblem& problem, 
                const HHObjFunc& f = HHFunc::volume,  
                const std::vector<int>& omitLLH = std::vector<int>() ) override;
    
    void
    initialise( HHProblem& problem, 
                const HHSolution& initialSolution,
                const HHObjFunc& f = HHFunc::volume,  
                const std::vector<int>& omitLLH = std::vector<int>() );

    // bool
    // settings( const SSHHConfig& sshConfig );
    
    void
    setMultiObj( bool mo ) { m_multi_obj =  mo; }
    
    double
    solve( int iterations = 1 ) override;
    
    const HMM&
    hmm( void ) const { return m_hmm; }
    
    HMM&
    hmm( void ) { return m_hmm; }

    int
    seed( void ) const override { return m_hmm.seed(); }
    
    void
    seed( unsigned int s ) override { m_hmm.seed( s ); }

    void
    learnRate( double lr ) { m_learn_rate = lr; }

    const std::vector<std::vector<SSHHSel>>&
    learned( void ) const { return m_selector.learned(); }

    std::vector<std::string>
    learnedLog( void ) const;

    // return the best nondominated objective function values found
    const std::vector<HHObjective>
    bestObjectives( void ) const override { return m_archive.values(); }
    
    const Archive&
    archive( void ) const { return m_archive; }
    
    bool
    save( const std::string& fileName ) const;
    
    bool
    saveSolution( const std::string& fileName ) const;

    void
    endRunLog( void ) override;

    
private:

    //! return a random index [0,...,N-1]
    int rndInt( long N ) { return (int) (m_ran.ran() * N); }
    
    int
    roulette( const std::vector<double>& obj_vals );
    
    void 
    applyHeuristic( const SSHHSel& sel );
    
    void
    addCrossSol( int mem_idx );
    
    int
    getCrossSol( void );

    
    int 
    acceptance( void );
    
    HHObjective 
    evaluateObj( void ) {  return m_problem->evalauteObj(m_mem_idx);  } 
    
    double
    run( int iterations );
    
    std::string
    print( const std::vector<SSHHSel>& learned ) const;
    

    int    m_multi_obj;
    int    m_cross_pool;
    int    m_time_to_initialise;
    int    m_time_last_improvement;
    int    m_mem_idx;               //!< The solution are currently working on
    
    double m_allowance;             //!< Allow solutions close to m_best_obj to be accepted
    double m_learn_rate;            //!< Learning rate 
    
    HMM m_hmm;
    URand m_ran;
    SSHHSelector m_selector;
    Archive m_archive;
};



#endif


