/* HydTypes 21/02/2021

 $$$$$$$$$$$$$$$$$$
 $   HydTypes.h   $
 $$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 General enums and strings for HydNetworks and their components
 
*/


#ifndef __HYDTYPES_H__
#define __HYDTYPES_H__

#include <string>


typedef float HydFloat;

// a diameter that implies that a pipe is 'switched off' -  i.e. used in New York Tunnels
const HydFloat PIPE_OFF = 0.001; 

enum class HydType 
{
    Junction,
    Pipe,
    Pump,
    Reservoir,
    Tank,
    Valve
};

enum class LinkStatusOption
{
    Open,
    Closed
};

enum class PipeStatus
{
    Open,
    Closed,
    CV
};

enum class ValveType
{
    PRV,
    PSV,
    PBV,
    FCV,
    TCV,
    GPV
};

enum class ValveFixedStatus
{
    None,
    Open,
    Closed
};

enum class MixingModel
{
    Mixed,
    TwoComp,
    FIFO,
    LIFO
};

std::string to_hydstring( HydType h );
std::string to_hydstring( LinkStatusOption h );
std::string to_hydstring( PipeStatus h );
std::string to_hydstring( ValveType h );
std::string to_hydstring( ValveFixedStatus h );
std::string to_hydstring( MixingModel h );

#endif


