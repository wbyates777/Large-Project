/* HydJSON 17/02/2021

 $$$$$$$$$$$$$$$$$
 $   HydJSON.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 Convert a HydNetwork or HydScenarios to and from JSON - we can also save/load as JSON
 The user owns the HydNetwork memory
 
 Also convert HOWSServer messages to/from JSON
 
 see https://json.nlohmann.me/
 
 Note this code depends on *every* HydNetwork component having an "id" field
 
 Note for binary streaming the main change is dealing with  std::vector<uint8_t> instead of std::strings
 
 Example.
 
 std::vector<uint8_t> v1 = {0x1b, 0x00, 0x00, 0x00, 0x08, 0x63, 0x6f, 0x6d,
                            0x70, 0x61, 0x63, 0x74, 0x00, 0x01, 0x10, 0x73,
                            0x63, 0x68, 0x65, 0x6d, 0x61, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00 };

 json j = json::from_bson(v1);
 std::vector<uint8_t> v2 = json::to_bson(j);
 v1 == v2 is true
 
 TODO: it might be a good idea to seperate out these functions into several classes
 TODO: i.e. HydNetworkJSON, MessagesJSON, HydScenariosJSON
 
*/


#ifndef __HYDJSON_H__
#define __HYDJSON_H__


#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif

#ifndef __HYDSCENARIOS_H__
#include "HydScenarios.h"
#endif


#ifndef __HOWSMESSAGES_H__
#include "HOWSMessages.h"
#endif


#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#define JSON_DIAGNOSTICS false
#include "json.hpp"
#endif

#include <string>
#include <vector>
#include <map>


//

class HydJSON
{
public:

    //
    // HydNetwork - pipes, juctions, tanks, pumps etc
    //
    
    static HydNetwork*
    loadNetwork( const std::string& file_name );
    
    static void
    saveNetwork( const std::string& file_name, HydNetwork *hydNet, int indent = 3 );
    
    static std::string
    toNetworkJSON( const HydNetwork *hydNet, int indent = 0 );
    
    static HydNetwork*
    fromNetworkJSON( const std::string& json_string );
    
    static void
    updateNetworkJSON( const std::string& json_string, HydNetwork* hydNet );

    //
    // LookAhead data - is stored as part of HydNetwork, also used as message
    //
    
    static std::string
    toLookAheadJSON( const HydLookAhead& la, int indent = 0 );
    
    static HydLookAhead
    fromLookAheadJSON( const std::string& json_string );
    
    //
    // HydScenarios - contains constraints, decision options, decision varaibles
    //
     
    static HydScenarios
    loadScenarios( const std::string& file_name );
    
    static void
    saveScenarios( const std::string& file_name, const HydScenarios& prob, int indent = 3 );
    
    static std::string
    toScenariosJSON( const HydScenarios& prob, int indent = 0 );
    
    static HydScenarios
    fromScenariosJSON( const std::string& json_string );

    //
    // Client/Server Messages
    //
    
    static std::string
    toLoadMessageJSON( const LoadMessage& msg, int indent = 0 );
    
    static LoadMessage
    fromLoadMessageJSON( const std::string& json_string );
    
    static std::string
    toRunMessageJSON( const RunMessage& msg, int indent = 0 );
    
    static RunMessage
    fromRunMessageJSON( const std::string& json_string );
    
    static std::string
    toDBMessageJSON( const DBMessage& msg, int indent = 0 );
    
    static DBMessage
    fromDBMessageJSON( const std::string& json_string );
    
    static std::string
    toOptimiseMessageJSON( const OptimiseMessage& msg, int indent = 0 );
    
    static OptimiseMessage
    fromOptimiseMessageJSON( const std::string& json_string );
    
    static std::string
    toLookAheadMessageJSON( const LookAheadMessage& msg, int indent = 0 );
    
    static LookAheadMessage
    fromLookAheadMessageJSON( const std::string& json_string );
    
    static std::string
    toChangeMessageJSON( const ChangeMessage& msg, int indent = 0 );
    
    static ChangeMessage
    fromChangeMessageJSON( const std::string& json_string );
    
    
private:

    HydJSON( void )=delete;
    ~HydJSON( void )=delete;
    
    static bool 
    buildNetwork( const nlohmann::json& j, HydNetwork* hydNet );

    template<class T>
    static bool 
    addComponent( const std::string& jname, HydType ctype, const nlohmann::json& json, HydNetwork* hydNet )
    {
        if (json.find(jname) != json.end())
        {
            auto comp = json.at( jname );
            for (auto i = comp.begin(); i != comp.end(); ++i)
            {
                // Note assumption here that every component has field "id"
                std::string ID = i->at("id").get<std::string>();
                T* newComp = static_cast<T*>(hydNet->findComponent(ID, ctype));
                
                // if component does not exist add to network, else update contents (which is faster)
                if (newComp == nullptr)
                    hydNet->addComponent(new T(i->get<T>()));
                else *newComp = i->get<T>();
            }
            return true;
        }
        return false;
    }
    
    template<class T>
    static std::string
    toMessageJSON( const T& msg, int indent = 0 )
    {
        nlohmann::json json = msg;
        return json.dump(indent);
    }
    
    template<class T>
    static T
    fromMessageJSON( const std::string& json_string )
    {
        nlohmann::json json; 
        try
        {
            json = nlohmann::json::parse(json_string);
            return json.get<T>();
        }
        catch (nlohmann::json::parse_error& e)
        {
            std::cout << e.what() << std::endl;
        }
        return T();
    }
    
};


#endif


