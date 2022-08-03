/* HydSession 11/03/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydSession.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HYDSESSION_H__
#include "HydSession.h"
#endif

#include <iostream>
#include <fstream>

typedef nlohmann::json Json;

void 
from_json(const Json& json, HydPipeSpec& comp)
{
    std::string hid = json.at("id").get<std::string>();
    comp.setID(hid);
    
    HydFloat diameter = json.at("diameter").get<HydFloat>();
    comp.setDiameter(diameter);
}


void 
to_json(Json& json, const HydPipeSpec& comp) 
{
    json["id"]         = comp.ID();
    json["diameter"]   = comp.diameter();
}


void 
from_json(const Json& json, HydInteraction& comp)
{
    std::string id = json.at("sessionid").get<std::string>();
    comp.setID(id);
    
    std::string date = json.at("timedate").get<std::string>();
    comp.setDate(date);
    
    std::string name = json.at("filename").get<std::string>();
    comp.setNetworkName(name);
    
    std::vector<HydPipeSpec> pipe_data = json.at("pipes").get<std::vector<HydPipeSpec>>(); 
    comp.setPipeData(pipe_data);

}


void 
to_json(Json& json, const HydInteraction& comp) 
{
    json["sessionid"]  = comp.ID();
    json["timedate"]   = comp.date();
    json["filename"]   = comp.networkName();
    json["pipes"]      = comp.pipeData();
}
//
//
//

HydSession::HydSession( void ) :m_sessionID(), m_date(), m_networkName(), m_interactions()
{ 
    m_logger.getLogLevel( "HDHeuristic" ); 
}

void
HydSession::loadJSON( const std::string& file_name )
{
    try
    {
        std::ifstream file;
        file.exceptions( std::ifstream::badbit | std::ifstream::failbit );
        file.open( file_name );
        
        Json json;
        json = Json::parse(file);
       
        m_interactions = json.at("data").get<std::vector<HydInteraction>>();
        
        if (!m_interactions.empty())
        {
            m_sessionID   = m_interactions[0].ID();
            m_date        = m_interactions[0].date();
            m_networkName = m_interactions[0].networkName();
        }
        
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Loaded session " << file_name << " with " << m_interactions.size() << " interactions";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    catch( const std::ifstream::failure e )
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: Failed to load session " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    catch( const nlohmann::detail::parse_error e )
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: Exception - " << e.what() << " for session " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }    
}

void
HydSession::saveJSON( const std::string& file_name, int indent )
{
    try
    {
        std::ofstream file;
        file.exceptions( std::ofstream::badbit | std::ofstream::failbit );
        file.open( file_name );
        
        Json json_session;
        
        json_session["filename"]   = ID();
        json_session["sessionid"]  = networkName();
        json_session["timedate"]   = date();
        json_session["pipes"]      = m_interactions;
        
        file << json_session.dump(indent);
        file.close();
        
        if (m_logger.level() >= 2)
        {
            Message txt(1);
            txt << "Saved session " << file_name << " with " << m_interactions.size() << " interactions";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    catch( const std::ofstream::failure e )
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: Failed to save session " << file_name;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
}

