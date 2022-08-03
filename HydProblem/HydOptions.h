/* HydOptions 26/02/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydOptions.h   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 The options for each decision variable - i.e pipe diameters for pipe decision variables
 
 TODO: if using options that are not HydFloats consider using  std::any and a type field instead  
*/


#ifndef __HYDOPTIONS_H__
#define __HYDOPTIONS_H__


#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#include <string>
#include <vector>
#include <algorithm>




class HydOptions
{
public:
 
    HydOptions( void ) : m_hydType(), m_group("default"), m_value(), m_cost()  {}
    
    HydOptions( const std::string& group, HydType htype) : m_hydType(htype), m_group(group), m_value(), m_cost()  {}
   
    ~HydOptions( void )=default;
    
    void
    addOption( HydFloat value, HydFloat cost ) { m_value.push_back(value); m_cost.push_back(cost); }
    
    HydFloat 
    cost(int i) const { return m_cost[i]; }
    
    HydFloat 
    value(int i) const { return m_value[i]; }
    
    int
    size(void) const { return (int) m_value.size(); }
    
    
    const std::vector<HydFloat>& 
    values(void) const { return m_value; }
            
    void
    values(const std::vector<HydFloat>& o)  { m_value = o; }
    
    
    const std::vector<HydFloat>& 
    costs(void) const { return m_cost; }
            
    void 
    costs(const std::vector<HydFloat>& c)  { m_cost = c; }
    
    
    const std::string& 
    group(void) const { return m_group; }
    
    void  
    group(const std::string& g) { m_group = g; }
    
    
    HydType 
    hydType(void) const { return m_hydType; }
    
    void
    hydType(HydType t) { m_hydType = t; }
    
    
    HydFloat
    min_cost( void ) const { return *std::min_element(m_cost.begin(), m_cost.end()); }
        
    HydFloat
    max_cost( void ) const { return *std::max_element(m_cost.begin(), m_cost.end()); }
        
    HydFloat
    min_option( void ) const { return *std::min_element(m_value.begin(), m_value.end()); }
        
    HydFloat
    max_option( void ) const { return *std::max_element(m_value.begin(), m_value.end()); }
 
private:

    HydType m_hydType;
    std::string m_group;
    
    std::vector<HydFloat>   m_value;
    std::vector<HydFloat>   m_cost; 

};

#endif


