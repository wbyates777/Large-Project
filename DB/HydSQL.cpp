/* HydSQL 31/05/2021

 $$$$$$$$$$$$$$$$$$
 $   HydSQL.cpp   $
 $$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HYDSQL_H__
#include "HydSQL.h"
#endif

std::string
HydSQL::pumpSQL(HydSQL* data, const HydPump& comp) 
{
    std::ostringstream insert_stmt;
    
    insert_stmt << "(";
    insert_stmt << toQuoted(data->m_user) << ',';
    insert_stmt << toQuoted(data->m_networkName) << ',';
    insert_stmt << toQuoted(comp.ID()) << ',';
    insert_stmt << toQuoted(comp.group()) << ',';
    insert_stmt << toQuoted(comp.fromNodeID()) << ',';
    insert_stmt << toQuoted(comp.toNodeID()) << ',';
    insert_stmt << comp.power() << ',';
    insert_stmt << comp.speed() << ',';
    insert_stmt << comp.energyPrice() << ',';
    insert_stmt << comp.headloss() << ',';
    insert_stmt << toQuoted(comp.pumpCurveID()) << ',';
    insert_stmt << toQuoted(comp.efficCurveID()) << ',';
    insert_stmt << toQuoted(comp.patternID()) << ',';
    insert_stmt << toQuoted(comp.pricePatternID()) << ',';
    insert_stmt << toQuoted(to_hydstring(comp.initialStatus()));
    insert_stmt << ")";
    
    return insert_stmt.str();
}

std::string
HydSQL::pumpValuesSQL(HydSQL* data, const HydPump& comp)
{
    std::ostringstream insert_stmt;
    
    for (int i = 0; i < comp.flow().size(); ++i) 
    {
        insert_stmt << "(";
        insert_stmt << toQuoted(data->m_user) << ',';
        insert_stmt << toQuoted(data->m_networkName) << ',';
        insert_stmt << toQuoted(comp.ID()) << ',';
        insert_stmt << i << ',';
        insert_stmt << comp.flow()[i] << ',';
        insert_stmt << comp.quality()[i]  << ',';
        insert_stmt << toQuoted(to_hydstring(comp.status()[i]));
        insert_stmt << ")";
        
        if (i < comp.flow().size() -1) 
            insert_stmt << ',';
    }
    
    return insert_stmt.str();
}


std::string
HydSQL::valveSQL(HydSQL* data, const HydValve& comp) 
{
    std::ostringstream insert_stmt;
    
    insert_stmt << "(";
    insert_stmt << toQuoted(data->m_user) << ',';
    insert_stmt << toQuoted(data->m_networkName) << ',';
    insert_stmt << toQuoted(comp.ID()) << ',';
    insert_stmt << toQuoted(comp.group()) << ',';
    insert_stmt << toQuoted(comp.fromNodeID()) << ',';
    insert_stmt << toQuoted(comp.toNodeID()) << ',';
    insert_stmt << comp.diameter() << ',';
    insert_stmt << toQuoted(to_hydstring(comp.type())) << ',';
    insert_stmt << comp.headloss() << ',';
    insert_stmt << comp.setting() << ',';
    insert_stmt << toQuoted(to_hydstring(comp.fixedStatus()));
    insert_stmt << ")";
    
    return insert_stmt.str();
}  

std::string
HydSQL::valveValuesSQL(HydSQL* data, const HydValve& comp)
{
    std::ostringstream insert_stmt;
    
    for (int i = 0; i < comp.flow().size(); ++i)
    {
        insert_stmt << "(";
        insert_stmt << toQuoted(data->m_user) << ',';
        insert_stmt << toQuoted(data->m_networkName) << ',';
        insert_stmt << toQuoted(comp.ID()) << ',';
        insert_stmt << i << ',';
        insert_stmt << comp.flow()[i] << ',';
        insert_stmt << comp.velocity()[i] << ',';
        insert_stmt << comp.quality()[i]  << ',';
        insert_stmt << toQuoted(to_hydstring(comp.status()[i]));
        insert_stmt << ")";
        
        if (i < comp.flow().size() -1) 
            insert_stmt << ',';
    }
    
    return insert_stmt.str();
}


std::string
HydSQL::pipeSQL(HydSQL* data, const HydPipe& comp) 
{
    std::ostringstream insert_stmt;
    
    insert_stmt << "(";
    insert_stmt << toQuoted(data->m_user) << ',';
    insert_stmt << toQuoted(data->m_networkName) << ',';
    insert_stmt << toQuoted(comp.ID()) << ',';
    insert_stmt << toQuoted(comp.group()) << ',';
    insert_stmt << toQuoted(comp.fromNodeID()) << ',';
    insert_stmt << toQuoted(comp.toNodeID()) << ',';
    insert_stmt << comp.diameter() << ',';
    insert_stmt << comp.length() << ',';
    insert_stmt << comp.roughness() << ',';
    insert_stmt << toQuoted(to_hydstring(comp.initialStatus()));
    insert_stmt << ")";
    
    return insert_stmt.str();
}

std::string
HydSQL::pipeValuesSQL(HydSQL* data, const HydPipe& comp)
{
    std::ostringstream insert_stmt;
    
    for (int i = 0; i < comp.flow().size(); ++i)
    {
        insert_stmt << "(";
        insert_stmt << toQuoted(data->m_user) << ',';
        insert_stmt << toQuoted(data->m_networkName) << ',';
        insert_stmt << toQuoted(comp.ID()) << ',';
        insert_stmt << i << ',';
        insert_stmt << comp.flow()[i] << ',';
        insert_stmt << comp.velocity()[i] << ',';
        insert_stmt << comp.frictionFactor()[i] << ',';
        insert_stmt << comp.reactionRate()[i] << ',';
        insert_stmt << comp.quality()[i]  << ',';
        insert_stmt << toQuoted(to_hydstring(comp.status()[i]));
        insert_stmt << ")";
        
        if (i < comp.flow().size() -1) 
            insert_stmt << ',';
    }
    
    return insert_stmt.str();
}


std::string
HydSQL::junctionSQL(HydSQL* data, const HydJunction& comp) 
{
    std::ostringstream insert_stmt;
    
    insert_stmt << "(";
    insert_stmt << toQuoted(data->m_user) << ',';
    insert_stmt << toQuoted(data->m_networkName) << ',';
    insert_stmt << toQuoted(comp.ID()) << ',';
    insert_stmt << toQuoted(comp.group()) << ',';
    insert_stmt << comp.baseElevation() << ',';
    insert_stmt << comp.baseDemand();
    insert_stmt << ")";
    
    return insert_stmt.str();
}

std::string
HydSQL::junctionValuesSQL(HydSQL* data, const HydJunction& comp)
{
    std::ostringstream insert_stmt;
    
    for (int i = 0; i < comp.totalHead().size(); ++i)
    {
        insert_stmt << "(";
        insert_stmt << toQuoted(data->m_user) << ',';
        insert_stmt << toQuoted(data->m_networkName) << ',';
        insert_stmt << toQuoted(comp.ID()) << ',';
        insert_stmt << i << ',';
        insert_stmt << comp.totalHead()[i] << ',';
        insert_stmt << comp.headDeficit()[i] << ',';
        insert_stmt << comp.headExcess()[i] << ',';
        insert_stmt << comp.minHead()[i] << ',';
        insert_stmt << comp.pressure()[i]  << ',';
        insert_stmt << comp.actualDemand()[i]  << ',';
        insert_stmt << comp.quality()[i];
        insert_stmt << ")";
        
        if (i < comp.totalHead().size() -1) 
            insert_stmt << ',';
    }
    
    return insert_stmt.str();
}

std::string
HydSQL::reservoirSQL(HydSQL* data, const HydReservoir& comp) 
{
    std::ostringstream insert_stmt;
    
    insert_stmt << "(";
    insert_stmt << toQuoted(data->m_user) << ',';
    insert_stmt << toQuoted(data->m_networkName) << ',';
    insert_stmt << toQuoted(comp.ID()) << ',';
    insert_stmt << toQuoted(comp.group()) << ',';
    insert_stmt << comp.totalHead() << ',';
    insert_stmt << comp.elevation();
    insert_stmt << ")";
    
    return insert_stmt.str();
}

std::string
HydSQL::reservoirValuesSQL(HydSQL* data, const HydReservoir& comp)
{
    std::ostringstream insert_stmt;
    
    for (int i = 0; i < comp.netInflow().size(); ++i)
    {
        insert_stmt << "(";
        insert_stmt << toQuoted(data->m_user) << ',';
        insert_stmt << toQuoted(data->m_networkName) << ',';
        insert_stmt << toQuoted(comp.ID()) << ',';
        insert_stmt << i << ',';
        insert_stmt << comp.netInflow()[i] << ',';
        insert_stmt << comp.pressure()[i] << ',';
        insert_stmt << comp.quality()[i];
        insert_stmt << ")";
        
        if (i < comp.netInflow().size() -1) 
            insert_stmt << ',';
    }
    
    return insert_stmt.str();
}


//
// public methods
//

std::vector<std::string>
HydSQL::clear( const std::string& userName, const std::string& networkName )
{
    std::string whereClause;
    
    whereClause = "where network = " + toQuoted(networkName) + " and user = " + toQuoted(userName) + ";";
    
    std::vector<std::string> retVal = 
    {
        "delete from pipe " + whereClause,
        "delete from pipe_value " + whereClause, 
        
        "delete from junction " + whereClause,
        "delete from junction_value " + whereClause,
        
        "delete from pump " + whereClause,
        "delete from pump_value " + whereClause,
        
        "delete from valve " + whereClause,
        "delete from valve_value " + whereClause,
        
        "delete from reservoir " + whereClause,
        "delete from reservoir_value " + whereClause,
    };
    
    return retVal;
}

std::vector<std::string> 
HydSQL::toSQL(const std::string& userName, HydNetwork* hydNet)
{
    m_networkName = hydNet->name();
    m_user = userName;
    
    std::vector<std::string> retVal;
    std::vector<std::string> insert_stmt;
    
    insert_stmt = toSQL<HydPipe>(hydNet->pipes(), "pipe", pipeSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    insert_stmt = toSQL<HydPipe>(hydNet->pipes(), "pipe_value", pipeValuesSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    insert_stmt = toSQL<HydValve>(hydNet->valves(), "valve", valveSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    insert_stmt = toSQL<HydValve>(hydNet->valves(), "valve_value", valveValuesSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    insert_stmt = toSQL<HydPump>(hydNet->pumps(), "pump", pumpSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    insert_stmt = toSQL<HydPump>(hydNet->pumps(), "pump_value", pumpValuesSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    //
    
    insert_stmt = toSQL<HydJunction>(hydNet->junctions(), "junction", junctionSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());

    insert_stmt = toSQL<HydJunction>(hydNet->junctions(), "junction_value", junctionValuesSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());
    
    insert_stmt = toSQL<HydReservoir>(hydNet->reservoirs(), "reservoir", reservoirSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end());

    insert_stmt = toSQL<HydReservoir>(hydNet->reservoirs(), "reservoir_value", reservoirValuesSQL);
    retVal.insert(retVal.end(), insert_stmt.begin(), insert_stmt.end()); 
    
    return retVal;
}
