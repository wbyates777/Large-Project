/* HydJSON 17/02/2021

 $$$$$$$$$$$$$$$$$$$
 $   HydJSON.cpp   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 Convert a HydNetwork to and from JSON - we can also save/load as JSON

*/


#include <fstream>
#include <sstream>


#ifndef __HYDJSON_H__
#include "HydJSON.h"
#endif

#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

#ifndef __HHTYPES_H__
#include "HHTypes.h"
#endif

typedef nlohmann::json Json;


//
// these functions are called by the json parser automatically - they must be defined first
// Ive tried declaring them as static members but to no avail
//


//
// Client/Server Messages
//

void 
to_json(Json& json, const LoadMessage& msg)
{
    json["user"] = msg.user();
    json["name"] = msg.name();
    json["randomise"] = msg.randomise();
    json["msgtype"] = msg.msgType();
}

void 
from_json(const Json& json, LoadMessage& msg)
{
    msg.user(json.at("user").get<std::string>());
    msg.name(json.at("name").get<std::string>());
    msg.randomise(json.at("randomise").get<int>());
    msg.msgType(json.at("msgtype").get<std::string>());
}
   
void 
to_json(Json& json, const RunMessage& msg)
{
    json["user"] = msg.user();
    json["msgtype"] = msg.msgType();
}

void 
from_json(const Json& json, RunMessage& msg)
{
    msg.user(json.at("user").get<std::string>());
    msg.msgType(json.at("msgtype").get<std::string>());
}

void 
to_json(Json& json, const DBMessage& msg)
{
    json["user"] = msg.user();
    json["msgtype"] = msg.msgType();
    json["name"] = msg.name();
    json["cmd"] = msg.cmd();
}

void 
from_json(const Json& json, DBMessage& msg)
{
    msg.user(json.at("user").get<std::string>());
    msg.msgType(json.at("msgtype").get<std::string>());
    msg.name(json.at("name").get<std::string>());
    msg.cmd(json.at("cmd").get<int>());
}

void 
to_json(Json& json, const OptimiseMessage& msg)
{
    json["user"] = msg.user();
    json["msgtype"]    = msg.msgType();
    json["iterations"] = msg.iters();
    json["seed"]       = msg.seed();
    json["obj_type"]   = msg.objFuncType();
    json["obj_params"] = msg.objFuncParams();
    json["llhs"]       = msg.llhs();
}

void 
from_json(const Json& json, OptimiseMessage& msg)
{
    msg.user(json.at("user").get<std::string>());
    msg.msgType(json.at("msgtype").get<std::string>());
    msg.iters(json.at("iterations").get<int>());
    msg.seed(json.at("seed").get<int>());
    msg.objFuncType(json.at("obj_type").get<int>());
    msg.objFuncParams(json.at("obj_params").get<std::vector<ObjType>>());
    msg.llhs(json.at("llhs").get<std::vector<int>>());
}

void 
to_json(Json& json, const LookAheadMessage& msg)
{
    json["user"] = msg.user();
    json["pipe_id"] = msg.pipeID();
    json["msgtype"] = msg.msgType();
}

void 
from_json(const Json& json, LookAheadMessage& msg)
{
    msg.user(json.at("user").get<std::string>());
    msg.pipeID(json.at("pipe_id").get<std::string>());
    msg.msgType(json.at("msgtype").get<std::string>());
}

void 
to_json(Json& json, const ChangeMessage& msg)
{
    json["user"] = msg.user();
    json["id"]      = msg.ID();
    json["msgtype"] = msg.msgType();
    json["type"]    = msg.hydType();
    json["value"]   = msg.value();
}

void 
from_json(const Json& json, ChangeMessage& msg)
{
    msg.user(json.at("user").get<std::string>());
    msg.ID(json.at("id").get<std::string>());
    msg.msgType(json.at("msgtype").get<std::string>());
    msg.hydType(static_cast<HydType>(json.at("type").get<int>()));
    msg.value(json.at("value").get<int>());
}


//
// These functions must be defined here so they can 'see' the relevant to_json and from_json functions
//


std::string
HydJSON::toLoadMessageJSON( const LoadMessage& msg, int indent )
{
    return toMessageJSON<LoadMessage>(msg, indent);
}

LoadMessage
HydJSON::fromLoadMessageJSON( const std::string& json_string )
{
    return fromMessageJSON<LoadMessage>(json_string);
}

std::string
HydJSON::toRunMessageJSON( const RunMessage& msg, int indent )
{
    return toMessageJSON<RunMessage>(msg, indent);
}

RunMessage
HydJSON::fromRunMessageJSON( const std::string& json_string )
{
    return fromMessageJSON<RunMessage>(json_string);
}

std::string
HydJSON::toDBMessageJSON( const DBMessage& msg, int indent )
{
    return toMessageJSON<DBMessage>(msg, indent);
}

DBMessage
HydJSON::fromDBMessageJSON( const std::string& json_string )
{
    return fromMessageJSON<DBMessage>(json_string);
}

std::string
HydJSON::toOptimiseMessageJSON( const OptimiseMessage& msg, int indent )
{
    return toMessageJSON<OptimiseMessage>(msg, indent);
}

OptimiseMessage
HydJSON::fromOptimiseMessageJSON( const std::string& json_string )
{
    return fromMessageJSON<OptimiseMessage>(json_string);
}

std::string
HydJSON::toLookAheadMessageJSON( const LookAheadMessage& msg, int indent )
{
    return toMessageJSON<LookAheadMessage>(msg, indent);
}

LookAheadMessage
HydJSON::fromLookAheadMessageJSON( const std::string& json_string )
{
    return fromMessageJSON<LookAheadMessage>(json_string);
}

std::string
HydJSON::toChangeMessageJSON( const ChangeMessage& msg, int indent )
{
    return toMessageJSON<ChangeMessage>(msg, indent);
}

ChangeMessage
HydJSON::fromChangeMessageJSON( const std::string& json_string )
{
    return fromMessageJSON<ChangeMessage>(json_string);
}


std::string
HydJSON::toLookAheadJSON( const HydLookAhead& msg, int indent)
{
    return toMessageJSON<HydLookAhead>(msg, indent);
}

HydLookAhead
HydJSON::fromLookAheadJSON( const std::string& json_string )
{
    return fromMessageJSON<HydLookAhead>(json_string);
}

//
// HydScenarios - constraints, options etc
//

void 
to_json(Json& json, const HydOptions& comp)
{
    json["options"] = comp.values();
    json["cost"]    = comp.costs();
    json["group"]   = comp.group();
    json["type"]    = comp.hydType();
}

void 
from_json(const Json& json, HydOptions& comp)
{
    comp.values(json.at("options").get<std::vector<HydFloat>>());
    comp.costs(json.at("cost").get<std::vector<HydFloat>>());
    comp.group(json.at("group").get<std::string>());
    comp.hydType(static_cast<HydType>(json.at("type").get<int>()));
}

void 
to_json(Json& json, const HydDVariable& comp)
{
    json["id"]    = comp.ID();
    json["group"] = comp.group();
    json["type"]  = comp.hydType();
}

void 
from_json(const Json& json, HydDVariable& comp)
{
    comp.ID(json.at("id").get<std::string>());
    comp.group (json.at("group").get<std::string>());
    comp.hydType(static_cast<HydType>(json.at("type").get<int>()));
}

void 
to_json(Json& json, const HydConstraint& comp)
{
    json["id"]     = comp.ID();
    json["values"] = comp.values();
}

void 
from_json(const Json& json, HydConstraint& comp)
{
    comp.ID(json.at("id").get<std::string>());
    comp.values(json.at("values").get<std::vector<HydFloat>>());
}

//
// coordinates/vertexs
//

void 
to_json(Json& json, const HydVertex& comp)
{
    json["x"] = comp.x;
    json["y"] = comp.y;
    json["z"] = comp.z;
}

void 
from_json(const Json& json, HydVertex& comp)
{
    HydFloat x = json.at("x").get<HydFloat>();
    HydFloat y = json.at("y").get<HydFloat>();
    HydFloat z = json.at("z").get<HydFloat>();
    comp.set(x, y, z);
}


//
// Look Ahead Data
//

void
to_json(Json& json, const HydLookAhead& comp)
{
    json["diameter_index"] = comp.selectedDiamIdx();
    json["currency"]       = comp.currency();
    json["pipe_id"]        = comp.selectedPipeId();
    json["diameter_unit"]  = comp.diamUnits();
    json["obj_names"]      = comp.objNames();
    json["obj_units"]      = comp.objUnits();
    json["diams"]          = comp.diameters();
    json["diams_cost"]     = comp.diametersCost();
    json["obj_values"]     = comp.objValues();
    json["obj_min"]        = comp.objMin();
    json["obj_max"]        = comp.objMax();
    json["obj_cur"]        = comp.objValuesCurrent();
    json["obj_minimise"]   = comp.objMinimise();
}

void 
from_json(const Json& json, HydLookAhead& comp)
{
    int diamIdx = json.at("diameter_index").get<int>();
    comp.setSelectedDiamIdx(diamIdx);
    
    std::string currency = json.at("currency").get<std::string>();
    comp.setCurrency(currency);
    
    std::string pipeID = json.at("pipe_id").get<std::string>();
    comp.setSelectedPipeId(pipeID);
    
    std::string diamUnit = json.at("diameter_unit").get<std::string>();
    comp.setDiamUnits(diamUnit);
    
    std::vector<std::string> objNames = json.at("obj_names").get<std::vector<std::string>>();
    comp.setObjNames(objNames);
    
    std::vector<std::string> objUnits = json.at("obj_units").get<std::vector<std::string>>();
    comp.setObjNames(objUnits);
    

    std::vector<HydFloat> diams = json.at("diams").get<std::vector<HydFloat>>();
    comp.setDiameters(diams); 
    
    std::vector<HydFloat> diamsCost = json.at("diams_cost").get<std::vector<HydFloat>>();
    comp.setDiametersCost(diamsCost);
    
    std::vector<std::vector<HydObjectives>> objValues = json.at("obj_values").get<std::vector<std::vector<HydObjectives>>>();
    comp.setObjValues(objValues);
    
    std::vector<HydObjectives> objMin = json.at("obj_min").get<std::vector<HydObjectives>>();
    comp.setObjMin(objMin);
    
    std::vector<HydObjectives> objMax = json.at("obj_max").get<std::vector<HydObjectives>>();
    comp.setObjMax(objMax);
    
    std::vector<HydObjectives> objCur = json.at("obj_cur").get<std::vector<HydObjectives>>();
    comp.setObjValuesCurrent(objCur);
    
    std::vector<bool> objMinimise = json.at("obj_minimise").get<std::vector<bool>>();
    comp.setObjMinimise(objMinimise);
}



//
// link - pipes
//

void 
to_json(Json& json, const HydPipe& comp) 
{
    json["id"]             = comp.ID();
    json["group"]          = comp.group();
    json["coords"]         = comp.coords();
    json["fromNodeId"]     = comp.fromNodeID();
    json["toNodeId"]       = comp.toNodeID();
    json["diameter"]       = comp.diameter();
    json["length"]         = comp.length();
    json["flow"]           = comp.flow();
    json["velocity"]       = comp.velocity();
    json["headloss"]       = comp.unitHeadloss();
    json["frictionFactor"] = comp.frictionFactor();
    json["reactionRate"]   = comp.reactionRate();
    json["quality"]        = comp.quality();
    json["roughness"]      = comp.roughness();
    json["bulk_coeff"]     = comp.bulkCoeff();
    json["wall_coeff"]     = comp.wallCoeff();
    json["status"]         = comp.status();
    json["initial_status"] = comp.initialStatus();
}

void 
from_json(const Json& json, HydPipe& comp) 
{
    std::string PipeID = json.at("id").get<std::string>();
    comp.setID(PipeID);
    
    std::string Group = json.at("group").get<std::string>();
    comp.setGroup(Group);
    
    std::vector<HydVertex> coords = json.at("coords").get<std::vector<HydVertex>>();
    comp.setCoords(coords);
    
    std::string  sPipeFromNode = json.at("fromNodeId").get<std::string>();
    comp.setFromNodeID(sPipeFromNode);

    std::string sPipeToNode = json.at("toNodeId").get<std::string>();
    comp.setToNodeID(sPipeToNode);

    HydFloat PipeDiameter = json.at("diameter").get<HydFloat>();
    comp.setDiameter(PipeDiameter);

    HydFloat PipeLength = json.at("length").get<HydFloat>();
    comp.setLength(PipeLength);
    
    HydFloat Roughness = json.at("roughness").get<HydFloat>();
    comp.setRoughness(Roughness);

    HydFloat BulkCoeff = json.at("bulk_coeff").get<HydFloat>();
    comp.setBulkCoeff(BulkCoeff);
    
    HydFloat WallCoeff = json.at("wall_coeff").get<HydFloat>();
    comp.setWallCoeff(WallCoeff);

    std::vector<HydFloat> FlowVector = json.at("flow").get<std::vector<HydFloat>>();
    comp.setFlow(FlowVector);

    std::vector<HydFloat> VelocityVector = json.at("velocity").get<std::vector<HydFloat>>();
    comp.setVelocity(VelocityVector);

    std::vector<HydFloat> HeadLossVector = json.at("headloss").get<std::vector<HydFloat>>();
    comp.setUnitHeadLoss(HeadLossVector);

    std::vector<HydFloat> FrictionFactorVector = json.at("frictionFactor").get<std::vector<HydFloat>>();
    comp.setFrictionFactor(FrictionFactorVector);


    std::vector<HydFloat> ReactionRateVector = json.at("reactionRate").get<std::vector<HydFloat>>();
    comp.setReactionRate(ReactionRateVector);
    
    // check that quality is defined
    if (json.find("quality") != json.end() && !json.at("quality").is_null())
    {
        std::vector<HydFloat> QualityVector = json.at("quality").get<std::vector<HydFloat>>();
        comp.setQuality(QualityVector);
    }
    
    if (json.find("status") != json.end() && !json.at("status").is_null())
    {
        std::vector<LinkStatusOption> StatusVector = json.at("status").get<std::vector<LinkStatusOption>>();
        comp.setStatus(StatusVector);
    }
    
    PipeStatus InitialStatus = json.at("initial_status").get<PipeStatus>();
    comp.setInitialStatus(InitialStatus);
}

//
// link pumps
//

void 
to_json(Json& json, const HydPump& comp)
{
    json["id"]         = comp.ID();
    json["group"]      = comp.group();
    json["coords"]     = comp.coords();
    json["fromNodeId"] = comp.fromNodeID();
    json["toNodeId"]   = comp.toNodeID();
}

void 
from_json(const Json& json, HydPump& comp)
{
    std::string JunctionID = json.at("id").get<std::string>();
    comp.setID(JunctionID);
    
    std::string Group = json.at("group").get<std::string>();
    comp.setGroup(Group);
    
    std::vector<HydVertex> coords = json.at("coords").get<std::vector<HydVertex>>();
    comp.setCoords(coords);
    
    std::string sPumpFromNode = json.at("fromNodeId").get<std::string>();
    comp.setFromNodeID(sPumpFromNode);

    std::string sPumpToNode = json.at("toNodeId").get<std::string>();
    comp.setToNodeID(sPumpToNode);
}

//
// link - valves
//

void 
to_json(Json& json, const HydValve& comp) 
{
    json["id"]         = comp.ID();
    json["group"]      = comp.group();
    json["coords"]     = comp.coords();
    json["fromNodeId"] = comp.fromNodeID();
    json["toNodeId"]   = comp.toNodeID();
    json["diameter"]   = comp.diameter();
    json["valve_type"] = comp.type();
}

void 
from_json(const Json& json, HydValve& comp)
{
    std::string ValveID = json.at("id").get<std::string>();
    comp.setID(ValveID);
    
    std::string Group = json.at("group").get<std::string>();
    comp.setGroup(Group);
    
    std::vector<HydVertex> coords = json.at("coords").get<std::vector<HydVertex>>();
    comp.setCoords(coords);
    
    std::string sValveFromNode = json.at("fromNodeId").get<std::string>();
    comp.setFromNodeID(sValveFromNode);

    std::string sValveToNode = json.at("toNodeId").get<std::string>();
    comp.setToNodeID(sValveToNode);

    HydFloat ValveDiameter = json.at("diameter").get<HydFloat>();
    comp.setDiameter(ValveDiameter);

    int ValveTypeInt = json.at("valve_type").get<int>();

    switch (ValveTypeInt)
    {
        case 3:
            comp.setType(ValveType::PRV);
            break;
        case 4:
            comp.setType(ValveType::PSV);
            break;
        case 5:
            comp.setType(ValveType::PBV);
            break;
        case 6:
            comp.setType(ValveType::FCV);
            break;
        case 7:
            comp.setType(ValveType::TCV);
            break;
        case 8:
            comp.setType(ValveType::GPV);
            break;
        default:
            std::cout << "Valve %s has unknown type " << ValveID << ". Setting to GPV..." << std::endl;
            comp.setType(ValveType::GPV);
            break;
    }
}


//
// node - junctions
//

void 
to_json(Json& json, const HydJunction& comp) 
{
    json["id"]           = comp.ID();
    json["group"]        = comp.group();
    json["coord"]        = comp.coords();
    json["elevation"]    = comp.baseElevation();
    json["base_demand"]  = comp.baseDemand();
    json["total_head"]   = comp.totalHead();
    json["head_deficit"] = comp.headDeficit();
    json["head_excess"]  = comp.headExcess();
    json["min_head"]     = comp.minHead();
    json["pressure"]     = comp.pressure();
    json["actual_demand"] = comp.actualDemand();
    json["quality"]      = comp.quality();
}

void 
from_json(const Json& json, HydJunction& comp) 
{
    std::string JunctionID = json.at("id").get<std::string>();
    comp.setID(JunctionID);
    
    std::string Group = json.at("group").get<std::string>();
    comp.setGroup(Group);
    
    HydVertex JunctionCoord = json.at("coord").get<HydVertex>();
    comp.setCoords(JunctionCoord);
    
    HydFloat JunctionElevation = json.at("elevation").get<HydFloat>(); ;
    comp.setBaseElevation(JunctionElevation);
    
    HydFloat JunctionBaseDemand = json.at("base_demand").get<HydFloat>(); ;
    comp.setBaseDemand(JunctionBaseDemand);
    
    std::vector<HydFloat> JunctionHead = json.at("total_head").get<std::vector<HydFloat>>(); 
    comp.setTotalHead(JunctionHead);
    
    std::vector<HydFloat> JunctionDeficit = json.at("head_deficit").get<std::vector<HydFloat>>(); 
    comp.setHeadDeficit(JunctionDeficit);
    
    std::vector<HydFloat> JunctionExcess = json.at("head_excess").get<std::vector<HydFloat>>(); 
    comp.setHeadExcess(JunctionExcess);
    
    std::vector<HydFloat> JunctionMinHead = json.at("min_head").get<std::vector<HydFloat>>(); 
    comp.setTotalHead(JunctionMinHead);
    
    std::vector<HydFloat>  JunctionPressure = json.at("pressure").get<std::vector<HydFloat>>(); 
    comp.setPressure(JunctionPressure);
    
    std::vector<HydFloat> ActualDemand = json.at("actual_demand").get<std::vector<HydFloat>>(); 
    comp.setActualDemand(ActualDemand);
    
    if ( json.find("quality") != json.end() && !json.at("quality").is_null())
    {
        std::vector<HydFloat>  JunctionQuality = json.at("quality").get<std::vector<HydFloat>>();
        comp.setQuality(JunctionQuality);
    }
}

//
// node - reservoirs
//

void 
to_json(Json& json, const HydReservoir& comp)
{
    json["id"]         = comp.ID();
    json["group"]      = comp.group();
    json["coord"]      = comp.coords();
    json["total_head"] = comp.totalHead();
    json["elevation"]  = comp.elevation();
    json["inflow"]     = comp.netInflow();
    json["pressure"]   = comp.pressure();
    json["quality"]    = comp.quality();
}

void 
from_json(const Json& json, HydReservoir& comp)
{
    std::string ReservoirID = json.at("id").get<std::string>();
    comp.setID(ReservoirID);
    
    std::string Group = json.at("group").get<std::string>();
    comp.setGroup(Group);
    
    HydVertex ReservoirCoord = json.at("coord").get<HydVertex>();
    comp.setCoords(ReservoirCoord);

    HydFloat ReservoirHead = json.at("total_head").get<HydFloat>();
    comp.setTotalHead(ReservoirHead);
    
    if (json.find("elevation") != json.end())
    {
        HydFloat ReservoirElevation = json.at("elevation").get<HydFloat>();
        comp.setElevation(ReservoirElevation);
    }
    
    if (json.find("inflow") != json.end())
    {
        std::vector<HydFloat> ReservoirInflow = json.at("inflow").get<std::vector<HydFloat>>();
        comp.setNetInflow(ReservoirInflow);
    }
    
    std::vector<HydFloat>  ReservoirPressure = json.at("pressure").get<std::vector<HydFloat>>(); 
    comp.setPressure(ReservoirPressure);
    
    if ( json.find("quality") != json.end() && !json.at("quality").is_null())
    {
        std::vector<HydFloat>  ReservoirQuality = json.at("quality").get<std::vector<HydFloat>>();
        comp.setQuality(ReservoirQuality);
    }
}

//
// node - tanks
//

void 
to_json(Json& json, const HydTank& comp) 
{
    json["id"]        = comp.ID();
    json["group"]     = comp.group();
    json["coord"]     = comp.coords();
    json["elevation"] = comp.elevation();
    json["initlvl"]   = comp.initLevel();
    json["minlvl"]    = comp.minLevel();
    json["maxlvl"]    = comp.maxLevel();
    json["diameter"]  = comp.diameter();
}

void 
from_json(const Json& json, HydTank& comp)
{
    std::string TankID = json.at("id").get<std::string>();
    comp.setID(TankID);
    
    std::string Group = json.at("group").get<std::string>();
    comp.setGroup(Group);
    
    HydVertex TankCoord = json.at("coord").get<HydVertex>();
    comp.setCoords(TankCoord);

    HydFloat TankElevation = json.at("elevation").get<HydFloat>();
    comp.setBaseElevation(TankElevation);
    
    HydFloat TankInitLevel = json.at("initlvl").get<HydFloat>();
    comp.setInitLevel(TankInitLevel);
    
    HydFloat TankMinLevel = json.at("minlvl").get<HydFloat>();
    comp.setMinLevel(TankMinLevel);
    
    HydFloat TankMaxLevel = json.at("maxlvl").get<HydFloat>();
    comp.setMaxLevel(TankMaxLevel);
    
    HydFloat TankDiameter = json.at("diameter").get<HydFloat>();
    comp.setDiameter(TankDiameter);
}

//
// end of json functions
//


//
// the code
//

HydNetwork*
HydJSON::loadNetwork( const std::string& file_name )
{
    Json json;
    HydNetwork* hydNet = nullptr;
    
    try
    {
        std::ifstream file;
        file.exceptions( std::ifstream::badbit | std::ifstream::failbit );
        file.open( file_name );

        json = Json::parse(file);
        
        hydNet = new HydNetwork;
        
        if (buildNetwork( json, hydNet ))
        {
            //int indent = 2;
            //std:: cout << json.dump(indent) << std::endl;
            hydNet->setName( file_name );
            return hydNet;
        }
    }
    catch( const std::ifstream::failure e )
    {
        std::cout  << "Failed to load \"" << file_name << "\" !" << std::endl;
    }
    catch( const nlohmann::detail::parse_error e )
    {
        std::cout  << e.what() << std::endl;
    }
    
    delete hydNet;
    return nullptr;
}

void
HydJSON::saveNetwork( const std::string& file_name, HydNetwork* hydNet, int indent )
{
    if (!hydNet)
        return;
    
    try
    {
        std::ofstream file;
        file.exceptions( std::ofstream::badbit | std::ofstream::failbit );
        file.open( file_name );
        file << toNetworkJSON(hydNet,indent);
    }
    catch( const std::ofstream::failure e )
    {
        std::cout  << "Failed to save \"" << file_name << "\" !" << std::endl;
    }
}


std::string
HydJSON::toNetworkJSON( const HydNetwork* hydNet, int indent )
// Construct a JSON string from the network for communication with client
// return: JSON Object
{
    if (!hydNet)
        return "";
    
    Json json_network;
    
    json_network["name"]         = hydNet->name();
    //json_network["look_ahead"]  = hydNet->lookAhead();
    json_network["options"]      = hydNet->options(); 

    json_network["time_points"]  = hydNet->timePoints();
    
    json_network["cost"]         = hydNet->cost(); 
    json_network["resilience"]   = hydNet->resilience();
    json_network["head_deficit"] = hydNet->headDeficit();
    
    json_network["min_diam"]     = hydNet->minMaxDiameter().first;
    json_network["max_diam"]     = hydNet->minMaxDiameter().second;
    
    for (auto i = hydNet->junctions().begin(); i != hydNet->junctions().end(); ++i)
        json_network["junctions"].push_back(**i);
    
    for (auto i = hydNet->pipes().begin(); i != hydNet->pipes().end(); ++i)
        json_network["pipes"].push_back(**i);

    for (auto i = hydNet->pumps().begin(); i != hydNet->pumps().end(); ++i)
        json_network["pumps"].push_back(**i);

    for (auto i = hydNet->valves().begin(); i != hydNet->valves().end(); ++i)
        json_network["valves"].push_back(**i);

    for (auto i = hydNet->reservoirs().begin(); i != hydNet->reservoirs().end(); ++i)
        json_network["reservoirs"].push_back(**i);

    for (auto i = hydNet->tanks().begin(); i != hydNet->tanks().end(); ++i)
        json_network["tanks"].push_back(**i);
    
     return json_network.dump(indent);
    //return json::to_bson(json_network);
} 

HydNetwork*
HydJSON::fromNetworkJSON( const std::string& json_string )
{
    HydNetwork* hydNet = nullptr;
    
    try
    {
        //Json json = Json::from_bson(json_string_bin);
        Json json = Json::parse(json_string);
        hydNet = new HydNetwork;
        
        hydNet->setName(json.at("name").get<std::string>());
       // hydNet->setLookAhead(json.at("look_ahead").get<HydLookAhead>());
        hydNet->setTimePoints(json.at("time_points").get<std::vector<long>>());
        hydNet->setOptions(json.at("options").get<std::map<std::string, HydOptions>>());
        
        buildNetwork( json, hydNet );
        
        return hydNet;
    }
    catch (Json::parse_error& e)
    {
        std::cout << e.what() << std::endl;
    }
    
   
    delete hydNet;
    
    return nullptr;
}

void
HydJSON::updateNetworkJSON( const std::string& json_string, HydNetwork* hydNet )
{
    try
    {
        Json json = Json::parse(json_string);
        buildNetwork( json, hydNet );
    }
    catch (Json::parse_error& e)
    {
        std::cout << e.what() << std::endl;
    }  
}

bool 
HydJSON::buildNetwork( const Json& json, HydNetwork* hydNet )
{

    std::vector<HydFloat> cost         = json.at("cost").get<std::vector<HydFloat>>();
    std::vector<HydFloat> resilience   = json.at("resilience").get<std::vector<HydFloat>>();
    std::vector<HydFloat> head_deficit = json.at("head_deficit").get<std::vector<HydFloat>>();
    
    hydNet->setObjectives( cost, head_deficit, resilience );
    
    hydNet->minDiameter( json.at("min_diam").get<HydFloat>() );
    hydNet->maxDiameter( json.at("max_diam").get<HydFloat>() );
    
    addComponent<HydJunction>("junctions", HydType::Junction, json, hydNet);
    addComponent<HydPipe>("pipes", HydType::Pipe, json, hydNet);
    addComponent<HydReservoir>("reservoirs", HydType::Reservoir, json, hydNet);
    addComponent<HydTank>("tanks", HydType::Tank, json, hydNet);
    addComponent<HydPump>("pumps", HydType::Pump, json, hydNet);
    addComponent<HydValve>("valves", HydType::Valve, json, hydNet);

    return true;
}

//
//
//

HydScenarios
HydJSON::loadScenarios( const std::string& file_name )
{
    Json json;
    
    try
    {
        std::ifstream file;
        file.exceptions( std::ifstream::badbit | std::ifstream::failbit );
        file.open( file_name );
        file >> json;
        
        std::map<std::string,HydOptions> options     = json.at("options").get<std::map<std::string,HydOptions>>();
        std::vector<HydDVariable>         decisions   = json.at("decisions").get<std::vector<HydDVariable>>();
        std::vector<HydConstraint>       constraints = json.at("constraints").get<std::vector<HydConstraint>>();
        return    HydScenarios(options, decisions, constraints); 
    }
    catch( const std::ifstream::failure e )
    {
        std::cout  << "Failed to load \"" << file_name << "\" !" << std::endl;
    }
    catch( const nlohmann::detail::parse_error e )
    {
        std::cout  << e.what() << std::endl;
    }
    
    return HydScenarios();
}

void
HydJSON::saveScenarios( const std::string& file_name, const HydScenarios& prob, int indent )
{
    try
    {
        std::ofstream file;
        file.exceptions( std::ofstream::badbit | std::ofstream::failbit );
        file.open( file_name );
        file << toScenariosJSON(prob,indent);
    }
    catch( const std::ofstream::failure e )
    {
        std::cout  << "Failed to save \"" << file_name << "\" !" << std::endl;
    }
}

std::string
HydJSON::toScenariosJSON( const HydScenarios& prob, int indent )
{
    Json json_problem;
    json_problem["options"] = prob.options();
    json_problem["decisions"] = prob.decisions();
    json_problem["constraints"] = prob.constraints();
    return json_problem.dump(indent);
}

HydScenarios
HydJSON::fromScenariosJSON( const std::string& json_string )
{
    Json json = Json::parse(json_string);
    std::map<std::string,HydOptions> options     = json.at( "options" ).get<std::map<std::string,HydOptions>>();
    std::vector<HydDVariable>        decisions   = json.at( "decisions" ).get<std::vector<HydDVariable>>();
    std::vector<HydConstraint>       constraints = json.at( "constraints" ).get<std::vector<HydConstraint>>();
    return HydScenarios(options, decisions, constraints); 
}



