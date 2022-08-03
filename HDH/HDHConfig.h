/* HDHConfig 17/03/2021

 $$$$$$$$$$$$$$$$$$$
 $   HDHConfig.h   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 Configuration for a Human Derived Heuristic (HDH)
 
 Specify the training sessions, parameters, and/or generated forest file name for an HDHeuristic

 TODO:  should contain the required input and output varaible names 
 TODO:  add proper names for params in JSON and class - like flow_params_min etc 

 
*/


#ifndef __HDHCONFIG_H__
#define __HDHCONFIG_H__

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#include "json.hpp"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <string>
#include <vector>

class HDHConfig
{
public:

    HDHConfig( void );
    ~HDHConfig( void );
    
    HDHConfig(const HDHConfig& c);
    
    HDHConfig&
    operator=(const HDHConfig& rhs);

    const std::string&
    name( void ) const { return m_name; }
    
    void
    name( const std::string& n ) { m_name = n; }
    
    const std::string&
    hdhType( void ) const { return m_hdhType; }
    
    void
    hdhType( const std::string& n ) { m_hdhType = n; }  
    
    const std::string&
    outputName( void ) const { return m_out_name; }
    
    void
    outputName( const std::string& n ) { m_out_name = n; }  
    
    void 
    inputNames( const std::vector<std::string>& data ) { m_in_names = data; }
    
    const std::vector<std::string>& 
    inputNames( void ) const { return m_in_names; }
    
    
    void 
    sessions( const std::vector<std::string>& data ) { m_sessions = data; }
    
    const std::vector<std::string>& 
    sessions( void ) const { return m_sessions; }
    
    void 
    params( const std::vector<std::vector<double>>& data ) { m_params = data; }
    
    const std::vector<std::vector<double>>& 
    params( void ) const { return m_params; }
    
    
    // max depth 0 - unlimited depth
    int
    maxDepth( void ) const { return m_maxDepth; }
    
    void
    maxDepth( int n ) { m_maxDepth = n; }  
    
    int
    numTrees( void ) const { return m_numTrees; }
    
    void
    numTrees( int n ) { m_numTrees = n; }    
    
    // num threads 0 - ranger::forest default
    int
    numThreads( void ) const { return m_numThreads; }
    
    void
    numThreads( int n ) { m_numThreads = n; }    
    
    void
    loadJSON( const std::string& file_name );
    
    void
    saveJSON( const std::string& file_name, int indent = 3);
    
private:

    int m_numTrees;
    int m_maxDepth;
    int m_numThreads;
    std::string m_name; 
    std::string m_hdhType; // classification or regression
    
    std::string m_out_name;
    std::vector<std::string> m_in_names;
    
    std::vector<std::string> m_sessions; // if empty HDHeuristic will try to load forest m_name
    std::vector<std::vector<double>> m_params;
    
    mutable Logger m_logger;
    
};


#endif


