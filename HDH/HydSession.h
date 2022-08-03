/* HydSession 11/03/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydSession.h   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A record of an interactive session - the updates are pipes in a network 
 
 TODO: The interactions in a session should be sorted by time (earliest first) - we rely on order in file
*/


#ifndef __HYDSESSION_H__
#define __HYDSESSION_H__

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#include "json.hpp"
#endif


#include <string>
#include <vector>
#include <map>

class HydPipeSpec
{
public:
    
    HydPipeSpec( void ):m_id(), m_diameter(0.0) {}
    HydPipeSpec( const std::string& ID, HydFloat d ):m_id(ID), m_diameter(d) {}
    
    std::string 
    ID(void) const { return m_id; }
    
    void 
    setID( const std::string& ID ) { m_id = ID; }
    
    HydFloat 
    diameter( void ) const { return m_diameter; }
    
    void 
    setDiameter( HydFloat d ) { m_diameter = d; }
    
private:
    
    std::string m_id;
    HydFloat m_diameter;
    
};

class HydInteraction
{
public:
    
    const std::string& 
    ID( void ) const { return m_sessionID; }
    
    void 
    setID( const std::string& sid ) { m_sessionID = sid; }
    
    const std::string& 
    date( void ) const { return m_date; }
    
    void 
    setDate( const std::string& sid ) { m_date = sid; }
    
    const std::string& 
    networkName( void ) const { return m_networkName; }
    
    void 
    setNetworkName( const std::string& name ) { m_networkName = name; }
    
    void 
    setPipeData( const std::vector<HydPipeSpec>& data ) { m_pipeData = data; }
    
    const std::vector<HydPipeSpec>& 
    pipeData( void ) const { return m_pipeData; }
    
    int
    size(void) const { return m_pipeData.size(); }
    
    const HydPipeSpec&
    operator[](int i) const { return m_pipeData[i]; }
    
    
private:
    
    std::string m_sessionID;
    std::string m_date;
    std::string m_networkName;
    
    std::vector<HydPipeSpec> m_pipeData;
    
};


class HydSession
{
public:

    HydSession( void );
    ~HydSession( void ) {}

    const std::string& 
    ID( void ) const { return m_sessionID; }

    
    const std::string& 
    date( void ) const { return m_date; }
    

    const std::string& 
    networkName( void ) const { return m_networkName; }
    
    const std::vector<HydInteraction>&
    interactions( void ) const { return m_interactions; }
                 
 
    void
    loadJSON( const std::string& file_name );
    
    void
    saveJSON( const std::string& file_name, int indent );
    
private:

    HydSession( const HydSession& )=delete;

    HydSession&
    operator=( const HydSession& )=delete;
    
    std::string m_sessionID;
    std::string m_date;
    std::string m_networkName;
    
    std::vector<HydInteraction> m_interactions;
    
    mutable Logger m_logger;

};

#endif


