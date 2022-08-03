/* HydDVariable 26/02/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydDVariable.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 A decision variable for a link component - i.e a pipe, pump or valve id

*/


#ifndef __HYDDVARIABLE_H__
#define __HYDDVARIABLE_H__


#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#include <string>

class HydDVariable
{
public:

    HydDVariable( void ) : m_index(-1), m_hydType(), m_ID(), m_group("default") {}
    HydDVariable( const std::string& hid, const std::string& group, HydType htype ) : m_index(-1), m_hydType(htype), m_ID(hid), m_group(group) {}
    ~HydDVariable( void )=default;

    const std::string& 
    ID(void) const { return m_ID; }
    
    void 
    ID(const std::string& id) { m_ID = id; }
    
    const std::string& 
    group(void) const { return m_group; }
    
    void  
    group(const std::string& g) { m_group = g; }
    
    
    HydType 
    hydType(void) const { return m_hydType; }
    
    void
    hydType(HydType t) { m_hydType = t; }
    
    
    // EPANET index 1,2,...
    int index(void) const { return m_index; }
    void setIndex(int i)  { m_index = i; }
    
private:

    int m_index;
    HydType m_hydType;
    std::string m_ID;
    std::string m_group;
};

#endif


