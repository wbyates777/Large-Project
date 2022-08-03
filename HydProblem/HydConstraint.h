/* HydConstraint 26/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$
 $   HydConstraint.h   $
 $$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Network constraints for node components - i.e pressure constrains on junctions
 
*/


#ifndef __HYDCONSTRAINT_H__
#define __HYDCONSTRAINT_H__

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#include <string>
#include <vector>


class HydConstraint
{
public:

    HydConstraint( void ) : m_index(-1), m_ID(), m_values() {}
    HydConstraint( const std::string& hid, const std::vector<HydFloat>& value ) : m_index(-1), m_ID(hid), m_values(value) {}
    
    ~HydConstraint( void )=default;

    const std::string& 
    ID(void) const { return m_ID; }

    void 
    ID(const std::string& id) { m_ID = id; }
    
    const std::vector<HydFloat>& 
    values(void) const { return m_values; }
         
    void 
    values(const std::vector<HydFloat>& v)  { m_values = v; }
    
    
    // EPANET index 1,2,....
    int index(void) const { return m_index; }
    void setIndex(int i)  { m_index = i; }
    
private:

    int m_index;
    std::string m_ID;
    // vector of head, velocity or pressure constrains - 1 for each scenario 
    std::vector<HydFloat> m_values;
};

#endif


