/* DBConfig 10/01/2021

 $$$$$$$$$$$%%$$$$$$$
 $   DBConfig.cpp   $
 $$$$$$$$$%%$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 
*/

#ifndef __DBCONFIG_H__
#include "DBConfig.h"
#endif

#include <fstream>


std::ostream&
operator<<(std::ostream& ostr, const DBConfig& c)
{
    ostr << c.user() << ' ' << c.dbname() << ' ' << c.passwd() << ' '  << c.host() << ' '  << c.conns() << '\n';
    return ostr;
}


std::istream&
operator>>(std::istream& istr, DBConfig &c)
{
    int conns = 1;
    std::string dbUser;
    std::string dbName;
    std::string dbPasswd;
    std::string dbHost;  

    istr >> dbUser >> dbName >> dbPasswd >> dbHost >> conns;
    
    c.user(dbUser);
    c.dbname(dbName);
    c.passwd(dbPasswd);
    c.host(dbHost);
    c.conns(conns);
    
    return istr;
}
 
bool
DBConfig::load( const std::string& file_name )
{

    std::ifstream from(file_name);
   
    if (from)
    {
        from >>  m_dbUser;
        from >>  m_dbName;
        from >>  m_dbPasswd;
        from >>  m_dbHost;  
   
        from.close();
        
        return true;
    }
    
    return false;
}
