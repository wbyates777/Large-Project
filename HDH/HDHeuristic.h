/* HDHeuristic 02/03/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   HDHeuristic.h   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 This class can be used in a stand alone manner for training and saving random forests
 see the commented out main() in the .cpp file
 
 It also integrates with SSHH as a heuristic - in this case set 
 "sessions" = [] in the config file, and HDHeuristic will load
 a trained forest

 TODO: add a regression ranger::forest class
 
 
 See 
 
 "Generating heuristics to mimic experts in water distribution network optimisation"
 D. J. Walker and  M. B. Johns and  E. C.  Keedwell and D. Savic,
 WDSA / CCWI 2018 Joint Conference, Morgan Kaufmann, 2018
 
*/

 
#ifndef __HDHEURISTIC_H__
#define __HDHEURISTIC_H__


#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#ifndef __HDHCONFIG_H__
#include "HDHConfig.h"
#endif

#ifndef __HYDFOREST_H__
#include "HydForest.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <string>
#include <vector>
#include <map>
#include <iostream>

class HydNetwork;
class HydEPANET;
class HydSession;

class HDHeuristic 
{
public:

    HDHeuristic( void );
    ~HDHeuristic( void );

    void
    init( HydEPANET* epanet = nullptr );
    

    void
    learn( void ); 
    
    bool
    predict( const std::string& pipe_id );
    
    void
    set( const HDHConfig& cfg );
    
    void 
    load( const std::string& name = "" );
    
    void 
    save( const std::string& name = "" );
    
private:

    inline HydFloat
    normalise( HydFloat value, HydFloat min, HydFloat max )
    {
        return (value - min) / (max -  min);
    }
    
    // run the hydraulic simulation - update m_hydNet
    std::vector<HydFloat>
    run( void );
    

    void
    process_interactions(const HydSession& session);
    
    void
    setPipesToMax( void );
    
    void
    setPipesToMin( void );

    enum HydNormVal { Velocity, Flow, Length, Influence, Diameter};
    
    std::vector<HydFloat>
    getNormValues(const std::string& pipe_id);
    
    HydFloat
    getNormPipeDiameter( const std::string& pipe_id, HydFloat diameter = -1.0 );
    
    std::pair<HydFloat,HydFloat>
    getNormUpDownNodeDeficit( const std::string& pipe_id );
    
    HydNetwork* m_hydNet;
    HydEPANET* m_epanet;

    int m_own_epanet;

    // training data - inputs and outputs (values are normalised)
    std::vector<std::vector<double>> m_processedInputData;
    std::vector<std::vector<double>> m_processedTargetData;
    
    HydForest m_rforest;
    HDHConfig m_config;
    
    mutable Logger m_logger;
};

#endif


