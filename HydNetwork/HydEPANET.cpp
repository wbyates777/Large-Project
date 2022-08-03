/* HydEPANET 17/02/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   HydEPANET.cpp   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HYDEPANET_H__
#include "HydEPANET.h"
#endif

#ifndef __HYDOBJECTIVE_H__
#include "HydObjective.h"
#endif

extern "C" {
#include "types.h"
#include "funcs.h"
#include "epanet2_2.h"
}

int SHOW_EPANET_ERRORS = 1;
int SHOW_EPANET_WARNINGS = 1;

HydEPANET::HydEPANET( void ) : m_proj(nullptr), m_hydNet(nullptr), m_epanetOpen(0), m_extended(false)
{
    m_logger.getLogLevel( "HydEPANET" ); 
}

HydEPANET::~HydEPANET( void )
{
    if (m_epanetOpen)
    {   
        EN_close(m_proj); 
        EN_deleteproject(m_proj);
        m_proj = nullptr;
        m_epanetOpen = 0;
    }
    
    // do not delete - HydEPANET caller owns memory
    m_hydNet = nullptr;
}

bool
HydEPANET::epanetError(int error_code ) const
/*
 WARNINGS - not errors
 System_hydraulically_unbalanced = 1
 System_may_be_hydraulically_unstable = 2
 System_disconnected = 3
 Pumps_cannot_deliver_enough_flow_or_head = 4
 Valves_cannot_deliver_enough_flow = 5
 System_has_negative_pressures = 6
 */
{

    if (error_code)
    {

        if (SHOW_EPANET_ERRORS && error_code > 6)
        {
            char msg[128];
            EN_geterror( error_code, msg, 128);
            if (m_logger.level() >= 1)
            {
                char msg[128];
                EN_geterror( error_code, msg, 128);
                
                Message txt(1);
                txt << "EPANET ERROR: " << std::string(msg);
                m_logger.logMsg(txt);
                m_logger.flush();
            }
            
            return true;
        }
        

    }
    return false;
}



double 
HydEPANET::pressure( int node_idx ) const
{ 
    double p;
    EN_getnodevalue(m_proj, node_idx, EN_PRESSURE, &p);
    return p;
}

double 
HydEPANET::head( int node_idx ) const 
{ 
    double h;
    EN_getnodevalue(m_proj, node_idx, EN_HEAD, &h);
    return h;   
} 

double 
HydEPANET::diameter( int link_idx ) const
{
    double d;
    EN_getlinkvalue(m_proj, link_idx, EN_DIAMETER, &d);
    return d; 
}

void 
HydEPANET::setDiameter( int link_idx, double diameter ) const
{
    EN_setlinkvalue(m_proj, link_idx, EN_DIAMETER, diameter);
}

double 
HydEPANET::length( int link_idx ) const
{
    double d;
    EN_getlinkvalue(m_proj, link_idx, EN_LENGTH, &d);
    return d; 
}

LinkStatusOption 
HydEPANET::valveStatus( int link_idx ) const
{
    double d;
    EN_getlinkvalue(m_proj, link_idx, EN_STATUS, &d);
    return (d > 0.0) ?  LinkStatusOption::Open : LinkStatusOption::Closed;
}

void
HydEPANET::setValveStatus( int link_idx, LinkStatusOption status ) const
{
    double d = (status == LinkStatusOption::Open) ? 1.0 : 0.0;
    EN_setlinkvalue(m_proj, link_idx, EN_STATUS, d);
}



HydNetwork*
HydEPANET::load( const std::string& name )
{
    int errcode = 0;
    
    if (m_epanetOpen)
    {   
        EN_close(m_proj); 
        EN_deleteproject(m_proj);
        m_epanetOpen = 0;
    }
    
    // initalise EPANET
    errcode = EN_createproject(&m_proj);
    if (epanetError(errcode))
        exit(EXIT_FAILURE);
    
    char file1[] = "report.txt";
    char file2[] = "";

    errcode = EN_open(m_proj, name.c_str(), file1, file2);
    if (epanetError(errcode))
        exit(EXIT_FAILURE);
    
    EN_resetreport(m_proj);
    char msg1[] = "MESSAGES NO";
    EN_setreport(m_proj, msg1);
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Loaded network file " << name;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    // time stuff
    long run_time; // in seconds
    errcode = EN_gettimeparam(m_proj, EN_DURATION, &run_time);
    if (epanetError(errcode))
        exit(EXIT_FAILURE);
    
    long time_step; // in seconds
    errcode = EN_gettimeparam(m_proj, EN_HYDSTEP, &time_step); 
    if (epanetError(errcode))
        exit(EXIT_FAILURE);
    
    m_extended = false;
    if (run_time > 0)
    {
        m_extended = true;
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Network uses " << 1 + (run_time / time_step) << " time points";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }
    //
    
    m_epanetOpen = 1;
    
    // do not delete - user owns this memory now
    m_hydNet = new HydNetwork;
    
    buildNetwork();
    m_hydNet->setName( name ); 
    return m_hydNet;
}

void
HydEPANET::run(void) const
{
    int errcode = EN_openH(m_proj);
    if (epanetError(errcode))
        exit(EXIT_FAILURE);

    errcode = EN_initH(m_proj,0);
    if (epanetError(errcode))
        exit(EXIT_FAILURE);
    
    int scenario_index = 0;
    long currentTime = 0; // seconds
    long tstep = 0;       // seconds
    
    m_hydNet->clear();
    
    while (true)
    {
        errcode = EN_runH(m_proj, &currentTime);

        if (epanetError(errcode))
            exit(EXIT_FAILURE);

        // query EPANET and update the dynamic values in hydNet
        updateNetwork(scenario_index);
        
        m_hydNet->addTimePoints(currentTime);
        
        ++scenario_index;
        
        errcode = EN_nextH(m_proj, &tstep);
        if (epanetError(errcode))
            exit(EXIT_FAILURE);
        
        if (tstep <= 0)
            break;
    }
    
    errcode = EN_closeH(m_proj);
    if (epanetError(errcode))
        exit(EXIT_FAILURE);
}

void
HydEPANET::updateNetwork(HydPipe* pipe) const
{
    int err;
    
    // diameter can be modified
    double diameter;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_DIAMETER, &diameter);
    epanetError(err);
    
    double flow;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_FLOW, &flow);
    epanetError(err);
    
    double velocity;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_VELOCITY, &velocity);
    epanetError(err);
    
    double headloss;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_HEADLOSS, &headloss);
    epanetError(err);
    
    double status;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_STATUS, &status);
    epanetError(err);
    
    double quality;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_QUALITY, &quality);
    epanetError(err);
    
    double roughness;
    err = EN_getlinkvalue(m_proj, pipe->index(), EN_ROUGHNESS, &roughness);
    epanetError(err);
    

    // friction_factor = #dunno how to get friction factor.... Could be //EN_SETTING....?
    // reaction_rate = #dunno how to get reaction rate....?

    if (m_extended)
    {
        pipe->setDiameter(diameter);
        pipe->addFlow(flow);
        pipe->addVelocity(velocity);
        pipe->addUnitHeadLoss(headloss);
        pipe->addQuality(quality);
        pipe->setRoughness(roughness);
        
        pipe->addReactionRate(0.0);
        pipe->addFrictionFactor(0.0);
        
        //  Note EN_CLOSED = 0,  EN_OPEN = 1
        if (status > 0.0)
            pipe->addStatus( LinkStatusOption::Open );
        else pipe->addStatus( LinkStatusOption::Closed );
    }
    else
    {
        // overwrites any existing data
        pipe->setDiameter(diameter);
        pipe->setFlow(flow);
        pipe->setVelocity(velocity);
        pipe->setUnitHeadLoss(headloss);
        pipe->setQuality(quality);
        pipe->setRoughness(roughness);
        
        pipe->setReactionRate(0.0);
        pipe->setFrictionFactor(0.0);
        
        //  Note EN_CLOSED = 0,  EN_OPEN = 1
        if (status > 0.0)
            pipe->setStatus( LinkStatusOption::Open );
        else pipe->setStatus( LinkStatusOption::Closed );
    }
}

void
HydEPANET::updateNetwork(HydPump* pump) const
{
    int err;
    
    double flow;
    err = EN_getlinkvalue(m_proj, pump->index(), EN_FLOW, &flow);
    epanetError(err);
    
    double status;
    err = EN_getlinkvalue(m_proj, pump->index(), EN_STATUS, &status);
    epanetError(err);

    double quality;
    err = EN_getlinkvalue(m_proj, pump->index(), EN_QUALITY, &quality);
    epanetError(err);
    
    if (m_extended)
    {
        pump->addFlow(flow);
        pump->addQuality(quality);

        //  Note EN_CLOSED = 0,  EN_OPEN = 1
        if (status > 0.0)
            pump->addStatus( LinkStatusOption::Open );
        else pump->addStatus( LinkStatusOption::Closed );
    }
    else
    {
        // overwrites any existing data
        pump->setFlow(flow);
        pump->setQuality(quality);

        //  Note EN_CLOSED = 0,  EN_OPEN = 1
        if (status > 0.0)
            pump->setStatus( LinkStatusOption::Open );
        else pump->setStatus( LinkStatusOption::Closed );

    }
}

void
HydEPANET::updateNetwork(HydTank* tank) const
{
    TODO:
    int err;
}

void
HydEPANET::updateNetwork(HydValve* valve) const
{
    int err;
    
    double flow;
    err = EN_getlinkvalue(m_proj, valve->index(), EN_FLOW, &flow);
    epanetError(err);
    
    double velocity;
    err = EN_getlinkvalue(m_proj, valve->index(), EN_VELOCITY, &velocity);
    epanetError(err);
    
    double headloss;
    err = EN_getlinkvalue(m_proj, valve->index(), EN_HEADLOSS, &headloss);
    epanetError(err);
    
    double status;
    err = EN_getlinkvalue(m_proj, valve->index(), EN_STATUS, &status);
    epanetError(err);

    double quality;
    err = EN_getlinkvalue(m_proj, valve->index(), EN_QUALITY, &quality);
    epanetError(err);
    
    if (m_extended)
    {
        valve->addFlow(flow);
        valve->addVelocity(velocity);
        valve->setHeadloss(headloss);
        valve->addQuality(quality);

        //  Note EN_CLOSED = 0,  EN_OPEN = 1
        if (status > 0.0)
            valve->addStatus( LinkStatusOption::Open );
        else valve->addStatus( LinkStatusOption::Closed );
    }
    else
    {
        // overwrites any existing data
        valve->setFlow(flow);
        valve->setVelocity(velocity);
        valve->setHeadloss(headloss);
        valve->setQuality(quality);

        //  Note EN_CLOSED = 0,  EN_OPEN = 1
        if (status > 0.0)
            valve->setStatus( LinkStatusOption::Open );
        else valve->setStatus( LinkStatusOption::Closed );
    }
}

void
HydEPANET::updateNetwork(HydJunction* junction) const
{
    int err;
    
    double actual_demand;
    err = EN_getnodevalue(m_proj, junction->index(), EN_DEMAND, &actual_demand);
    epanetError(err);
    
    double total_head;
    err = EN_getnodevalue(m_proj, junction->index(), EN_HEAD, &total_head);
    epanetError(err);
    
    double pressure;
    err = EN_getnodevalue(m_proj, junction->index(), EN_PRESSURE, &pressure);
    epanetError(err);
    
    double quality;
    err = EN_getnodevalue(m_proj, junction->index(), EN_QUALITY, &quality);
    epanetError(err);
    
    if (m_extended)
    {
        junction->addActualDemand(actual_demand);
        junction->addTotalHead(total_head);
        junction->addPressure(pressure);
        junction->addQuality(quality);
    }
    else
    {
        // overwrites any existing data
        junction->setActualDemand(actual_demand);
        junction->setTotalHead(total_head);
        junction->setPressure(pressure);
        junction->setQuality(quality);
    }
}


void
HydEPANET::updateNetwork(HydReservoir* reservoir)  const
{
    int err;
    
    double net_inflow;
    err = EN_getnodevalue(m_proj, reservoir->index(), EN_DEMAND, &net_inflow);
    epanetError(err);
    
    double pressure;
    err = EN_getnodevalue(m_proj, reservoir->index(), EN_PRESSURE, &pressure);
    epanetError(err);
    
    double quality;
    err = EN_getnodevalue(m_proj, reservoir->index(), EN_QUALITY, &quality);
    epanetError(err);
    
    if (m_extended)
    {
        reservoir->addNetInflow(std::fabs(net_inflow));
        reservoir->addPressure(pressure);
        reservoir->addQuality(quality);
    }
    else 
    {
        // overwrites any existing data
        reservoir->setNetInflow(std::fabs(net_inflow)); 
        reservoir->setPressure(pressure);
        reservoir->setQuality(quality);
    }
}


void
HydEPANET::updateNetwork( int scenario_index ) const
// only update the dynamic varaibles i.e don't update pipe length
{
    if (!m_hydNet)
        return; 
    
    const std::vector<HydPipe*>& pipes = m_hydNet->pipes();
    for (auto i = pipes.begin(); i != pipes.end(); ++i)
        updateNetwork(*i);

    const std::vector<HydJunction*>& junctions = m_hydNet->junctions();
    for (auto i = junctions.begin(); i != junctions.end(); ++i)
        updateNetwork(*i);   
        
    const std::vector<HydReservoir*>& reservoirs = m_hydNet->reservoirs();
    for (auto i = reservoirs.begin(); i != reservoirs.end(); ++i)
        updateNetwork(*i);
    
    const std::vector<HydTank*>& tanks = m_hydNet->tanks();
    for (auto i = tanks.begin(); i != tanks.end(); ++i)
        updateNetwork(*i);
    
    const std::vector<HydPump*>& pumps = m_hydNet->pumps();
    for (auto i = pumps.begin(); i != pumps.end(); ++i)
        updateNetwork(*i);
    
    const std::vector<HydValve*>& valves = m_hydNet->valves();
    for (auto i = valves.begin(); i != valves.end(); ++i)
        updateNetwork(*i);
    
    double c = HydObjective::cost(m_hydNet) / 1E6;                      // cost in millions
    double h = HydObjective::headDeficit(m_hydNet, scenario_index);     // calls junction->setHeadDeficit(deficit);
    double r = HydObjective::resilience(m_hydNet, scenario_index);
    
    if (m_extended)
        m_hydNet->addObjectives(c, h, r);
    else m_hydNet->setObjectives(c, h, r);
}

void
HydEPANET::buildNetwork( void )
{
    int err;
    
    int numLinks;
    err = EN_getcount(m_proj, EN_LINKCOUNT, &numLinks);
    epanetError(err);
    
    int numNodes;
    err = EN_getcount(m_proj, EN_NODECOUNT, &numNodes);
    epanetError(err);

    for (int i = 0; i < numLinks; ++i)
    {
        int link_type;
        err = EN_getlinktype(m_proj,i+1, &link_type );
        epanetError(err);
        
        char link_id[64];
        err = EN_getlinkid(m_proj,i+1, link_id);
        epanetError(err);

        switch (link_type)
        {
            case EN_PIPE: 
                addPipe(i+1, link_id);  // - cv - false
                break; 
            case EN_CVPIPE: 
                addPipe(i+1, link_id);  // - cv - true
                break; 
            case EN_PUMP: 
                addPump(i+1, link_id); 
                break; 
            case EN_PRV:
            case EN_PSV:
            case EN_PBV:
            case EN_FCV:
            case EN_TCV:
            case EN_GPV:
                addValve(i+1, link_id); 
                break;
            default:
                if (m_logger.level() >= 1)
                {
                    Message txt(1);
                    txt << "Build network error: unknown link type";
                    m_logger.logMsg(txt);
                    m_logger.flush();
                }
                exit(EXIT_FAILURE);
                break;
        }
    }
    
    for (int i = 0; i < numNodes; ++i) 
    {
        int node_type;
        err  = EN_getnodetype(m_proj, i+1, &node_type);
        epanetError(err);

        char node_id[64];
        err = EN_getnodeid(m_proj, i+1, node_id);
        epanetError(err);

        switch (node_type)
        {
            case EN_RESERVOIR: 
                addReservoir(i+1, node_id);
                break; 
            case EN_TANK: 
                addTank(i+1, node_id);
                break; 
            case EN_JUNCTION: 
                addJunction(i+1, node_id);
                break; 
            default:
                if (m_logger.level() >= 1)
                {
                    Message txt(1);
                    txt << "Build network error: unknown node type";
                    m_logger.logMsg(txt);
                    m_logger.flush();
                }
                exit(EXIT_FAILURE);
                break;
        }
    }
    if (m_logger.level() >= 2)
    {
        Message txt(1);
        txt << "Network built with " << numNodes << " nodes and " << numLinks << " links";
        m_logger.logMsg(txt);
        m_logger.flush();
    }
}


//
// link types
//

bool
HydEPANET::addReservoir(int index, const std::string& hid)
{
    int err;
    
    double elevation;
    err = EN_getnodevalue(m_proj, index, EN_ELEVATION, &elevation);
    epanetError(err);

    double head;
    err = EN_getnodevalue(m_proj, index, EN_HEAD, &head);
    epanetError(err);
    
    double x_coord,  y_coord;
    err = EN_getcoord(m_proj, index, &x_coord, &y_coord);
    epanetError(err);
    
    double net_inflow;
    err = EN_getnodevalue(m_proj, index, EN_DEMAND, &net_inflow);
    epanetError(err);
    

    HydReservoir* newReservoir = m_hydNet->findReservoir(hid);
    
    if (newReservoir == nullptr)
    {
        newReservoir = new HydReservoir(hid);
        newReservoir->setCoords(HydVertex(x_coord, y_coord));
        // WARNING: in original network.py Total_Head is set to Elevation
        // this will prevent division by zero errors in HydNetwork::getNodeMinMax()
        //newReservoir->setTotalHead(head); 
        newReservoir->setTotalHead(elevation);
        newReservoir->setElevation(elevation);
        newReservoir->setNetInflow(std::vector<HydFloat>(1,std::fabs(net_inflow)));
        newReservoir->setIndex(index);
        
        m_hydNet->addComponent(newReservoir);
        
        if (m_logger.level() >= 3)
        {
            Message txt(1);
            txt << "Added reservoir " << hid;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        
        return true;
    }
    return false;
}


bool
HydEPANET::addTank(int index, const std::string& hid)
{
    int err;
    double init_level;
    err = EN_getnodevalue(m_proj, index, EN_TANKLEVEL, &init_level);
    epanetError(err);

    double min_level;
    err  = EN_getnodevalue(m_proj, index, EN_MINLEVEL, &min_level);
    epanetError(err);

    double max_level;
    err = EN_getnodevalue(m_proj, index, EN_MAXLEVEL, &max_level);
    epanetError(err);

    double diameter;
    err = EN_getnodevalue(m_proj, index, EN_DIAMETER, &diameter);
    epanetError(err);

    double min_volume;
    err = EN_getnodevalue(m_proj, index, EN_MINVOLUME, &min_volume);
    epanetError(err);

    // returns integer index as a double 
    double volume_curve_id; 
    err = EN_getnodevalue(m_proj, index, EN_VOLCURVE, &volume_curve_id);
    epanetError(err);

    double elevation;
    err = EN_getnodevalue(m_proj, index, EN_ELEVATION, &elevation);
    epanetError(err);

    double x_coord,  y_coord;
    err = EN_getcoord(m_proj, index, &x_coord, &y_coord);
    epanetError(err);
    
    HydTank* newTank = m_hydNet->findTank(hid);

    if (newTank == nullptr)
    {
        newTank = new HydTank(hid);

        newTank->setCoords(HydVertex(x_coord, y_coord));
        newTank->setElevation(elevation);
        newTank->setInitLevel(init_level);
        newTank->setMinLevel(min_level);
        newTank->setMaxLevel(max_level);
        newTank->setDiameter(diameter);
        newTank->setMinVolume(min_volume);
        newTank->setIndex(index);
       // newTank->setMixingFraction( mixing_fraction);
       // newTank->setReactionCoeff( reaction_coeff);
       // newTank->setBaseElevation( base_elevation);
        newTank->setVolumeCurveID(std::to_string(volume_curve_id));
        
        m_hydNet->addComponent(newTank);
        
        if (m_logger.level() >= 3)
        {
            Message txt(1);
            txt << "Added tank " << hid;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        
        return true;
    }
    return false;
}

bool
HydEPANET::addJunction(int index, const std::string& hid)
{
    int err;
    
    double base_elevation;
    err = EN_getnodevalue(m_proj, index, EN_ELEVATION, &base_elevation);
    epanetError(err);

    double base_demand;
    err = EN_getnodevalue(m_proj, index, EN_BASEDEMAND, &base_demand);
    epanetError(err);
    
    double pressure;
    err = EN_getnodevalue(m_proj, index, EN_PRESSURE, &pressure);
    epanetError(err);
    
    double quality;
    err = EN_getnodevalue(m_proj, index, EN_QUALITY, &quality);
    epanetError(err);
    
    double actual_demand;
    err = EN_getnodevalue(m_proj, index, EN_DEMAND, &actual_demand);
    epanetError(err);
    
    double total_head;
    err = EN_getnodevalue(m_proj, index, EN_HEAD, &total_head);
    epanetError(err);
    
    double x_coord,  y_coord;
    err = EN_getcoord(m_proj, index, &x_coord, &y_coord);
    epanetError(err);
    
    HydJunction* newJunction = m_hydNet->findJunction(hid);
    
    if (newJunction == nullptr)
    {
        newJunction = new HydJunction(hid);
        newJunction->setCoords(HydVertex(x_coord, y_coord));
        newJunction->setBaseElevation(base_elevation);
        newJunction->setBaseDemand(base_demand);
        
        //newJunction->setPressure(std::vector<HydFloat>(1,pressure));
        //newJunction->setQuality(std::vector<HydFloat>(1,quality));
        //newJunction->setActualDemand(std::vector<HydFloat>(1,actual_demand));
       // newJunction->setTotalHead(std::vector<HydFloat>(1,total_head));
        
        newJunction->setIndex(index);
        
        m_hydNet->addComponent(newJunction);
        
        if (m_logger.level() >= 3)
        {
            Message txt(1);
            txt << "Added junction " << hid;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        
        return true;
    }
    return false;
}


//
// link types
//

bool
HydEPANET::addPipe(int index, const std::string&  hid)
{
    int err;
    
    double length;
    err = EN_getlinkvalue(m_proj, index, EN_LENGTH, &length);
    epanetError(err);

    double roughness;
    err = EN_getlinkvalue(m_proj, index, EN_ROUGHNESS, &roughness);
    epanetError(err);

    double initial_status;
    err = EN_getlinkvalue(m_proj, index, EN_INITSTATUS, &initial_status);
    epanetError(err);

    double diameter;
    err = EN_getlinkvalue(m_proj, index, EN_DIAMETER, &diameter);
    epanetError(err);

    int from_node_index, to_node_index; 
    err = EN_getlinknodes(m_proj, index, &from_node_index, &to_node_index);
    epanetError(err);
    
    char from_node_id[64];
    err = EN_getnodeid(m_proj, from_node_index, from_node_id);
    epanetError(err);

    char to_node_id[64];
    err = EN_getnodeid(m_proj, to_node_index, to_node_id);
    epanetError(err);

    double flow;
    err = EN_getlinkvalue(m_proj, index, EN_FLOW, &flow);
    epanetError(err);
    
    double velocity;
    err = EN_getlinkvalue(m_proj, index, EN_VELOCITY, &velocity);
    epanetError(err);
    
    double headloss;
    err = EN_getlinkvalue(m_proj, index, EN_HEADLOSS, &headloss);
    epanetError(err);
    
    double status;
    err = EN_getlinkvalue(m_proj, index, EN_STATUS, &status);
    epanetError(err);
    
    double bulkcoef;
    err = EN_getlinkvalue(m_proj, index, EN_KBULK, &bulkcoef);
    epanetError(err);

    double wallcoef;
    err = EN_getlinkvalue(m_proj, index, EN_KWALL, &wallcoef);
    epanetError(err);
    
    // links can have several vertices
    int vertex_count;
    err = EN_getvertexcount(m_proj, index, &vertex_count);
    epanetError(err);
    
    std::vector<HydVertex> vertices;
    for (int i = 0; i < vertex_count; ++i)
    {
        double x_coord,  y_coord;
        err = EN_getvertex(m_proj, index, i+1, &x_coord, &y_coord);
        epanetError(err);
        vertices.push_back( HydVertex(x_coord, y_coord) );
    }
    
    HydPipe* newPipe = m_hydNet->findPipe(hid);

    if (newPipe == nullptr)
    { 
        newPipe = new HydPipe(hid);
        newPipe->setFromNodeID(std::string(from_node_id));
        newPipe->setToNodeID(std::string(to_node_id));
        newPipe->setDiameter(diameter);
        newPipe->setLength(length);
        newPipe->setRoughness(roughness);
        newPipe->setCoords(vertices);
        newPipe->setInitialStatus((initial_status > 0.0) ? PipeStatus::Open : PipeStatus::Closed);
        
        newPipe->setFlow(std::vector<HydFloat>(1,flow));
        newPipe->setVelocity(std::vector<HydFloat>(1,velocity));
        newPipe->setUnitHeadLoss(std::vector<HydFloat>(1,headloss));
        
        newPipe->setStatus((status > 0.0) ? LinkStatusOption::Open : LinkStatusOption::Closed);
        newPipe->setBulkCoeff(bulkcoef);
        newPipe->setWallCoeff(wallcoef);
        
        newPipe->setIndex(index);

        m_hydNet->addComponent(newPipe);
        
        if (m_logger.level() >= 3)
        {
            Message txt(1);
            txt << "Added pipe " << hid;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        
        return true;
    }
    
    return false;
}



bool
HydEPANET::addPump(int index, const std::string& hid)
{
    int err;
    
    int from_node_index, to_node_index; 
    err = EN_getlinknodes(m_proj, index, &from_node_index, &to_node_index);
    epanetError(err);
    
    char from_node_id[64];
    err = EN_getnodeid(m_proj, from_node_index, from_node_id);
    epanetError(err);

    char to_node_id[64];
    err = EN_getnodeid(m_proj, to_node_index, to_node_id);
    epanetError(err);
    
    double initial_status;
    err = EN_getlinkvalue(m_proj, index, EN_INITSTATUS, &initial_status);
    epanetError(err);

    // links can have several vertices
    int vertex_count;
    err = EN_getvertexcount(m_proj, index, &vertex_count);
    epanetError(err);
    
    std::vector<HydVertex> vertices;
    for (int i = 0; i < vertex_count; ++i)
    {
        double x_coord,  y_coord;
        err = EN_getvertex(m_proj, index, i+1, &x_coord, &y_coord);
        epanetError(err);
        vertices.push_back( HydVertex(x_coord, y_coord) );
    }
    
    HydPump* newPump = m_hydNet->findPump(hid);

    if (newPump == nullptr)
    {
        newPump = new HydPump(hid);
        newPump->setFromNode(from_node_index);
        newPump->setFromNodeID(from_node_id);
        newPump->setToNode(to_node_index);
        newPump->setToNodeID(to_node_id);
        newPump->setCoords(vertices);
        newPump->setInitialStatus((initial_status > 0.0) ? LinkStatusOption::Open : LinkStatusOption::Closed);
        newPump->setIndex(index);
        
        m_hydNet->addComponent(newPump);
        
        if (m_logger.level() >= 3)
        {
            Message txt(1);
            txt << "Added pump " << hid;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        
        return true;

    }
    return false;
}

bool
HydEPANET::addValve(int index, const std::string& hid )
{
    int err;
    
    double diameter;
    err = EN_getlinkvalue(m_proj, index, EN_DIAMETER, &diameter);
    epanetError(err);
    
    int from_node_index, to_node_index; 
    err = EN_getlinknodes(m_proj, index, &from_node_index, &to_node_index);
    epanetError(err);
    
    char from_node_id[64];
    err = EN_getnodeid(m_proj, from_node_index, from_node_id);
    epanetError(err);

    char to_node_id[64];
    err = EN_getnodeid(m_proj, to_node_index, to_node_id);
    epanetError(err);

    int link_type;
    err = EN_getlinktype(m_proj, index, &link_type);
    epanetError(err);
    
    ValveType vType;
    switch (link_type)
    {
        case LinkType::PRV:
            vType = ValveType::PRV;
            break;
        case LinkType::PSV:
            vType = ValveType::PSV;
            break;
        case LinkType::PBV:
            vType = ValveType::PBV;
            break;
        case LinkType::FCV:
            vType = ValveType::FCV;
            break;
        case LinkType::TCV:
            vType = ValveType::TCV;
            break;
        case LinkType::GPV:
            vType = ValveType::GPV;
            break;
        default:
            std::cout << "Valve has unknown type " << hid << ". Setting to GPV..." << std::endl;
            vType = ValveType::GPV;
            break;
    }
    
    // links can have several vertices
    int vertex_count;
    err = EN_getvertexcount(m_proj, index, &vertex_count);
    epanetError(err);
    
    std::vector<HydVertex> vertices;
    for (int i = 0; i < vertex_count; ++i)
    {
        double x_coord,  y_coord;
        err = EN_getvertex(m_proj, index, i+1, &x_coord, &y_coord);
        epanetError(err);
        vertices.push_back( HydVertex(x_coord, y_coord) );
    }
    
    HydValve* newValve = m_hydNet->findValve(hid);

    if (newValve == nullptr)
    {
        newValve = new HydValve(hid);
        newValve->setFromNode(from_node_index);
        newValve->setFromNodeID(from_node_id);
        newValve->setToNode(to_node_index);
        newValve->setToNodeID(to_node_id);
        newValve->setDiameter(diameter);
        newValve->setType(vType);
        newValve->setCoords(vertices);
        newValve->setIndex(index);

        m_hydNet->addComponent(newValve);
        
        if (m_logger.level() >= 3)
        {
            Message txt(1);
            txt << "Added valve " << hid;
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        
        return true;
    }
    return false;
}

