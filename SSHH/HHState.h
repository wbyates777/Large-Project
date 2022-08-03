/* HHState 19/06/2017

 $$$$$$$$$$$$$$$$$
 $   HHState.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 Encapsulates the state of the SSHH algorithm at a point in time.
 Used for data logging.

*/


#ifndef __HHSTATE_H__
#define __HHSTATE_H__

#include <ctime>
#include <iostream>


class HHState
{
public:
    HHState( void ):m_llh(0), 
                    m_param(0), 
                    m_htype(0), 
                    m_accepted(0), 
                    m_start(0), 
                    m_finish(0), 
                    m_cur_obj(0.0), 
                    m_new_obj(0.0) 
    {}
    
    HHState( int h, int p, char ht = 0, char a = 0, clock_t s = 0, clock_t f = 0, double c = 0.0, double n = 0.0  ): 
                        m_llh(h), 
                        m_param(p), 
                        m_htype(ht), 
                        m_accepted(a), 
                        m_start(s), 
                        m_finish(f), 
                        m_cur_obj(c), 
                        m_new_obj(n) 
    {}
    
    ~HHState( void ) { m_cur_obj = m_new_obj = 0.0; m_llh = m_param = m_htype = m_accepted = m_start = m_finish = 0; }

    int
    llh( void ) const { return m_llh; }
    
    void
    llh( int s ) { m_llh = s; }
    
    
    int
    param( void ) const { return m_param; }
    
    void
    param( int p ) { m_param = p; }
 
    
    char
    htype( void ) const { return m_htype; }
    
    void
    htype( char ht ) { m_htype = ht; }
    
    
    char
    accepted( void ) const { return m_accepted; }
    
    void
    accepted( char a ) { m_accepted = a; }
    
    
    clock_t
    start( void ) const { return m_start; }
    
    void
    start( clock_t s ) { m_start = s; }
    
    
    clock_t
    finish( void ) const { return m_finish; }
    
    void
    finish( clock_t f ) { m_finish = f; }
    
    
    double
    cur_obj( void ) const { return m_cur_obj; }
    
    void
    cur_obj( double c ) { m_cur_obj = c; }
    
    
    double
    new_obj( void ) const { return m_new_obj; }
    
    void
    new_obj( double n ) { m_new_obj = n; }
    
private:

    unsigned char m_llh;        //!< The low level heuristic
    unsigned char m_param;      //!< The low level heuristic's parameter
    unsigned char m_htype;      //!< The low level heuristic'c class (i.e. 'C', 'L', 'R', or ''M')
    unsigned char m_accepted;   //!< The acceptance decision
    clock_t  m_start;           //!< The start time for this low level heuristic
    clock_t  m_finish;          //!< The finishing time for this low level heuristic
    double m_cur_obj;           //!< The current objective function value
    double m_new_obj;           //!< The new objective function value

};
    
inline std::ostream&
operator<<( std::ostream& ostr, const HHState& s )
{
    ostr.precision(6);

    ostr << s.htype() << s.llh() << '\t' << s.param() / 10.0
         << '\t' << s.cur_obj() << '\t'
         << s.new_obj() << '\t' << s.accepted()
         << '\t' << double(s.finish()  - s.start()) / CLOCKS_PER_SEC;

    return ostr;
}
    


#endif


