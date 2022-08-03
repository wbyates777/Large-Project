/* A simple Logger class 14/09/07
 
 $$$$$$$$$$$$$$$$$$$$$$$$$
 $   Logger.cpp - code   $
 $$$$$$$$$$$$$$$$$$$$$$$$$
 
 Copyright 2007 (c) W.B. Yates. All rights reserved
 History: Despite the mutex used in this class, it is NOT thread safe.
 The mutex is soley used to synchronise access to the output stream
 
 Usage: 
 First in main or somewhere before you use the Logger class   
 
 Logger::setLogLevel("Network",2);
 
 then in the class(es) where you want logging enabled, define a mutable 
 Logger m_logger object, then somewhere in your code, add the line
	
 m_logger.getLogLevel("Network");
 
 The m_logger object will then lookup the log level associated with 
 "Network" which in this case is 2 and then proceed to
 log all messages with a log level of 1 or 2 but no greater. 
 If no association exists log level defaults to 0, that is, no output.
 
 Do not use negative log levels
 */

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <time.h>
#include <fstream>

std::mutex Logger::m_classMutex;
std::map<std::string,unsigned int> Logger::m_logLevels;


std::ostream&
operator<<( std::ostream& ostr, const Message& m )
{
    ostr << m.m_txt.str();
    return ostr;
}

Logger::~Logger( void ) 
{
    m_flush = false;
    m_logLevel = 0; // should I flush here?
    m_size = 0;
    m_id.clear();
    m_logText.str("");
}

void 
Logger::clear( void ) 
{
    if (m_size)
    {
        m_logText.str("");
        m_size = 0;	
    }
}

void
Logger::getLogLevel( std::string_view logId )
{
	m_id = logId;
	
	m_classMutex.lock();
	std::map<std::string,unsigned int>::const_iterator idx = m_logLevels.find( m_id );
	
	if (idx != m_logLevels.end())
	{
		m_logLevel = idx->second;
	}
	m_classMutex.unlock();
}

// void 
// Logger::logMessage(const char* file, const int line, const Message& txt)
void  
Logger::logMessage( const Message& txt )
{
    if (m_logLevel == 0)
        return;
    
    if (txt.level() <= m_logLevel)
    {
        m_size += static_cast<short>(txt.length());
        m_logText.width(10);
        //m_logText << file << '\t' << line << '\t';
        m_logText << timeStamp() << '\t'  << m_id << '\t' << txt << '\n';		   
    }
}

void
Logger::appendLog( const Logger& l ) 
{
	   if (m_logLevel == 0)
           return;
    
	   m_size += static_cast<short>(l.m_size);
	   m_logText << l.m_logText.str();		   
}

void
Logger::flush( void ) 
{
   if (m_flush && m_size)
   {
       m_classMutex.lock();
       std::cout << m_logText.str();
       std::cout.flush();
       m_logText.str("");
       m_size = 0;
       m_classMutex.unlock();	
   }
}

void
Logger::setLogLevel( std::string_view logId, unsigned int level )
{
    m_classMutex.lock();
    m_logLevels[std::string(logId)] = level;
    m_classMutex.unlock();
}

void
Logger::loadLogLevels( const std::string& name )
{
	m_classMutex.lock();
   
    std::ifstream from(name);
   
    if (from)
    {
       std::string logId;
       int logLevel;

       m_logLevels.clear();
        
       while (!from.eof())
       {
            from >> logId;
            from >> logLevel;
            m_logLevels.insert( std::map<std::string,int>::value_type(logId,logLevel) );
       }
        
       from.close();
    }
     
    m_classMutex.unlock();
}

std::string
Logger::timeStamp( void )
{
  time_t ltime;
  ::time(&ltime);
  struct tm t;

  ::localtime_r(&ltime, &t);

  char buff[20];
  sprintf(buff, "%02u/%02u/%4u %02u:%02u:%02u", (unsigned) t.tm_mday, 
                                                (unsigned) (t.tm_mon + 1), 
                                                (unsigned) t.tm_year + 1900, 
                                                (unsigned) t.tm_hour, 
                                                (unsigned) t.tm_min, 
                                                (unsigned) t.tm_sec);  
   return std::string(buff,19);
}





