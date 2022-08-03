/* HydTypes 21/02/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydTypes.cpp   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#include <iostream>

std::string 
to_hydstring( HydType h )
{
    switch(h)
    {
        case HydType::Pipe:      return "HydPipe";  break;
        case HydType::Pump:      return "HydPump";  break;
        case HydType::Valve:     return "HydValve"; break;
        case HydType::Tank:      return "HydTank";   break;
        case HydType::Reservoir: return "HydReservoir";   break;
        case HydType::Junction:  return "HydJunction";   break;
        default:
            std::cout << "unknown HydType in to_hydstring" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return std::string();
}

std::string 
to_hydstring( LinkStatusOption h )
{
    switch(h)
    {
        case LinkStatusOption::Open:    return "Open"; break;
        case LinkStatusOption::Closed:  return "Closed"; break;
            
        default:
            std::cout << "unknown LinkStatusOption in to_hydstring" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return std::string();
}

std::string 
to_hydstring( PipeStatus h )
{
    switch(h)
    {
        case PipeStatus::Open:    return "Open"; break;
        case PipeStatus::Closed:  return "Closed"; break;
        case PipeStatus::CV:      return "CV"; break;
            
        default:
            std::cout << "unknown PipeStatus in to_hydstring" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return std::string();
}


std::string 
to_hydstring( ValveType h )
{
    switch(h)
    {
        case ValveType::PRV:  return "PRV";  break;
        case ValveType::PSV:  return "PSV";  break;
        case ValveType::PBV:  return "PBV";  break;
        case ValveType::FCV:  return "FCV";  break;
        case ValveType::TCV:  return "TCV";  break;
        case ValveType::GPV:  return "GPV";  break;
            
        default:
            std::cout << "unknown ValveType in to_hydstring" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return std::string();
}

std::string 
to_hydstring( ValveFixedStatus h )
{
    switch(h)
    {
        case ValveFixedStatus::None:    return "None"; break;
        case ValveFixedStatus::Open:    return "Open"; break;
        case ValveFixedStatus::Closed:  return "Closed"; break;

        default:
            std::cout << "unknown ValveFixedStatus in to_hydstring" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return std::string();
}


std::string 
to_hydstring( MixingModel h )
{
    switch(h)
    {
        case MixingModel::Mixed:   return "Mixed"; break;
        case MixingModel::TwoComp: return "TwoComp"; break;
        case MixingModel::FIFO:    return "FIFO"; break;
        case MixingModel::LIFO:    return "LIFO"; break;

        default:
            std::cout << "unknown ValveFixedStatus in to_hydstring" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return std::string();
}
