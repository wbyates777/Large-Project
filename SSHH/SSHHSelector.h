/* SSHHSelector 12/06/2017

 $$$$$$$$$$$$$$$$$$$$$$
 $   SSHHSelector.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 The selection mechanism and offline learning component of SSHH

*/


#ifndef __SSHHSELECTOR_H__
#define __SSHHSELECTOR_H__

#ifndef __HMM_H__
#include "HMM.h"
#endif

#ifndef __SSHHSEL_H__
#include "SSHHSel.h"
#endif


    
class SSHHSelector
{
public:
    
    enum SelType { STATE = 0, LLH = 1, PARAM = 2, ACCEPT = 3 };
    
    SSHHSelector( void ) : m_hmm(nullptr), m_selCount(0), m_learnCount(0)  {}
    SSHHSelector( HMM* hmm ) : m_hmm(nullptr), m_selCount(0), m_learnCount(0)  { set( hmm ); }
    ~SSHHSelector( void ) { m_hmm = nullptr; m_learnCount = m_selCount = 0; } // we don't own hmm memory; do not delete
    
    void
    set( HMM* hmm );
    
    const SSHHSel& 
    nextSel( void ) 
    { 
        m_history.push_back( SSHHSel(m_selCount++, m_hmm->run()) );
        return m_history.back(); 
    }
    
    const SSHHSel&
    lastSel( void ) const { return m_history.back(); };
    
    void 
    learn( const std::vector<double>& inc = { 0.1, 0.1, 0.1, 0.1 } );
    
    
    void
    clearHistory( void )
    {
       // m_history[0] = m_history.back(); // original code does this;
        // mine makes more sense and performs better
        m_history.resize(1);
    }

    int
    updates( void ) const { return m_learnCount; }
    
    HMM*
    hmm( void ) { return m_hmm; }
    
    const std::vector<std::vector<SSHHSel>>&
    learned( void ) const { return m_learned; }

//private:
    
    void
    updateProbs( std::vector<double>& probs, int& active, int pIdx, double inc ) const;
   
    void
    updateProbs2( std::vector<double>& probs, int& active, int pIdx, double inc ) const;
    
    
    HMM *m_hmm;
    int  m_selCount;
    int  m_learnCount;
    
    std::vector<int>              m_activeStates;
    std::vector<std::vector<int>> m_activeEmissions;
    
    std::vector<SSHHSel> m_history; 
    std::vector<std::vector<SSHHSel>> m_learned;

};



#endif


