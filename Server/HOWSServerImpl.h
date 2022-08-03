/* HOWSServerImpl 02/04/2021

 $$$$$$$$$$$$$$$$$$$$$$$$
 $   HOWSServerImpl.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 The HOWSServer main class 
 
 You can test the HOWSServer from the command line using 
 
 curl -i -X PUT -H 'Content-Type: application/json' -d '{"msgtype":"Load","name":"two_loop","randomise":1,"user":"bill"}' http://127.0.0.1:8000/HOWS
 
 curl -i -X PUT -H 'Content-Type: application/json' -d '{"msgtype":"Optimise", "seed":1, "iterations":1000,"user":"bill", "llhs":[], "obj_type":0, "obj_params":[1,1,2]}' http://127.0.0.1:8000/HOWS
 
 curl -i -X PUT -H 'Content-Type: application/json' -d '{"msgtype":"DB","name":"two_loop","cmd":0,"user":"bill"}' http://127.0.0.1:8000/HOWS
 
 see HOWSMessages.h for other message formats
 
 
 
 Dependencies
 
 C++17
 Wrapper code         - SWIG Version 4.0.2           http://www.swig.org
 Webserver            - Django 3.2 - Python 3.7      https://www.djangoproject.com
 Random Forest code   - RANGER_VERSION "0.12.4"      https://github.com/imbs-hl/ranger
 JSON for Modern C++  - version 3.9.1                https://json.nlohmann.me/ https://github.com/nlohmann/json
 Hydraulic simulation - EPANET2.2                    https://www.epa.gov/water-research/epanet
 Database             - MySQL 8.0.23                 https://www.mysql.com
 
*/


#ifndef __HOWSSERVERIMPL_H__
#define __HOWSSERVERIMPL_H__

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif 

#ifndef __HHTYPES_H__
#include "HHTypes.h"
#endif

#ifndef __HOWSMESSAGES_H__
#include "HOWSMessages.h"
#endif

#ifndef __DBCONNECTION_H__
#include "DBConnection.h"
#endif

#include <string>
#include <vector>
#include <map>

class HOWSProblem;
class HydNetwork;

class HOWSServerImpl
{
public:

    HOWSServerImpl( void );
    ~HOWSServerImpl( void );
    
    std::string 
    process_request( std::string input );
    
private:

    HOWSServerImpl( const HOWSServerImpl& )=delete;

    HOWSServerImpl&
    operator=( const HOWSServerImpl& )=delete;

    
    
    std::string
    load( const LoadMessage& msg );
    
    std::string
    run( const RunMessage& msg );
    
    std::string
    lookAhead( const LookAheadMessage& msg );
    
    std::string
    optimise( const OptimiseMessage& msg );
    
    std::string
    change( const ChangeMessage& msg );
    
    std::string
    database( const DBMessage& msg );
    
    HydNetwork*  m_hydNet;
    HOWSProblem* m_problem; 
    HHSolution   m_current_solution;
    DBConnection m_db;
    std::string  m_networkName;
    mutable Logger m_logger;
    
};

#endif


