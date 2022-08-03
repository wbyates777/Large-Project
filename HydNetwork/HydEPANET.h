/* HydEPANET 17/02/2021

 $$$$$$$$$$$$$$$$$$$
 $   HydEPANET.h   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 This is an interface to EPANET - the intention is to hide EPANET details
 
 what is EPANET
 https://www.epa.gov/water-research/epanet
 
 EPANET user manual
 https://nepis.epa.gov/Adobe/PDF/P1007WWU.pdf
 
 EPANET programmer's guide
 http://epanet.info/wp-content/uploads/2012/10/toolkit_help.pdf

 EPANET code
 https://github.com/OpenWaterAnalytics/EPANET/wiki/
 
 This class loads an EPANET file and constructs the HydNetwork structure. 
 It also runs the simulation, updating HydNetwork

 NOTE in the original network.py on which this code is based "total_head" is set to "elevation"
      this will prevent division by zero errors in HydNetwork::getNodeMinMax()
      seed HydEPANET::addReservoir() for details
 
 TODO: updateNetwork(HydTank* tank)
 TODO: double check that all values that need to be set are being set - especially pumps, valves, tanks
 
*/


#ifndef __HYDEPANET_H__
#define __HYDEPANET_H__

#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <string>
#include <vector>
#include <map>


// defined in EPANET types.h
struct Project;

class HydEPANET
{
public:

    HydEPANET( void );
    ~HydEPANET( void );

    
    // load a new EPANET network and construct a HydNetwork - caller owns HydNetwork memory
    // subsequent calls will generate a new 'current' HydNetwork
    // only the current HydNetwork will be updated - see run() below
    HydNetwork*
    load( const std::string& name );
    
    // this will update the current HydNetwork
    void
    run(void) const;
    
    // the current HydNetwork 
    HydNetwork* 
    getHydNet( void ) const { return m_hydNet; }
    
    // the current EPANET project 
    Project* 
    getProject( void ) const  { return m_proj; }
    

    // direct access to EPANET node data
    double 
    pressure( int node_idx ) const;

    double 
    head( int node_idx ) const;
    //
    
    // direct access to EPANET link data
    double 
    diameter( int link_idx ) const;
    
    void 
    setDiameter( int link_idx, double diameter ) const;

    double 
    length( int link_idx ) const;

    LinkStatusOption 
    valveStatus( int link_idx ) const;
    
    void
    setValveStatus( int link_idx, LinkStatusOption status ) const;
    // 
    
private:

    HydEPANET( const HydEPANET& )=delete;

    HydEPANET&
    operator=( const HydEPANET& )=delete;
    
    bool
    epanetError( int error_code ) const;
    
    /// buld a HydNetwork from the .inp definition loaded in EPANET project
    void 
    buildNetwork( void );
    
    bool
    addReservoir(int index, const std::string& hid);
    
    bool
    addTank(int index, const std::string& hid);
    
    bool
    addJunction(int index, const std::string& hid);

    bool
    addPipe(int index, const std::string&  hid);
    
    bool
    addPump(int index, const std::string& hid);
    
    bool
    addValve(int index, const std::string& hid );
    ///
    
    /// update/add EPANET dynamic values to HydNetwork
    void
    updateNetwork( int scenIdx ) const;
    
    void
    updateNetwork(HydPipe* pipe) const;
    
    void
    updateNetwork(HydPump* pump) const;
    
    void
    updateNetwork(HydValve* valve) const;
    
    void
    updateNetwork(HydJunction* junction) const;
    
    void
    updateNetwork(HydTank* tank) const;
    
    void
    updateNetwork(HydReservoir* reservoir) const;
    ///

    Project    *m_proj;
    HydNetwork *m_hydNet;
    int m_epanetOpen;
    bool m_extended;
    mutable Logger m_logger;
};

#endif


