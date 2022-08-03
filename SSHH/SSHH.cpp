/* SSHH 13/09/2019
 
 $$$$$$$$$$$$$$$$
 $   SSHH.cpp   $
 $$$$$$$$$$$$$$$$
 
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
 
 */


#ifndef __SSHH_H__
#include "SSHH.h"
#endif

#ifndef __BAUMWELCH_H__
#include "BaumWelch.h"
#endif

#ifndef __HMMHELPER_H__
#include "HMMHelper.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <cmath>
#include <numeric>
#include <assert.h>
#include <fstream>
#include <iomanip>

#define PREC(X)  std::setprecision((X)) << std::fixed 

SSHH::SSHH( void ): HH("SSHH"),
                    m_multi_obj(1),
                    m_cross_pool(5),
                    m_mem_idx(0),
                    m_allowance(0.0), 
                    m_learn_rate(0.1),
                    m_hmm(),
                    m_ran(18),
                    m_selector() 
{
    m_logger.getLogLevel( "SSHH" );
    m_name = "SSHH";
}

SSHH::~SSHH( void ) 
{
    m_cross_pool = m_iters = m_mem_idx = m_time_to_initialise = m_time_last_improvement = 0;
    m_allowance = m_learn_rate = 0.0;
}

void
SSHH::initialise( HHProblem& problem, const HHObjFunc& f, const std::vector<int>& omitLLH )
{
    // omit any LLH selected by the user
    setHeuristics(problem, omitLLH);
    
    // set up problem and objective function
    m_problem  = &problem;
    m_toScalar = f;
    
    m_problem->setMemorySize(3 + m_cross_pool);
    m_problem->initialiseSolution(CUR_SOL);     /// current solution
    m_problem->initialiseSolution(NEW_SOL);     /// new solution
    m_problem->initialiseSolution(BEST_SOL);    /// best solution

    // crossover management strategy taken from AdaptHH by Misir
    for (int i = 0; i < m_cross_pool; ++i)
        m_problem->initialiseSolution(CROSS_SOL+i);  /// crossover solution
    //
    
    m_iters                 = 0;
    m_time_last_improvement = 0;
    m_time_to_initialise    = 6;    /// time_to_initialise_after_no_improvement (was 6)  
    m_allowance             = 1.05; /// allow a 5% increase in objective function value;
    m_learn_rate            = 0.2;
    m_mem_idx               = CUR_SOL;
    m_evals                 = 0;
    m_best_iter             = 0;
   
    HHObjective objs = m_problem->evalauteObj(CUR_SOL);
    m_new_obj = m_cur_obj = m_best_obj = toScalar(objs);
    
    // is this a multi-objective problem
    if (objs.size() == 1 || m_multi_obj == 0)
        m_multi_obj = 0;
    else m_multi_obj = 1;
    
    m_archive.clear();
    m_archive.update(m_problem->getSolution(CUR_SOL), objs, 0);
    
    m_history.clear();
    
    // set up the hidden Markov model
    std::vector<std::string> states;
    for (int i = 0; i < m_heuristics.size(); ++i)
        states.push_back( m_llhNames[m_heuristics[i]] );

 
    m_hmm.set(states, {states, {"1", "2","3","4","5"}, {"F","T"}});
    // sometimes using equiprobable is better; 
    m_hmm.emiss_probs(0) = HMMHelper::identity(m_hmm.emiss_probs(0).rows()); 
   
    m_selector.set( &m_hmm );
    
    m_usage.resize(m_llhNames.size(), 0);
}


void
SSHH::initialise( HHProblem& problem, const HHSolution& initialSolution, const HHObjFunc& f, const std::vector<int>& omitLLH )
{
    // omit any LLH selected by the user
    setHeuristics(problem, omitLLH);
    
    // set up problem and objective function
    m_problem  = &problem;
    m_toScalar = f;
    
    m_problem->setMemorySize(3 + m_cross_pool);
    //m_problem->initialiseSolution(CUR_SOL);     /// current solution
    m_problem->setSolution(initialSolution, CUR_SOL);
    m_problem->initialiseSolution(NEW_SOL);     /// new solution
    m_problem->initialiseSolution(BEST_SOL);    /// best solution

    // crossover management strategy taken from AdaptHH by Misir
    for (int i = 0; i < m_cross_pool; ++i)
        m_problem->initialiseSolution(CROSS_SOL+i);  /// crossover solution
    //
    
    m_iters                 = 0;
    m_time_last_improvement = 0;
    m_time_to_initialise    = 6;    /// time_to_initialise_after_no_improvement (was 6)  
    m_allowance             = 1.05; /// allow a 5% increase in objective function value;
    m_learn_rate            = 0.2;
    m_mem_idx               = CUR_SOL;
    m_evals                 = 0;
    m_best_iter             = 0;
   
    HHObjective objs = m_problem->evalauteObj(CUR_SOL);
    m_new_obj = m_cur_obj = m_best_obj = toScalar(objs);
    
    // is this a multi-objective problem
    if (objs.size() == 1 || m_multi_obj == 0)
        m_multi_obj = 0;
    else m_multi_obj = 1;
    
    m_archive.clear();
    m_archive.update(m_problem->getSolution(CUR_SOL), objs, 0);
    
    m_history.clear();
    
    // set up the hidden Markov model
    std::vector<std::string> states;
    for (int i = 0; i < m_heuristics.size(); ++i)
        states.push_back( m_llhNames[m_heuristics[i]] );

 
    m_hmm.set(states, {states, {"1", "2","3","4","5"}, {"F","T"}});
    // sometimes using equiprobable is better; 
    m_hmm.emiss_probs(0) = HMMHelper::identity(m_hmm.emiss_probs(0).rows()); 
   
    m_selector.set( &m_hmm );
    
    m_usage.resize(m_llhNames.size(), 0);
}

void
SSHH::endRunLog( void )
{
    HH::endRunLog();
    
    if (m_logger.level() >= 1)
    {
        // std::cout << m_archive.values() << std::endl;
        Message txt1(1);
        txt1 << m_archive.values().size() << " nondominated solutions";
        m_logger.logMsg(txt1);
        m_logger.flush();
    }
    
    // print out the learned subsequences
    if (m_logger.level() >= 2)
    {
        std::vector<std::string> subseqs = learnedLog();
        Message header(2);
        header << subseqs.size() << " subsequences learned (with parameters)";
        m_logger.logMsg(header);
        for (int i = 0; i < subseqs.size(); ++i)
        {
            Message txt1(4);
            txt1 << subseqs[i];
            m_logger.logMsg(txt1);
        }
        m_logger.flush();
    }
}

std::string
SSHH::print(const std::vector<SSHHSel>& learned) const
{
    std::ostringstream txt2;
    int offset = 1;
    
    if (learned.size() == 1)
        offset = 0;
    
    txt2 << '\t' << learned[offset].iteration() << '\t';
    for (int j = offset; j < learned.size(); ++j)
        txt2 << m_llhNames[m_heuristics[learned[j].llh()]] << ' ';
    
    txt2 << '\t';
    
    /*for (int j = offset; j < learned[i].size(); ++j)
     {
     txt2 << learned[i][j].param();
     if (j < learned[i].size() - 1)
     txt2 << ' ';
     }*/
    
    std::cout << txt2.str() << std::endl;
    
    return txt2.str();
    
}

std::vector<std::string>
SSHH::learnedLog( void ) const
{
    std::vector<std::string> retVal;

    const std::vector<std::vector<SSHHSel>>& learned = m_selector.learned();

    for (int i = 0; i < learned.size(); ++i)
    {
        std::ostringstream txt2;
        int offset = 1;

        if (i == 0 && learned[i].size() == 1)
            offset = 0;

        if (offset < learned[i].size())
        {
            txt2 << i + 1 << '\t' << learned[i][offset].iteration() << '\t';
            for (int j = offset; j < learned[i].size(); ++j)
                txt2 << m_llhNames[m_heuristics[learned[i][j].llh()]] << ' ';

            txt2 << '\t';

            /*for (int j = offset; j < learned[i].size(); ++j)
            {
                txt2 << learned[i][j].param();
                if (j < learned[i].size() - 1)
                    txt2 << ' ';
            }*/

            retVal.push_back( txt2.str() );
        }
    }

    return retVal;
}


void
SSHH::addCrossSol( int mem_idx )
// replace at random
{
    int idx = rndInt(m_cross_pool);
    m_problem->copySolution(mem_idx, CROSS_SOL + idx);
}


int
SSHH::roulette( const std::vector<double>& obj_vals )
// select a chromosome using roulette wheel selection
// without replacement
// returns an index into the table
{          
    int numVals = (int) obj_vals.size();
    
    if (numVals == 0)
        return -1;
    
    if (numVals == 1)
        return 0;
    
    double total = util::sum(obj_vals);
    double prob  = m_ran.ran() * total;   
    double sum   = 0.0;

    for (int i = 0; i < numVals; ++i) 
    {
        sum += obj_vals[i];
        if (sum > prob)
            return i;
    }
    
    return -1;
}

int
SSHH::getCrossSol( void )
{
    int mem_idx = -1;
    
    if (!m_multi_obj)
    {
        // select at random from the pool - single objective - SSHH
        int idx = rndInt(m_cross_pool);
        mem_idx = CROSS_SOL + idx;
    }
    else
    {
        // select at random from the archive - SSHH
        int idx = rndInt(m_archive.size());
        m_problem->setSolution(m_archive[idx], CROSS_SOL);
        mem_idx = CROSS_SOL;
    }
    
    //
    // Example alternative crossover mechanisms
    //
    /*  
    int idx = rndInt(m_archive.size());
    if (m_archive.size() > 5)
    {
        std::vector<double> crowd = m_archive.crowding();
        idx = roulette(crowd);
    }
    m_problem->setSolution(m_archive[idx], CROSS_SOL);
    return CROSS_SOL;
    */
    

    return mem_idx;
}


void 
SSHH::applyHeuristic( const SSHHSel& sel ) 
// apply the selected heuristic to the solution specified by m_mem_idx
{ 
    double param = (double) sel.param(); 
    m_problem->setHeuristicParam(param);

    int h = m_heuristics[sel.llh()];

    if (m_problem->getHeuristicType(h)[0] == 'C') // "CROSSOVER"
    {
        m_llh_start = std::clock();
        m_problem->applyHeuristic(h, m_mem_idx, getCrossSol(), NEW_SOL);
        m_llh_finish  = std::clock();
    }
    else
    {
        m_llh_start = std::clock();
        m_problem->applyHeuristic(h, m_mem_idx, NEW_SOL);
        m_llh_finish  = std::clock();
    }
    
    // work on new solution
    m_mem_idx = NEW_SOL;
}

int 
SSHH::acceptance( void ) 
// do we accept the new solution?
// if so replace the current solution with the new solution
{
    int accept = 0;

    if ((m_new_obj < m_cur_obj) || (m_new_obj < m_best_obj * m_allowance))
    {
        m_problem->copySolution(NEW_SOL, CUR_SOL);
        m_cur_obj = m_new_obj;
        accept = 1;
    }
    
    // work on current solution
    m_mem_idx = CUR_SOL;
    
    return accept;
}


double
SSHH::solve( int iterations )
// optimise the problem (with online learning)
{
    const double ALLOWANCE = 1.05; // allow a 5% increase in objective function value
    
    if (m_history.empty())
        m_history.reserve(iterations);
    
    HHObjective objs;
    
    for (int i = 0; i < iterations; ++i, ++m_iters)
    {
        if (m_iters - m_time_last_improvement > m_time_to_initialise)
        {
            m_time_last_improvement = m_iters;
            m_allowance -= (ALLOWANCE - 1.0) / m_time_to_initialise;

            if (m_allowance < 1.0)
                m_allowance =  1.0; 
        }
        
        // select a low level heuristic, its parameters and an acceptance check decision
        SSHHSel sel = m_selector.nextSel();

        applyHeuristic( sel );
       
        ++m_usage[ m_heuristics[sel.llh()] ];
        
        // set the logging variables; accepted of -1 means no decision on accept/reject has been taken yet
        double cur_obj_log = m_cur_obj; int accepted = -1;

        // we can evaluate here for logging purposes; 
        // for real world situations comment this out 
        // HHObjective objs = evaluateObj(); 
        // m_new_obj    = toScalar(objs);
        
        if ( sel.acceptCheck() == 1 )
        {
            // if the member m_new_obj has been set above (for logging purposes)
            // all we need do is increment the count i.e comment out evaluateObj and toScalar
            ++m_evals;  
            HHObjective objs = evaluateObj();
            m_new_obj = toScalar(objs);

            // should only be called after evaluate
            m_archive.update(m_problem->getSolution(NEW_SOL), objs, i);
            
            // do we accept the new solution?
            accepted = acceptance();
            
            // if the new objective function values dominate the best objective function values
            if (m_new_obj < m_best_obj)
            {
                m_best_obj      = m_new_obj;
                m_best_iter = m_time_last_improvement = m_iters;
                m_allowance = ALLOWANCE;
                m_problem->copySolution(NEW_SOL, BEST_SOL);
                
                if (!m_multi_obj)
                    addCrossSol(NEW_SOL);

                m_selector.learn(std::vector<double>(4, m_learn_rate)); // online learning
            }

            m_selector.clearHistory();
            
            int idx = (int) m_history.size() - 1;
            if (idx >= 0)
            {
                while (idx >= 0 && m_history[idx].accepted() == ' ')
                    m_history[idx--].accepted( (accepted == 0) ? 'F' : ((accepted == 1) ? 'T' : ' '));
            }
        } 

        // data logging
        updateRunLog( m_heuristics[sel.llh()], sel.param(), accepted, cur_obj_log, m_new_obj );
    }
    
    return finalLogReturn();
}


bool
SSHH::save( const std::string& fileName ) const
{
    std::string filename1 = fileName + ".hmm";
    std::string filename2 = fileName + ".txt";
    
    hmm().save(filename1);
    
    std::ofstream to;
    to.open( filename2 );

    if (!to)
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Could not open " << fileName << " for save";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }
    
    to << m_problem->solutionToString(BEST_SOL) << '\n';
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Best solution saved to " << filename2;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    to.close();
    
    return true;
}

bool
SSHH::saveSolution( const std::string& fileName ) const
{
    std::string filename2 = fileName + ".txt";
    
    std::ofstream to;
    to.open( filename2 );
    
    if (!to)
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Could not open " << fileName << " for save";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }
    
    to << m_problem->solutionToString(BEST_SOL) << '\n';
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Best solution saved to " << filename2;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    to.close();
    
    return true;
}

//

