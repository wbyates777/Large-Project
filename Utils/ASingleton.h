/* Singleton Factory Template 15/08/2009.

		$$$$$$$$$$$$$%%$$$$$$$$$$$$$$
		$   ASingleton.h - header   $
		$$$$$$$$$$$$$$$%%$$$$$$$$$$$$

   by W.B. Yates 
   Copyright 2009. All rights reserved.
   
   History: Singleton template
   
    Eg.
	

	the<std::string>::initialise();
	
	std::string &t3 = the<std::string>::getClass();
	t3 = "bill";
	std::string& t4 = the<std::string>::getClass();
	cout << t4 << endl;

*/

#ifndef __SINGLETON_H__
#define __SINGLETON_H__



#include <map>
#include <mutex>


template <typename T>
class Singleton
{
    
public:
    
	static void 
	initialise( void ); // initialise the singleton; referenced counted and thread safe; you must call this at least once before using this class
	
	static void 
	destroy( void ); // destroy the singleton when references are zero; thread safe
	
	static T*
	getClass(void) { return m_instance; }

	static int
	refCount(void) { int retVal = 0; m_classMutex.lock(); retVal =  m_count; m_classMutex.unlock(); return retVal; }

private:

	// constructors, destructiors, and copy hidden from user and not defined
    Singleton( void )=delete;

    ~Singleton( void )=delete;
	
    Singleton( const Singleton<T>& )=delete;
    
	Singleton<T>& 
    operator=(const Singleton<T>& )=delete;
	//
    
	static T *m_instance;
	static  std::mutex m_classMutex;
	static int m_count;
};


template <typename T>
T* Singleton<T>::m_instance = 0;

template <typename T>
std::mutex Singleton<T>::m_classMutex;

template <typename T>
int Singleton<T>::m_count = 0;



template <typename T>
void
Singleton<T>::initialise( void )
{
	m_classMutex.lock();
	
	if (m_instance == nullptr)
	{
	    m_instance = new T();
	}
	++m_count;
	
	m_classMutex.unlock();
}

template <typename T>
void
Singleton<T>::destroy( void )
{
	m_classMutex.lock();

	if (m_count > 0)
	{
		--m_count;
	
		if (m_instance && m_count == 0)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}
	
	m_classMutex.unlock();
}

#define the Singleton 



#endif








