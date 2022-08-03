/* HydScenarios 19/02/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydScenarios.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Specify the decsion variables, their possible values (options) and the constraints
 that apply to a particular network over a number of time periods (or scenarios)
 
 
 This class is intended to be loaded/saved  from/to a file
 and is used to setup HydProblem
 
 TODO: not finished! - does not load all constraint types see .cpp for details
 
*/


#ifndef __HYDSCENARIOS_H__
#define __HYDSCENARIOS_H__


#include <string>
#include <vector>
#include <map>
#include <regex>

#ifndef __HYDDVARIABLE_H__
#include "HydDVariable.h"
#endif

#ifndef __HYDOPTIONS_H__
#include "HydOptions.h"
#endif

#ifndef __HYDCONSTRAINT_H__
#include "HydConstraint.h"
#endif



class HydScenarios
{
public:
    
    enum Section  { PIPE_OPTIONS, PUMP_OPTIONS, VALVE_OPTIONS, TANK_OPTIONS,
                    PIPE_DECISION, PUMP_DECISION, VALVE_DECISION, TANK_DECISION, 
                    HEAD_CONSTRAINTS, PRESSURE_CONSTRAINTS, VELOCITY_CONSTRAINTS, TANK_CONSTRAINTS };
    

    HydScenarios( void )=default;
    
    HydScenarios( const std::map<std::string,HydOptions>& o,
                  const std::vector<HydDVariable>& d,
                  const std::vector<HydConstraint>& c): m_options(o), m_decisions(d), m_constraints(c)  {}
    
    ~HydScenarios( void )=default;

    // load the EPANET (like) problem file .prb
    bool
    load( const std::string& file_name );

    
    const std::map<std::string,HydOptions>&      
    options(void) const { return m_options; }
    
    std::map<std::string, HydOptions>&      
    options(void) { return m_options; }
    
    
    const std::vector<HydDVariable>&    
    decisions(void) const { return m_decisions; }
    
    std::vector<HydDVariable>&    
    decisions(void) { return m_decisions; }
    
    
    const std::vector<HydConstraint>& 
    constraints(void) const { return m_constraints; }
    
    std::vector<HydConstraint>& 
    constraints(void) { return m_constraints; }

    
private:

    std::vector<std::string>
    getTokens( const std::string& line );
    
    void
    addOption(const std::string& group, HydFloat value, HydFloat cost, HydType option_type);
    
    bool
    matchHeader( const std::string& token, Section& section);
    
    std::map<std::string, HydOptions> m_options;
    std::vector<HydDVariable> m_decisions;
    
    std::vector<HydConstraint> m_constraints;
    
    static std::regex pipe_options_token;
    static std::regex pipe_decision_token;
    static std::regex pump_options_token;
    static std::regex pump_decision_token;    
    static std::regex valve_options_token;
    static std::regex valve_decision_token;
    static std::regex tank_options_token;
    static std::regex tank_decision_token;
    static std::regex head_constraints_token;
    static std::regex pressure_constraints_token;
    static std::regex velocity_constraints_token;
    static std::regex tank_constraints_token;
};





#endif


