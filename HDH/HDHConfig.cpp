/* HDHConfig 17/03/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   HDHConfig.cpp   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HDHCONFIG_H__
#include "HDHConfig.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <fstream>

typedef nlohmann::json Json;


//
//
//

void 
from_json(const Json& json, HDHConfig& comp)
{
    comp.name(json.at("name").get<std::string>());
    comp.hdhType(json.at("hdhType").get<std::string>());
    comp.sessions(json.at("sessions").get<std::vector<std::string>>());
    comp.params(json.at("params").get<std::vector<std::vector<double>>>());
    
    comp.outputName(json.at("output_name").get<std::string>());
    comp.inputNames(json.at("input_names").get<std::vector<std::string>>());
    
    comp.numTrees(json.at("num_trees").get<int>());
    comp.maxDepth(json.at("max_depth").get<int>());
    comp.numThreads(json.at("num_threads").get<int>());
}


void 
to_json(Json& json, const HDHConfig& comp) 
{
    json["name"]      = comp.name();
    json["hdhType"]   = comp.hdhType();
    json["sessions"]  = comp.sessions();
    json["params"]    = comp.params();
    
    json["output_name"]    = comp.outputName();
    json["input_names"]    = comp.inputNames();
    
    json["num_trees"]    = comp.numTrees();
    json["max_depth"]    = comp.maxDepth();
    json["num_threads"]    = comp.numThreads();
}

//
//
//

HDHConfig::HDHConfig( void ) : m_numTrees(10), m_maxDepth(6), m_numThreads(0),
                               m_name("default"), m_hdhType(), 
                               m_out_name(), m_in_names(), m_sessions(), m_params() 
{
    m_logger.getLogLevel( "HDHeuristic" );
} 

HDHConfig::~HDHConfig( void ) {};

HDHConfig::HDHConfig(const HDHConfig& c) : m_numTrees(c.m_numTrees), 
                                           m_maxDepth(c.m_maxDepth),
                                           m_numThreads(c.m_numThreads),
                                           m_name(c.m_name), 
                                           m_hdhType(c.m_hdhType), 
                                           m_out_name(c.m_out_name), 
                                           m_in_names(c.m_in_names),
                                           m_sessions(c.m_sessions), 
                                           m_params(c.m_params) 
{ 
    m_logger.getLogLevel( "HDHeuristic" ); 
}

HDHConfig&
HDHConfig::operator=(const HDHConfig& rhs)
{
    if (this == &rhs)
        return  *this;
    
    m_name     = rhs.m_name; 
    m_hdhType  = rhs.m_hdhType; 
    m_sessions = rhs.m_sessions; 
    m_params   = rhs.m_params;
    
    m_out_name = rhs.m_out_name;
    m_in_names = rhs.m_in_names;
    
    m_numTrees   = rhs.m_numTrees;
    m_maxDepth   = rhs.m_maxDepth;
    m_numThreads = rhs.m_numThreads;
    
    return  *this;
}

void
HDHConfig::loadJSON( const std::string& file_name )
{
    try
    {
        std::ifstream file;
        file.exceptions( std::ifstream::badbit | std::ifstream::failbit );
        file.open( file_name );
        
        Json json;
        json = Json::parse(file);
       
        *this = json;
        
        if (m_logger.level() >= 2)
        {
            Message txt(1);
            txt << "Loaded HDH configuration " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    catch( const std::ifstream::failure e )
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: Failed to load " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    catch( const nlohmann::detail::parse_error e )
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: Exception - " << e.what();
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }    
}

void
HDHConfig::saveJSON( const std::string& file_name, int indent )
{
    try
    {
        std::ofstream file;
        file.exceptions( std::ofstream::badbit | std::ofstream::failbit );
        file.open( file_name );
        
        Json json;
        
        json = *this;

        file << json.dump(indent);
        file.close();
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Saved " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    catch( const std::ofstream::failure e )
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: Failed to save " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
}



//




