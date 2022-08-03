/* DBConfig 10/01/2021

 $$$$$$$$$$$$$$$$$$
 $   DBConfig.h   $
 $$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 TODO: improve error handling when loading/reading config data
 TODO: i.e. improve format using quoted strings
 
*/


#ifndef __DBCONFIG_H__
#define __DBCONFIG_H__

#include <string>
#include <iostream>

class DBConfig 
{
public:
    
    DBConfig( void ) : m_conns(1), m_dbUser(), m_dbName(), m_dbPasswd(), m_dbHost("localhost") {}
    DBConfig( int conns ) : m_conns(conns), m_dbUser(), m_dbName(), m_dbPasswd(), m_dbHost("localhost") {}
    DBConfig( const std::string& user, 
              const std::string& name, 
              const std::string& passwd, 
              const std::string& host = "localhost", 
              int conns = 1 ) : m_conns(conns), 
                                m_dbUser(user), 
                                m_dbName(name), 
                                m_dbPasswd(passwd),
                                m_dbHost(host) {}
    
    const std::string&
    user( void ) const { return m_dbUser; }
    
    void
    user( const std::string& n ) { m_dbUser = n; }
    
    const std::string&
    dbname( void ) const { return m_dbName; }
    
    void
    dbname( const std::string& n ) { m_dbName = n; }
    
    const std::string&
    host( void ) const { return m_dbHost; }
     
    void
    host( const std::string& n ) { m_dbHost = n; }
    
    const std::string&
    passwd( void ) const { return m_dbPasswd; }
    
    void
    passwd( const std::string& n ) { m_dbPasswd = n; }
    
    int
    conns( void ) const { return m_conns; }
    
    void
    conns( int c ) { m_conns = c; }
    

    bool
    load( const std::string& file_name = "dbConfig.txt" );
    
private:
    
    int    m_conns;
    std::string m_dbUser;
    std::string m_dbName;
    std::string m_dbPasswd;
    std::string m_dbHost;  
    
};


std::ostream&
operator<<(std::ostream& ostr, const DBConfig &conf);

std::istream&
operator>>(std::istream& istr, DBConfig &conf);

#endif


