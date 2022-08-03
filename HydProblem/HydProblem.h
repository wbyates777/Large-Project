/* HydProblem 18/03/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydProblem.h   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class loads HydScenarios which defines the optimisation problem
 that is options, constrains, and decision  variable definitions
 
 The class is complicated because KBH works at an EPANET level
 while HDH works at the HydNetwork level - it would be better if both worked with HydNetwork
 
 TODO: Instead of using EPANET indexes which are only unique for links or nodes - but not both
 TODO: rewrite using HydNetComp::pos() which is unique across all components
 TODO: see comments in HOWSProblem.h
*/


#ifndef __HYDPROBLEM_H__
#define __HYDPROBLEM_H__


#ifndef __HYDDVARIABLE_H__
#include "HydDVariable.h"
#endif

#ifndef __HYDOPTIONS_H__
#include "HydOptions.h"
#endif

#ifndef __HYDCONSTRAINT_H__
#include "HydConstraint.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <cmath>
#include <string>
#include <vector>
#include <map>

class HydEPANET;
class HydNetwork;

class HydProblem
{
public:

    HydProblem( void );
    ~HydProblem( void );

    void
    init( HydEPANET* epanet ) { m_epanet = epanet; }
    
    HydEPANET*
    getEPANET(void) const { return m_epanet; }
    
    void 
    load( const std::string& fileName );
    
    HydNetwork* 
    getHydNet(void) const { return m_hydNet; }
    
    
    const std::vector<HydDVariable>&
    dvariables(void) const { return m_dvariables;  }        /// applies to link components could be pipes
    
    const std::vector<HydOptions>&                          /// could be diameters
    options(void) const { return  m_options; }
    
    const std::vector<HydConstraint>&                       /// applies to nodes components
    constraints(void) const { return m_constraints; } 
    
    const std::vector<std::vector<int>>                     /// decision variable link index for each component type
    compTypeIdx(void) const { return m_compTypeIdx; } 
    
    const std::vector<int>&                                 /// index for each type of component present
    compType(void) const { return m_compType; }
    
    const std::vector<int>&                                 /// map an EPANET link index to a solution index (where possible)
    dvar2spec(void) const { return m_dvar2spec; } 
    
    const std::vector<int>&                                  /// map an EPANET node index to a constraint (where possible)
    cons2spec(void) const { return m_cons2spec; }
    
private:

    HydProblem( const HydProblem& )=delete;

    HydProblem&
    operator=( const HydProblem& )=delete;
    
    void
    calcStaticValues( void );
    
    
    HydEPANET* m_epanet;
    HydNetwork* m_hydNet;

    std::vector<HydDVariable>  m_dvariables;          /// could be pipes
    std::vector<HydOptions>    m_options;             /// could be diameters
    std::vector<HydConstraint> m_constraints;         /// usually apply to nodes
    
    std::vector<std::vector<int>> m_compTypeIdx;    /// decision variable index for each component type
    std::vector<int> m_compType;                    /// index for each type of component present
    
    std::vector<int> m_dvar2spec;                   /// map an EPANET index to a solution index (where possible)
    std::vector<int> m_cons2spec;                   /// map an EPANET index to a constraint (where possible)

    mutable Logger m_logger;
};

#endif


