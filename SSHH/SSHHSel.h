/* SSHHSel 15/06/2017

 $$$$$$$$$$$$$$$$$
 $   SSHHSel.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 Holds the SSHH algorithm's current selection of an LLH, its parameters and accept check decision.
 Note there is a maximum of 255 possible hidden states, llh's, and param values.

*/


#ifndef __SSHHSEL_H__
#define __SSHHSEL_H__

#include <vector>


    
class SSHHSel
{
public:
    SSHHSel( void ) : m_iteration(-1),
                      m_state(0),
                      m_llh(5),
                      m_param(5),
                      m_accept(1) {}
        
   /* SSHHSel( int s, int h, int p, int a ) : m_state(s),
                                            m_llh(h),
                                            m_param(p),
                                            m_accept(a) {} */
    
    explicit SSHHSel( int iter, const std::vector<int>& s ) : m_iteration(iter),
                                                              m_state(s[0]),
                                                              m_llh(s[1]),
                                                              m_param(s[2]),
                                                              m_accept(s[3]) {}
    
    ~SSHHSel( void ) { m_state = m_llh = m_param = m_accept = 0; }

    
    int
    iteration( void ) const { return m_iteration; }

    void
    iteration( int iter ) { m_iteration = iter; }

    int
    state( void ) const { return m_state; }

    void
    state( int s ) { m_state = s; }


    int
    llh( void ) const { return m_llh; }

    void
    llh( int l ) { m_llh = l; }


    int
    param( void ) const { return m_param; }

    void
    param( int p ) { m_param = p; }


    int
    acceptCheck( void ) const { return m_accept; }

    void
    acceptCheck( int a ) { m_accept = a; }
    

private:

    int           m_iteration;  //!< The iteration when this state was created
    unsigned short m_state;      //!< The hidden state index
    unsigned short m_llh;        //!< The low level heuristic
    unsigned short m_param;      //!< The low level heuristic's parameter
    unsigned short m_accept;     //!< The acceptance check decision

};



#endif


