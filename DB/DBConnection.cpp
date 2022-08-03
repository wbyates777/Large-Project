/* DBConnection 07/01/08

        $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        $   DBConnection.cpp - header   $
        $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

   by W.B. Yates 
   History: Handles DB access for MySQL

   Tested with MySQL 8.0.23
 
*/

#ifndef __DBCONNECTION_H__
#include "DBConnection.h"
#endif

#ifdef WIN32
#include <windows.h>
#endif

// note if you get a a "long long not supported in C++" error simply turn off "pedantic erros" in Xcode

extern "C"
{
#include <mysql.h>
}

#include <iostream>
 


DBConnection::DBConnection(  const DBConfig& dbconfig  ):m_conn(0), m_resSet(0)
{
	 open( dbconfig );
}

DBConnection::DBConnection( void ):m_conn(0), m_resSet(0)
{
}

DBConnection::~DBConnection( void )
{
    mysql_close(m_conn);
    m_conn = nullptr;
}

bool
DBConnection::open(  const DBConfig& dbconfig  )
{
	if (m_conn)
	{
	    mysql_close(m_conn);
	}
	
	m_conn = mysql_init(NULL);
    
    // mysql_ssl_set() // use secure sockets
    // http://www.percona.com/blog/2013/06/22/setting-up-mysql-ssl-and-secure-connections/
    // http://dev.mysql.com/doc/refman/5.5/en/creating-ssl-certs.html
    
    
    // host/IP,   user,         passwd,         db,      port, unix socket, clientflag
	if (!mysql_real_connect(m_conn, dbconfig.host().c_str(), dbconfig.user().c_str(), dbconfig.passwd().c_str(), dbconfig.dbname().c_str(), 0, NULL, 0))
	{
        m_conn = nullptr; // it should be nullptr but just in case
		std::cout << "DBConnection: Could not connect to database " << dbconfig << std::endl;
        return false;
	}
	
	if (!mysql_thread_safe())
	{
        m_conn = nullptr;
	    std::cout << "DBConnection: MySQL not thread safe" << std::endl;
        return false;
	}

	bool reconnect = 1;
	mysql_options(m_conn, MYSQL_OPT_RECONNECT, &reconnect);
    return true;
}



void
DBConnection::close( void )
{
    if (m_conn)
    {
        mysql_close(m_conn);
        m_conn = nullptr;
    }
}

std::string
DBConnection::getErrorMsg( void ) const
{
	return std::string( mysql_error(m_conn) );
}

long
DBConnection::rowsAffected(void)
{
    return mysql_affected_rows(m_conn);
}

bool
DBConnection::sqlQuery( const std::string& qry )
{
	bool retVal = !mysql_query(m_conn, qry.c_str());
	   
	m_resSet = mysql_store_result(m_conn);
	
	if (m_resSet)
	{
		mysql_free_result(m_resSet);
	}	
	
	return retVal;
}

bool
DBConnection::sqlQuery( const std::string& qry, ResultSet& resSet )
{
	MYSQL_ROW row;
	int i, rowIdx = 0;
	unsigned long *lengths;
	
	bool retVal = !mysql_query(m_conn, qry.c_str());
	
	if (!retVal)
		return retVal;
	   
	m_resSet = mysql_store_result(m_conn);
	resSet.clear();

	if (m_resSet)
	{
		resSet.resize( m_resSet->row_count, std::vector<std::string>(m_resSet->field_count) );
        const int fieldCount = m_resSet->field_count;		
		
        while ((row = mysql_fetch_row( m_resSet )))
		{
			lengths = mysql_fetch_lengths(m_resSet);

			for (i = 0; i < fieldCount; ++i)
			{
                // slightly faster than insert
				resSet[rowIdx][i].append(row[i], lengths[i]);
                //resSet[rowIdx][i].insert(resSet[rowIdx][i].begin(), row[i], row[i] + lengths[i]);
			}
			++rowIdx;
		}
		mysql_free_result(m_resSet);
	}	
	
	return retVal;
}

std::string
DBConnection::toBlob(const std::vector<char>& from ) const
{	
	std::vector<char> blob((from.size() * 2) + 1);
	size_t len = mysql_real_escape_string(m_conn, &blob[0], &from[0], from.size());
	
    return std::string(&blob[0],len);	
}

bool
DBConnection::sqlQuery( const std::string& qry, BinaryResultSet& resSet )
{
	MYSQL_ROW row;
	int i, rowIdx = 0;
	unsigned long *lengths;
	
	bool retVal = !mysql_query(m_conn, qry.c_str());
	
	if (!retVal)
		return retVal;
	
	m_resSet = mysql_store_result(m_conn);
	resSet.clear();
	
	if (m_resSet)
	{
		resSet.resize(m_resSet->row_count, std::vector< std::vector<char> >(m_resSet->field_count, std::vector<char>()));
		const int fieldCount = m_resSet->field_count;
		
        while ((row = mysql_fetch_row( m_resSet )))
		{
			lengths = mysql_fetch_lengths(m_resSet);
            
			for (i = 0; i < fieldCount; ++i)
			{										
				resSet[rowIdx][i].insert(resSet[rowIdx][i].begin(), row[i], row[i] + lengths[i]);
			}
			++rowIdx;
		}
		mysql_free_result(m_resSet);
	}	
	
	return retVal;
}






//




