/* A simple Logger class 14/09/07

		$$$$$$$$$$$$$$$$$$$$$$$$$
		$   Logger.h - header   $
		$$$$$$$$$$$$$$$$$$$$$$$$$

   Copyright 2007 (c) W.B. Yates. All rights reserved
   History: Despite the mutex used in this class, it is NOT thread safe.
   The mutex is soley used to synchronise access to the output stream

   Usage: 
    First in main or somewhere before you use the Logger class   
   	   
	   Logger::setLogLevel("Network",2);
   
    then in the class(es) where you want logging enabled, define a mutable Logger m_logger object, then somewhere in your code, add the line
	
	    m_logger.getLogLevel("Network");
		
    The m_logger object will then lookup the log level associated with "Network" which in this case is 2 and then proceed to
	log all messages with a log level of 1 or 2 but no greater. If no association exists log level defaults to 0, that is, no output.
    Do not use negative log levels
*/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream> 
#include <string>

#include <mutex>
#include <map>



//
// level = 0-255
// level 0 = ignore
//
class Message
{
public:
    
	Message( void ): m_level(static_cast<unsigned char>(1)), m_txt() {}
	
	explicit Message( int lev ): m_level(static_cast<unsigned char>(lev)), m_txt() {}

    explicit Message( std::string_view s, int lev = 1 ): m_level(static_cast<unsigned char>(lev)), m_txt()    
    {
        m_txt << s;
    }
    
	//explicit Message( const std::string& s, int lev = 1 ): m_level(static_cast<unsigned char>(lev)), m_txt()	
	//{
	//    m_txt << s;
	//}
	
    //explicit Message( const char* s, int lev = 1 ): m_level(static_cast<unsigned char>(lev)), m_txt()
	//{
	//    m_txt << s;
	//}
	
    virtual ~Message( void )=default;

    virtual void
	clear( void )
	{
		m_txt.str("");
	}
    
	template<typename T>
	Message&
	operator<<(const T& s)
	{
		m_txt << s;
		return *this;
	}
	
    friend std::ostream&
	operator<<(std::ostream& ostr, const Message& m);
	
    int length( void ) const { return static_cast<int>(m_txt.str().length()); }
	int level( void ) const { return m_level; }
	void level( unsigned int lev ) { m_level = static_cast<unsigned char>(lev); }

	std::string
	toString( void ) const { return m_txt.str(); }
	
protected:
	
	unsigned char m_level;
	std::ostringstream m_txt;

};

std::ostream&
operator<<( std::ostream& ostr, const Message& m );


class Logger
{
public:
    
    Logger( void ): m_flush(true), m_logLevel(0), m_size(0), m_id("Logger"), m_logText() {}
	
    ~Logger( void );
    
    void 
    clear( void );
    
    operator std::string( void ) const { return m_logText.str(); }	
		
	void
	getLogLevel( std::string_view logId );
	
	void 
    level( unsigned int ll ) { m_logLevel = static_cast<char>(ll); }
    
	int 
    level( void ) const { return m_logLevel; }
	 
	void 
    logMessage( /* const char* file, const int line, const Message& txt */ const Message& txt );
	
	void
    appendLog( const Logger& l );
    
	void
	setFlush( const bool b ) { m_flush = b; }
	
	void
    flush( void );
    
    static void
    loadLogLevels( const std::string& name );
    
    static void
    setLogLevel( std::string_view  logId, unsigned int level );

private:

	// disable copy
    Logger(const Logger&)=delete;
    Logger& operator=(const Logger&)=delete;
	
	std::string
	timeStamp( void );
	
	char   m_flush; // should we flush output to stream
	char   m_logLevel;
	short  m_size;
	std::string m_id; // this Logger object is identified by m_id
	std::ostringstream m_logText;
	
    static std::mutex m_classMutex;
    static std::map<std::string,unsigned int> m_logLevels; // contains the log level for a particular m_id
};

//#define logMsg(x) logMessage(__FILE__,__LINE__,x)
#define logMsg(x) logMessage(x)

#endif





