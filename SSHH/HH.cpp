/* HH 22/06/2017

 $$$$$$$$$$$$$$
 $   HH.cpp   $
 $$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HH_H__
#include "HH.h"
#endif



#ifndef __RETURNFUNCTIONS_H__
#include "ReturnFunctions.h"
#endif

#include <cmath>
#include <assert.h>
#include <fstream>
#include <iomanip>

#define PREC(X)  std::setprecision((X)) << std::fixed 
 



HH::HH( void ): m_problem(nullptr), 
            m_evals(0), 
            m_best_iter(0), 
            m_iters(0),
            m_best_obj(0.0), 
            m_start(0), m_finish(0), 
            m_llh_start(0), m_llh_finish(0), 
            m_name(),
            m_usage(),
            m_heuristics(),
            m_llhNames(),
            m_history(), 
            m_toScalar(HHFunc::volume) {}

HH::HH( const std::string& n ): m_problem(nullptr), 
                            m_evals(0), 
                            m_best_iter(0), 
                            m_iters(0),
                            m_best_obj(0.0), 
                            m_start(0), m_finish(0),
                            m_llh_start(0), m_llh_finish(0),
                            m_name(n), 
                            m_usage(),
                            m_heuristics(),
                            m_llhNames(),
                            m_history(), 
                            m_toScalar(HHFunc::volume) {}

HH::~HH( void ) 
{ 
    // we don't own problem memory; do not delete
    m_problem = nullptr; 
    m_iters = m_evals = m_best_iter = m_start = m_finish = m_llh_start = m_llh_finish = 0;
    m_best_obj = 0.0;
}

void
HH::setHeuristics( HHProblem& problem, const std::vector<int>& omitLLH )
{
    int llh_num = problem.getNumberOfHeuristics();

    assert(omitLLH.size() < llh_num);

    m_llhNames.resize(llh_num);
    for (int i = 0; i < llh_num; ++i)
        m_llhNames[i] = problem.getHeuristicType(i)[0] + std::to_string(i);

    m_heuristics.clear();

    if (omitLLH.empty())
    {
        for (int h = 0; h < llh_num; ++h)
            m_heuristics.push_back(h);
    }
    else
    {
         std::vector<int> my_omit = omitLLH;
         std::sort(my_omit.begin(), my_omit.end());
         int c = 0;
         for (int h = 0; h < llh_num; ++h)
         {
             if (c < my_omit.size() && my_omit[c] == h)
                 ++c;
             else m_heuristics.push_back(h);
         }
    }
}


double
HH::finalLogReturn( void ) const
{
    if (m_history.empty())
        return 0.0;

    return logRet( m_best_obj, m_history[0].cur_obj() );
}


double
HH::finalDecimalReturn( void ) const
{
    if (m_history.empty())
        return 0.0;

    return decRet(m_best_obj, m_history[0].cur_obj());
}


void
HH::logProblem( int iterations )
{
    if (m_logger.level() >= 1)
    {
        int llh_num = (int) m_heuristics.size();
        assert(llh_num > 0 && llh_num < 256); // see SSHHSel
    
        Message txt1(1);
        txt1 << "Optimising problem for " << iterations << " iterations";
        m_logger.logMsg(txt1);

        Message txt2(1);
        txt2 << llh_num << " low level heuristics: ";
        for (int i = 0; i < llh_num; ++i)
            txt2 << m_llhNames[m_heuristics[i]] << ' ';
        m_logger.logMsg(txt2);
        
        m_logger.flush();
    }
}
   

void
HH::beginRunLog( void )
{
    m_start  = m_finish = std::time(0);

    m_llh_start = m_llh_finish = 0;
    m_history.clear();
    m_history.reserve(150);
    
    if (m_logger.level() >= 3)
    {
        Message txt(3);
        txt << "Iter" << '\t' << "LLH" << '\t' << "Param" << '\t' << "Cur Obj " << '\t' << "New Obj " << '\t' << "Accept" << '\t' << "Time";
        m_logger.logMsg(txt);
        m_logger.flush();
    }        
}
   

void 
HH::updateRunLog( int llh, int param, int accepted, double cur_obj, double new_obj ) 
{
    char htype = m_problem->getHeuristicType(llh)[0];
    char acc = (accepted == 0) ? 'F' : ((accepted == 1) ? 'T' : ' ');
    
    HHState s(llh, param, htype, acc, m_llh_start, m_llh_finish, cur_obj, new_obj);
    
    m_history.push_back( s );
    
    if (m_logger.level() >= 3)
    {
        Message txt(3);
        txt << m_history.size() - 1 << '\t' << s;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
}


void
HH::endRunLog( void )
{
    m_finish = std::time(0);
    
    if (m_logger.level() >= 1)
    {
        double initial_obj = m_history[0].cur_obj();
        double lr = finalLogReturn();
        int    dp = 4;
        
        Message txt1(1);
        txt1 << "Initial " <<  PREC(dp) << initial_obj << " --- Final " <<  PREC(dp)  << m_best_obj;
        m_logger.logMsg(txt1);
        
        double pr = finalDecimalReturn()  * 100.0;
        long secs = m_finish - m_start; 
        
        Message txt2(1);
        txt2 << "Final log return " << PREC(dp) << lr << " (" << PREC(dp) << pr << " %) at iteration " << m_best_iter  << " in " << secs << " seconds";
        m_logger.logMsg(txt2);
        
        Message txt3(1);
        txt3 << m_evals << " objective function evaluations"; 
        m_logger.logMsg(txt3);
        
        Message txt4(1);
        txt4 << "Usage ";
        for (int i = 0; i < m_usage.size(); ++i)
            txt4 << m_usage[i] << ' ';
        m_logger.logMsg(txt4);
        
        m_logger.flush();
    }  
}
   
