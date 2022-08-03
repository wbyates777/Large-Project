/* SSHHSelector 12/06/2017

 $$$$$$$$$$$$$$$$$$$$$$$$
 $   SSHHSelector.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __SSHHSELECTOR_H__
#include "SSHHSelector.h"
#endif

static double __NEAR_ZERO_VALUE__ = 1.0E-6;

inline bool nearZero( double p )  { return ((p > -__NEAR_ZERO_VALUE__) && (p < __NEAR_ZERO_VALUE__)); }



#include <assert.h>



void
SSHHSelector::set( HMM* hmm )
{
    m_hmm = hmm;
    m_selCount = 0;
    m_learnCount = 0;
    m_learned.clear();
    m_history.clear();
    
    m_learned.reserve(100);
    m_history.reserve(100);

    // how many hidden states have nonz-zero probs i.e. they are 'active' 
    m_activeStates.clear();
    m_activeStates.resize(m_hmm->trans_probs().rows(), 0);
    for (int i = 0; i < m_hmm->trans_probs().rows(); ++i)
        for (int j = 0; j < m_hmm->trans_probs().cols(); ++j)
            if (!nearZero(m_hmm->trans_probs()[i][j]))
                ++m_activeStates[i];
    
    // how many emission states have non-zero probs i.e. they are 'active' 
    m_activeEmissions.clear();
    m_activeEmissions.resize(m_hmm->emiss_probs().size());
    for (int k = 0; k < m_hmm->emiss_probs().size(); ++k)
    {
        m_activeEmissions[k].resize( m_hmm->emiss_probs(k).rows(), 0);
        for (int i = 0; i < m_hmm->trans_probs().rows(); ++i)
            for (int j = 0; j < m_hmm->emiss_probs(k).cols(); ++j)
                if (!nearZero(m_hmm->emiss_probs(k)[i][j]))
                    ++m_activeEmissions[k][i];
    }
}

void 
SSHHSelector::learn( const std::vector<double>& inc )
// online learning
{
    m_learnCount++;

    for (int i = (int) m_history.size() - 1; i > 0; --i)
    {
        int prev_state = m_history[i-1].state();
        int curr_state = m_history[i].state();
        
        updateProbs( m_hmm->trans_probs()[prev_state], m_activeStates[prev_state], m_history[i].state(), inc[STATE] );
    
        updateProbs( m_hmm->emiss_probs(0)[curr_state], m_activeEmissions[0][curr_state], m_history[i].llh(), inc[LLH] );
        updateProbs( m_hmm->emiss_probs(1)[curr_state], m_activeEmissions[1][curr_state], m_history[i].param(), inc[PARAM] );
        updateProbs( m_hmm->emiss_probs(2)[curr_state], m_activeEmissions[2][curr_state], m_history[i].acceptCheck(), inc[ACCEPT] );
    }
    
    // save the subsequence we have just learnt from
    m_learned.push_back( m_history );
    
    // reset history; set last good heuristic to be the first
    m_history[0] = m_history.back();
    m_history.resize(1);
}

void
SSHHSelector::updateProbs2( std::vector<double>& probs, int& active, int pIdx, double inc ) const
{  
    probs[pIdx] += inc;
    double sum = (1.0 + inc);
    for (int i = 0; i < probs.size(); ++i)
    {
        probs[i] = (double) (probs[i] / sum);
    }
}

void
SSHHSelector::updateProbs( std::vector<double>& probs, int& active, int pIdx, double inc ) const
{
    const double min_prob =  0.005; // 0.5 / probs.size(); // was 0.005
    const int N = (int) probs.size();

    if (probs[pIdx] + inc > 1.0 - (min_prob * (N - 1)))
    {
        for (int i = 0; i < N; ++i)
            probs[i] = min_prob;
        
        probs[pIdx] = 1.0 - (min_prob * (N - 1));
       
        return;
    }
    
    double sum = 0.0;
    for (int i = 0; i < N; ++i)
        if (i != pIdx && probs[i] > min_prob)
            sum += probs[i];

    for (int i = 0; i < N; ++i)
        if (i != pIdx && probs[i] > min_prob)
            probs[i] -= (probs[i]/sum) * inc;

    probs[pIdx] += inc;
}
  /*
   void
   SSHHSelector::updateProbs( std::vector<double>& probs, int& active, int pIdx, double inc ) const
   {
       double p = 0.0;
       double dec = (active == 1) ? inc : (inc / ((double) active - 1));
       const double min_prob =  0.005; // 0.5 / probs.size(); // was 0.005
       
       for (int i = 0; i < probs.size(); ++i)
       {
           if (i != pIdx)
           {
               if (probs[i] > dec)
               {
                   p += dec; 
                   probs[i] -= dec;               
               }
               else if (probs[i] > min_prob)
               {
                   p += (probs[i] - min_prob);
                   probs[i] = min_prob;
               }
           }
       }
       
       if (nearZero(probs[pIdx]) && p > 0.0)
           ++active;
       
       probs[pIdx] += p;
   }
   */


