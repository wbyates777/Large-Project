/* HydSQL 31/05/2021

 $$$$$$$$$$$$$$$$
 $   HydSQL.h   $
 $$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 Generate the SQL inserts to store a HydNetwork in a MySQL database (see HOWS_Schema.sql)
 
 Note when adding/removing fields remember to double check that the order of the inserted fields
 and the order of the table columns match
 i.e. for junction_values make sure the value 'comp.headDeficit()' is stored 
 in the database column 'head_deficit' and not, say, 'head_excess'
 Also check HydJSON is in sync too.
 
 TODO: Maybe add DB load???
 TODO: toSQL functions for tanks

*/


#ifndef __HYDSQL_H__
#define __HYDSQL_H__

#include <string>
#include <vector>
#include <sstream>
#include <functional>

#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif


class HydSQL
{
public:

    HydSQL( void ): m_BATCH_SIZE(1000) {}
    ~HydSQL( void )=default;

    std::vector<std::string> 
    toSQL(const std::string& userName, HydNetwork* hydNet);
    
    std::vector<std::string>
    clear( const std::string& userName, const std::string& networkName );
    
private:

    HydSQL( const HydSQL& )=delete;

    HydSQL&
    operator=( const HydSQL& )=delete;
    
    static std::string  
    toQuoted(const std::string& s) { return "'" + s + "'"; }
    
    // these are declared static so they can be passed easily to toSQL<>()
    static std::string
    valveSQL(HydSQL* data, const HydValve& comp);
    
    static std::string
    valveValuesSQL(HydSQL* data, const HydValve& comp);
    
    static std::string
    pumpSQL(HydSQL* data, const HydPump& comp);
    
    static std::string
    pumpValuesSQL(HydSQL* data, const HydPump& comp);
    
    static std::string
    pipeSQL(HydSQL* data, const HydPipe& comp);
    
    static std::string
    pipeValuesSQL(HydSQL* data, const HydPipe& comp);
    
    
    static std::string
    junctionSQL(HydSQL* data, const HydJunction& comp);
    
    static std::string
    junctionValuesSQL(HydSQL* data, const HydJunction& comp);
    
    static std::string
    reservoirSQL(HydSQL* data, const HydReservoir& comp);
    
    static std::string
    reservoirValuesSQL(HydSQL* data, const HydReservoir& comp);
    

    
    // this is a type alias template
    template <class T>
    using toSQLFunc = std::function<std::string(HydSQL* data, const T&)>;
    
    template<class T>
    std::vector<std::string> 
    toSQL(const std::vector<T*>& comp,  const std::string& table_name, const toSQLFunc<T>& to_sql)
    {
        std::vector<std::string> retVal;
        
        if (comp.empty())
            return retVal;
        
        std::ostringstream insert_seq;
        insert_seq << "insert into " + table_name + " values ";

        for (int i = 0; i < comp.size(); ++i) 
        { 
            if (((i+1) % m_BATCH_SIZE) == 0)
            {
                insert_seq << ";";
                retVal.push_back( insert_seq.str() );
                insert_seq.str("");
                insert_seq << "insert into " << table_name << " values ";
            }
            
            insert_seq << to_sql(this, *comp[i]);
      
            if (i < comp.size() -1 && ((i+1) % m_BATCH_SIZE) != m_BATCH_SIZE-1) 
                insert_seq << ',';
        }

        insert_seq << ";";
        retVal.push_back( insert_seq.str() );
        return retVal;
    }
    
    // a single insert statement should not contain more than m_BATCH_SIZE values/rows
    int m_BATCH_SIZE; 
    std::string m_networkName;
    std::string m_user;

};

#endif


