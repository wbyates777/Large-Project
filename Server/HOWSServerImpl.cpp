/* HOWSServerImpl 02/04/2021

 $$$$$$$$$$$$$$$$$$$$$$$$$$
 $   HOWSServerImpl.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 The HOWSServer main class 
 
 1) You can test the HOWSServer from the command line using 
 
 curl -i -X PUT -H 'Content-Type: application/json' -d '{"msgtype":"Load","name":"two_loop","randomise":1,"user":"bill"}' http://127.0.0.1:8000/HOWS
 
 curl -i -X PUT -H 'Content-Type: application/json' -d '{"msgtype":"Optimise", "seed":1, "iterations":1000,"user":"bill", "llhs":[], "obj_type":0, "obj_params":[1,1,2]}' http://127.0.0.1:8000/HOWS
 
 curl -i -X PUT -H 'Content-Type: application/json' -d '{"msgtype":"DB","name":"two_loop","cmd":0,"user":"bill"}' http://127.0.0.1:8000/HOWS
 
 see HOWSMessages.h for other message formats
 
 
 2) You can define alternative objective functions for optimisation
 
 For example, the function:
 
 ObjType 
 wdn_paper(const HHObjective& objs) 
 { 
     return (objs[0] * 200.0) + (1000.0 * objs[1]) + (objs[2] * 5.0);
 }
 
 HHObjFunc my_obj_func = wdn_paper;
  
 or
 
 HHObjFunc my_obj_func = HHFunc::param_sum({200.0, 1000.0, 5.0});
 
 is used in:
 
 "Yates W.B, Keedwell E.C. (2020) 
 Offline Learning with a Selection Hyper-Heuristic: An Application to Water Distribution Network Optimisation, 
 Evolutionary Computation, pages 1-24, 
 DOI:10.1162/evco_a_00277." 
  

 3) Dependencies
 
 C++17
 Wrapper code         - SWIG Version 4.0.2           http://www.swig.org
 Webserver            - Django 3.2 - Python 3.7      https://www.djangoproject.com
 Random Forest code   - RANGER_VERSION "0.12.4"      https://github.com/imbs-hl/ranger
 JSON for Modern C++  - version 3.9.1                https://json.nlohmann.me/ https://github.com/nlohmann/json
 Hydraulic simulation - EPANET2.2                    https://www.epa.gov/water-research/epanet
 Database             - MySQL 8.0.23                 https://www.mysql.com
 
*/


#ifndef __HOWSSERVERIMPL_H__
#include "HOWSServerImpl.h"
#endif

#ifndef __AFILEFINDER_H__
#include "AFileFinder.h"
#endif

#ifndef __HYDJSON_H__
#include "HydJSON.h"
#endif

#ifndef __HOWSPROBLEM_H__
#include "HOWSProblem.h"
#endif

#ifndef __SSHH_H__
#include "SSHH.h"
#endif

#ifndef __DBCONFIG_H__
#include "DBConfig.h"
#endif

#ifndef __HYDSQL_H__
#include "HydSQL.h"
#endif

typedef nlohmann::json Json;

//#include <thread>
//#include <mutex>

HOWSServerImpl::HOWSServerImpl( void ) 
{ 
    Singleton<AFileFinder>::initialise();
    
    // setup file paths to search when we look for a file
    // assumes HOWSServer is run in Django subdirectory
    // NB if you have two files with the same name in different directories
    // theFF will return the file it finds first (no duplicate file warnings are given)
    theFF->addFileDir("./HOWSData");
    theFF->addFileDir("./HOWSData/WDNetworks");
    theFF->addFileDir("./HOWSData/HDH"); 
    theFF->addFileDir("./HOWSData/HDH/HDHSessions");
    
    // setup logging - note logger level 0 is 'silent'
    std::string loggerfile = theFF->findFile("logSettings.txt");
    if (loggerfile.empty())
    {
        Logger::setLogLevel("HOWSServer", 2);
        Logger::setLogLevel("HOWSProblem", 2);
        Logger::setLogLevel("HydProblem", 2);
        Logger::setLogLevel("HydEPANET", 2);
        Logger::setLogLevel("SSHH", 2);
        Logger::setLogLevel("HMM", 2);
        Logger::setLogLevel("BW", 2);
        Logger::setLogLevel("HDHeuristic", 2);
        // does not have any logging messages but is included for future use
        Logger::setLogLevel("KBHeuristic", 2);
    }
    else 
    {
        Logger::loadLogLevels(loggerfile);
    }

    // get my log level
    m_logger.getLogLevel( "HOWSServer" );
    
    // try and open a single connection to the database
    std::string dbconfig_filename = "dbConfig.txt";
    std::string dbconfig_path = theFF->findFile(dbconfig_filename);
    
    if (dbconfig_path.empty())
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Cannot find database configuration file " << dbconfig_filename;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    else
    {
        DBConfig dbconfig;
        dbconfig.load( dbconfig_path );

        if (m_db.open( dbconfig ))
        {
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Connected to database " << dbconfig.dbname() << " as user " << dbconfig.user();
                m_logger.logMsg(txt);
                m_logger.flush();
            }
        }
        else 
        {
            if (m_logger.level() >= 1)
            {
                Message txt(1);
                txt << "Could not connect to database " << dbconfig.dbname() << " as user " << dbconfig.user();
                m_logger.logMsg(txt);
                m_logger.flush();
            }
        }
    }
    
    m_problem = new HOWSProblem;
    m_hydNet =  nullptr;
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Initialisation complete";
        m_logger.logMsg(txt);
        m_logger.flush();
    }
}

HOWSServerImpl::~HOWSServerImpl( void ) 
{
    if (m_problem)
    {
        delete m_problem;
        m_problem = nullptr;
    }

    if (m_hydNet) 
    {
        delete m_hydNet;
        m_hydNet = nullptr;
    }
    
    m_db.close();

    // destroy this last in case a class tries to find a file when destructing (it shouldn't)
    Singleton<AFileFinder>::destroy();
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Shutdown complete";
        m_logger.logMsg(txt);
        m_logger.flush();
    }

}

//
//
//

std::string 
HOWSServerImpl::process_request(std::string input)
{
    try
    {
        Json json = Json::parse(input);
        std::string msgtype = json.at("msgtype").get<std::string>();
        
        if (msgtype == _LOAD_)
        {
            LoadMessage msg = HydJSON::fromLoadMessageJSON(input);
            return load( msg );
        }
        else if (msgtype == _OPTIMISE_)
        {
            OptimiseMessage msg = HydJSON::fromOptimiseMessageJSON(input);
            return optimise( msg );
        }
        else if (msgtype == _RUN_)
        {
            RunMessage msg = HydJSON::fromRunMessageJSON(input);
            return run( msg );
        }
        else if (msgtype == _LOOKAHEAD_)
        {
            LookAheadMessage msg = HydJSON::fromLookAheadMessageJSON(input);
            return lookAhead( msg );
        }
        else if (msgtype == _CHANGE_)
        {
            ChangeMessage msg = HydJSON::fromChangeMessageJSON(input);
            return change( msg );
        }
        else if (msgtype == _DB_)
        {
            DBMessage msg = HydJSON::fromDBMessageJSON(input);
            return database( msg );
        }
        else 
        {
            return std::string("Unknown Message Type: " + input);
        }
    }
    catch (std::exception& e)
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Error: " << e.what();
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return std::string(e.what());
    }
    
    return std::string();

}

//
//
//


std::string
HOWSServerImpl::load( const LoadMessage& msg )
{
    if (m_logger.level() >= 1)
    {
        Message txt1(1);
        txt1 << "Loading " <<  msg.name();
        m_logger.logMsg(txt1);
        m_logger.flush();
    }
    
    // check the network and problem files exist
    std::string network_name = theFF->findFile(msg.name() + ".inp");
    std::string problem_name = theFF->findFile(msg.name() + ".prb");
    
    if (network_name.empty() || problem_name.empty())
    {
        if (m_logger.level() >= 1)
        {
            Message txt1(1);
            if (network_name.empty())
                txt1 << "Could not find network file for " <<  msg.name();
            else txt1 << "Could not find problem file for " <<  msg.name();
            
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        return std::string();
    }
     
    // delete any existing network before loading a problem  
    // loading a problem creates a new network
    if (m_hydNet) 
    {
        delete m_hydNet;
        m_hydNet = nullptr;
    }
    
    m_problem->load( network_name );
    
    m_networkName = msg.name();
    m_hydNet = m_problem->getHydNet();
    
    m_hydNet->setName(m_networkName);

    // setup an initial possibly random solution
    m_problem->initialiseSolution(m_current_solution);
    if (!msg.randomise())
       m_current_solution.assign( m_current_solution.size(), 0 );

    HHObjective res = m_problem->evalauteObj(m_current_solution);
    
    if (m_logger.level() >= 2)
    {
        Message txt1(2);
        txt1 << "Cost " << res[0] << ", max head deficit " << res[1] << ", min resilience " << -res[2];
        m_logger.logMsg(txt1);
        m_logger.flush();
    }

    return HydJSON::toNetworkJSON( m_hydNet );
}

std::string
HOWSServerImpl::run( const RunMessage& msg )
{
    if (m_hydNet)
    {
        if (m_logger.level() >= 1)
        {
            Message txt1(1);
            txt1 << "Running network " << m_networkName;
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        HHObjective res = m_problem->evalauteObj(m_current_solution);
        
        if (m_logger.level() >= 2)
        {
            Message txt1(2);
            txt1 << "Cost " << res[0] << ", max head deficit " << res[1] << ", min resilience " << -res[2];
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
      
        return HydJSON::toNetworkJSON( m_hydNet );
    }

    return std::string();
}

std::string
HOWSServerImpl::database( const DBMessage& msg )
{
    if (!m_db.isOpen())
    {
        if (m_logger.level() >= 2)
        {
            Message txt1(2);
            txt1 << "No database connection" << m_db.getErrorMsg();
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        return std::string();
    }
        
    // Note we use the current loaded m_networkName not msg.name()
    if (m_hydNet)
    {
        if (m_logger.level() >= 1)
        {
            Message txt1(1);
            if (msg.cmd() == DBMessage::SAVE)
                txt1 << "Database save for network " << m_networkName;
            else if (msg.cmd() == DBMessage::DELETE)
                txt1 << "Database delete for network " << m_networkName;
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        HydSQL mysql;
        std::vector<std::string> sql_stmts;
        
        if (msg.cmd() == DBMessage::SAVE)
        {
            sql_stmts = mysql.clear(msg.user(), m_networkName);
            std::vector<std::string> tmp_sql  = mysql.toSQL(msg.user(), m_hydNet);
            sql_stmts.insert( sql_stmts.end(), tmp_sql.begin(), tmp_sql.end());
        }
        else if (msg.cmd() == DBMessage::DELETE)
        {
            sql_stmts = mysql.clear(msg.user(), m_networkName);
        }
        
        int rowsAffected = 0;
        for (int i = 0; i < sql_stmts.size(); ++i)
        {
            if (m_db.sqlQuery( sql_stmts[i] ))
            {
                rowsAffected += m_db.rowsAffected();
            }
            else
            {
                if (m_logger.level() >= 1)
                {
                    Message txt1(1);
                    txt1 << "Database Error: " << m_db.getErrorMsg() << '\n';
                    txt1 << sql_stmts[i];
                    m_logger.logMsg(txt1);
                    m_logger.flush();
                }
            }
        }
        
        if (m_logger.level() >= 2)
        {
            Message txt1(2);
            txt1 << rowsAffected << " rows affected";
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
    }

    return std::string();
}

std::string
HOWSServerImpl::lookAhead( const LookAheadMessage& msg )
{
    if (m_hydNet)
    {
        if (m_logger.level() >= 1)
        {
            Message txt1(1);
            txt1 << "Running look ahead for pipe " << msg.pipeID() << " in network " << m_networkName;
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        HydPipe* pipe = m_hydNet->findPipe( msg.pipeID() );
        
        if (pipe)
        {
            HydLookAhead data;
            data.setSelectedPipeId( pipe->ID() );
            data.setCurrency("USD");
            data.setObjNames( {"Network Cost", "Total Head Deficit", "Network Resilience"} );
            data.setObjUnits( {"USD", "m", "I"} );
            data.setDiamUnits("mm");
            data.setObjMinimise({ true, true, false });
            
            const int numScenarios = (int) m_hydNet->timePoints().size();
            std::vector<HydObjectives> obj_cur(numScenarios);
            std::vector<std::vector<HydObjectives>> obj_values(numScenarios);
            
            if (pipe->isDecisionVariable())
            {
                const HydOptions& diams = m_hydNet->options( HydType::Pipe, pipe->group() );
                std::vector<HHSolution>   solutions(diams.values().size(), m_current_solution);
                const int numDiams = (int) diams.values().size();
                
                // only link components can be decision variables - otherwise comp->index() is not unique
                int idx = m_problem->dvar2spec()[pipe->index()];
                assert(idx != -1);
                int currentDiamIdx = m_current_solution[idx];

                std::vector<HydFloat> avail_diams;
                std::vector<HydFloat> avail_diams_cost;
                
                for (int i = 0; i < numDiams; ++i)
                {
                    avail_diams.push_back(diams.value(i));
                    avail_diams_cost.push_back(diams.cost(i) * pipe->length());
                    
                    // modify existing solution and evaluate
                    solutions[i][idx] = i;
                    
                    // this will clear the networks simulation and objective function values
                    m_problem->evalauteObj(solutions[i]);
            
                    for (int j = 0; j < numScenarios; ++j)
                    {
                        // WARNING: this is done in original code
                        // needed so the client draws its 'barchart' graphics correctly
                        HydFloat resilience = m_hydNet->resilience(j);
                        if (resilience < 0.0)
                            resilience = 0.0;
                        
                        HydObjectives results({m_hydNet->cost(j), m_hydNet->headDeficit(j), resilience});
                        
                        obj_values[j].push_back( results );
                        
                        if (currentDiamIdx == i)
                            obj_cur[j] = results;
                    }
                }
                
                if (m_logger.level() >= 2)
                {
                    Message txt1(2);
                    txt1 << solutions.size() << " evaluations performed ";
                    m_logger.logMsg(txt1);
                    m_logger.flush();
                }
                
                data.setSelectedDiamIdx( currentDiamIdx );
                data.setDiameters(avail_diams);
                data.setDiametersCost(avail_diams_cost);
            }
            else
            {
                m_problem->evalauteObj(m_current_solution);
                for (int j = 0; j < numScenarios; ++j)
                {
                    // WARNING: this is done in original code
                    // needed so the client draws its 'barchart' graphics correctly
                    HydFloat resilience =  m_hydNet->resilience(j);
                    if (resilience < 0.0)
                        resilience = 0.0;
                    
                    HydObjectives results({m_hydNet->cost(j), m_hydNet->headDeficit(j), resilience});
                    
                    obj_values[j].push_back( results );
                    obj_cur[j] = results;
                }
                
                data.setSelectedDiamIdx( -1 );
            }
  
            std::vector<HydObjectives> obj_min(numScenarios, HydObjectives(3, std::numeric_limits<HydFloat>::max()));
            std::vector<HydObjectives> obj_max(numScenarios, HydObjectives(3, -std::numeric_limits<HydFloat>::max()));
            
            // WARNING: the original code does this - but should we treat 
            // resilience (which is to be maximised) differently?
            for (int i = 0; i < numScenarios; ++i)
            {
                for (int j = 0; j < obj_values[i].size(); ++j)
                {
                    for (int k = 0; k < 3; ++k)
                    {
                        obj_min[i][k] = std::min(obj_min[i][k], obj_values[i][j][k]);
                        obj_max[i][k] = std::max(obj_max[i][k], obj_values[i][j][k]);
                    }
                }
            }
            
            data.setObjValues( obj_values );
            data.setObjValuesCurrent(obj_cur);
            data.setObjMin(obj_min);
            data.setObjMax(obj_max);
            
            return HydJSON::toLookAheadJSON( data );
        }
    }
    
    return std::string();
}

 
std::string
HOWSServerImpl::change( const ChangeMessage& msg )
{
    if (m_hydNet)
    {
        if (m_logger.level() >= 1)
        {
            Message txt1(1);
            txt1 << "Change network " << m_networkName;
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        // only link components can be decision variables - otherwise idx = comp->index() is not unique
        // and so m_problem->dvar2spec()[idx] will not work properly
        
        switch(msg.hydType())
        {
            case HydType::Pipe: 
                {
                    HydPipe* pipe = m_hydNet->findPipe( msg.ID() ); 
                    if (pipe && pipe->isDecisionVariable())
                    {
                        int idx = m_problem->dvar2spec()[pipe->index()];

                        if (m_logger.level() >= 2)
                        {
                            Message txt1(2);
                            txt1 << "Changed pipe " << msg.ID() << " diameter index from " <<  m_current_solution[idx] << " to " << msg.value() ;
                            m_logger.logMsg(txt1);
                            m_logger.flush();
                        }
                        
                        m_current_solution[idx] = (int) msg.value();
                    }
                }
                break;
            case HydType::Pump:      ;  break;
            case HydType::Valve:     ;  break;
            //case HydType::Tank:      ;  break;
            //case HydType::Reservoir: ;  break;
            //case HydType::Junction:  ;  break;
            default:
                if (m_logger.level() >= 1)
                {
                    Message txt1(1);
                    txt1 << "Error: Cannot change unknown component type";
                    m_logger.logMsg(txt1);
                    m_logger.flush();
                }
                break;
        }
        
        HHObjective res = m_problem->evalauteObj(m_current_solution);
        
        if (m_logger.level() >= 2)
        {
            Message txt1(2);
            txt1 << "Cost " << res[0] << ", max head deficit " << res[1] << ", min resilience " << -res[2];
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        return HydJSON::toNetworkJSON( m_hydNet );
    }
    
    return std::string();
}


std::string
HOWSServerImpl::optimise( const OptimiseMessage& msg )
{
    if (m_hydNet)
    {
        if (m_logger.level() >= 1)
        {
            Message txt1(1);
            txt1 << "Optimise network " << m_hydNet->name() << " for " << msg.iters() << " iterations with seed " << msg.seed();
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        m_problem->seed(msg.seed());

        // set up the objective function
        HHObjFunc my_objFunc;
        if (msg.objFuncType() == OptimiseMessage::VOL)
            my_objFunc = HHFunc::param_volume(msg.objFuncParams()); // default is {1.0, 1.0, 2.0}
        else my_objFunc = HHFunc::param_sum(msg.objFuncParams()); 

        std::vector<int> omitLLH = msg.llhs();
 
        // initalise the SSHH optimiser with the problem and objective function
        // you could add more config here - choose a cross over mechanism, learning rates, HMM setup
        SSHH sshh;
        sshh.seed(msg.seed());
        sshh.initialise(*m_problem, m_current_solution, my_objFunc, omitLLH); 

        // run the optimiser
        sshh.logProblem( msg.iters() );
        sshh.beginRunLog();
        sshh.solve( msg.iters() );
        sshh.endRunLog();

        m_current_solution = m_problem->getSolution(SSHH::BEST_SOL);
        
        HHObjective res = m_problem->evalauteObj(m_current_solution);
        
        if (m_logger.level() >= 2)
        {
            Message txt1(2);
            txt1 << "Optimised Cost " << res[0] << ", max head deficit " << res[1] << ", min resilience " << -res[2];
            m_logger.logMsg(txt1);
            m_logger.flush();
        }
        
        return HydJSON::toNetworkJSON( m_hydNet );
    }
    
    return std::string();
}
