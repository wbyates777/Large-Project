/* HydScenarios 19/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$
 $   HydScenarios.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A problem specification for a HydNetwork
 
*/


#ifndef __HYDSCENARIOS_H__
#include "HydScenarios.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <iostream>
#include <sstream>
#include <fstream>


// we use regex here in case the tokens become more complicated i.e upper/lower case...
std::regex HydScenarios::pipe_options_token(R"((\[PIPE_OPTIONS\]))");
std::regex HydScenarios::pipe_decision_token(R"((\[PIPE_DECISION\]))");
std::regex HydScenarios::pump_options_token(R"((\[PUMP_OPTIONS\]))");
std::regex HydScenarios::pump_decision_token(R"((\[PUMP_DECISION\]))");
std::regex HydScenarios::valve_options_token(R"((\[VALVE_OPTIONS\]))");
std::regex HydScenarios::valve_decision_token(R"((\[VALVE_DECISION\]))");
std::regex HydScenarios::tank_options_token(R"((\[TANK_OPTIONS\]))");
std::regex HydScenarios::tank_decision_token(R"((\[TANK_DECISION\]))");
std::regex HydScenarios::head_constraints_token(R"((\[HEAD_CONSTRAINTS\]))");
std::regex HydScenarios::pressure_constraints_token(R"((\[PRESSURE_CONSTRAINTS\]))");
std::regex HydScenarios::velocity_constraints_token(R"((\[VELOCITY_CONSTRAINTS\]))");
std::regex HydScenarios::tank_constraints_token(R"((\[TANK_LEVEL_CONSTRAINTS\]))");

std::vector<std::string>
HydScenarios::getTokens( const std::string& line )
{
    std::vector<std::string> tokens;
    std::string delimiter = " "; // we use white space as a delim
    
    // convert any tabs to space delim and detect comments
    std::string cleanline;
    for (int i = 0; i < line.size(); ++i)
    {
        if (line[i] == ';')  break;
        cleanline += (line[i] == '\t') ? ' ' : line[i];
    }
 
    if (cleanline.find(delimiter) != std::string::npos)
    {
        // we have delimiters - break up the line
        size_t pos = 0;
        std::string token;
        while ((pos = cleanline.find(delimiter)) != std::string::npos) 
        {
            token = cleanline.substr(0, pos);
            if (!token.empty())
            {
                tokens.push_back(util::trim(token));
            }
            cleanline.erase(0, pos + delimiter.length());
        }
    }

    if (!cleanline.empty())
        tokens.push_back(cleanline);
    
    return tokens;
}


void
HydScenarios::addOption(const std::string& group, HydFloat value, HydFloat cost, HydType option_type)
{
    std::string key = to_hydstring(option_type) + "_" + group;    
    auto fidx = m_options.find(key);
    
    if (fidx == m_options.end())
    {
        HydOptions options(group, option_type);
        options.addOption(value, cost);
        m_options.insert(std::map<std::string,HydOptions>::value_type(key, options));
    }
    else fidx->second.addOption(value, cost);
}

bool
HydScenarios::matchHeader( const std::string& token, Section& section)
{
    if (std::regex_match(token, pipe_options_token))
    {
        section = PIPE_OPTIONS;
        return true;
    }
    if (std::regex_match(token, pump_options_token))
    {
        section = PUMP_OPTIONS;
        return true;
    }
    if (std::regex_match(token, valve_options_token))
    {
        section = VALVE_OPTIONS;
        return true;
    }
    if (std::regex_match(token, tank_options_token))
    {
        section = TANK_OPTIONS;
        return true;
    }
    
    if (std::regex_match(token, pipe_decision_token))
    {
        section = PIPE_DECISION;
        return true;
    }
    if (std::regex_match(token, pump_decision_token))
    {
        section = PUMP_DECISION;
        return true;
    }
    if (std::regex_match(token, valve_decision_token))
    {
        section = VALVE_DECISION;
        return true;
    }
    if (std::regex_match(token, tank_decision_token))
    {
        section = TANK_DECISION;
        return true;
    }
   
    if (std::regex_match(token, head_constraints_token))
    {
        section = HEAD_CONSTRAINTS;
        return true;
    }
    if (std::regex_match(token, pressure_constraints_token))
    {
        section = PRESSURE_CONSTRAINTS;
        return true;
    }
    if (std::regex_match(token, velocity_constraints_token))
    {
        section = VELOCITY_CONSTRAINTS;
        return true;
    }
    if (std::regex_match(token, tank_constraints_token))
    {
        section = TANK_CONSTRAINTS;
        return true;
    }
    
    return false;
}

bool
HydScenarios::load( const std::string& file_name )
{    
    try
    {
        std::ifstream file;
        file.open( file_name );
        
        if (file)
        {
            // 0 = options, 1 = decisions, 2 = minHead
            Section section;
            std::vector<std::string> tokens;
            std::string line;
            
            while(std::getline(file, line))
            { 
                tokens = getTokens(line);
                
                for (int i = 0; i < tokens.size(); ++i)
                {
                    // have we started a new section of the .prb file
                    if (matchHeader(tokens[i], section))
                        break;
                    
                    
                    //
                    // Decisions
                    //
                    if (section == PIPE_DECISION && tokens.size() == 2)
                    {
                        // expecting id, group
                        m_decisions.push_back( HydDVariable(tokens[0], tokens[1], HydType::Pipe) );
                        break;
                    }
                    if (section == PUMP_DECISION && tokens.size() == 2)
                    {
                        // expecting id, group
                        m_decisions.push_back( HydDVariable(tokens[0], tokens[1], HydType::Pump) );
                        break;
                    }
                    if (section == VALVE_DECISION && tokens.size() == 2)
                    {
                        // expecting id, group
                        m_decisions.push_back( HydDVariable(tokens[0], tokens[1], HydType::Valve) );
                        break;
                    }
                    if (section == TANK_DECISION && tokens.size() == 2)
                    {
                        // expecting id, group
                        m_decisions.push_back( HydDVariable(tokens[0], tokens[1], HydType::Tank) );
                        break;
                    }
                    
                    //
                    // Options
                    //
                    
                    if (section == PIPE_OPTIONS && tokens.size() >= 3)
                    {
                        std::string group = tokens[0];
                        HydFloat diameter = static_cast<HydFloat>(std::stod(tokens[1]));
                        HydFloat cost     = static_cast<HydFloat>(std::stod(tokens[2]));
                        addOption(group, diameter, cost, HydType::Pipe);
                        break;
                    }
                    if (section == VALVE_OPTIONS && tokens.size() >= 3)
                    {
                        std::string group = tokens[0];
                        HydFloat status = (tokens[1] == "Open") ? 1 : 0;
                        HydFloat cost = std::stod(tokens[2]);
                        addOption(group, status, cost, HydType::Valve);
                        break;
                    }
                    if (section == PUMP_OPTIONS && tokens.size() >= 3)
                    {
                        //std::string group = tokens[0];
                       // HydFloat curve    = tokens[1];
                        //HydFloat cost     = std::stod(tokens[2]);
                        //addOption(group, curve, cost, HydType::Pump);
                        break;
                    }
                    if (section == TANK_OPTIONS && tokens.size() >= 3)
                    {
                        std::string group = tokens[0];
                        HydFloat diameter   = std::stod(tokens[1]);
                        HydFloat cost     = std::stod(tokens[2]);
                        addOption(group, diameter, cost, HydType::Tank);
                        break;
                    }
                    
                    //
                    // Constraints
                    //
                    
                    if (section == HEAD_CONSTRAINTS && tokens.size() >= 2)
                    {
                        std::vector<HydFloat> values;
                        for (int j = 1; j < tokens.size(); ++j)
                            values.push_back( std::stod(tokens[j]) );
                        
                        m_constraints.push_back( HydConstraint(tokens[0], values) );
                        break;
                    }
                    if (section == PRESSURE_CONSTRAINTS && tokens.size() >= 2)
                    {
                        break;
                    }
                    if (section == VELOCITY_CONSTRAINTS && tokens.size() >= 2)
                    {
                        break;
                    }
                    if (section == TANK_CONSTRAINTS && tokens.size() >= 2)
                    {
                        break;
                    }
                }
                
                tokens.clear();
            }
             
            return true;
        }
    }
    catch( std::exception &e )
    {
        std::cout  << "Failed to load \"" << file_name << "\" : "<< e.what() << std::endl;
    }
    
    return false;
}
