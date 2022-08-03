/* HOWSMessages 03/05/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HOWSMessages.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 These are definitions of the messages exchanged between client and server
 Also see HydJSON.cpp
 
 For the network definition see HydNetwork

*/


#ifndef __HOWSMESSAGES_H__
#define __HOWSMESSAGES_H__


#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#ifndef __HHTYPES_H__
#include "HHTypes.h"
#endif

#include <string>

const std::string _LOAD_       = "Load";
const std::string _RUN_        = "Run";
const std::string _DB_         = "DB";
const std::string _CHANGE_     = "Change";
const std::string _OPTIMISE_   = "Optimise";
const std::string _LOOKAHEAD_  = "LookAhead";

class BaseMessage
{
public:
    
    BaseMessage( void )=default;
    BaseMessage( const std::string& msgType ) : m_msgType(msgType), m_user("default") {}
    virtual ~BaseMessage( void )=default;
    
    const std::string& 
    msgType(void) const { return m_msgType; }
    
    void 
    msgType(const std::string& msgType) { m_msgType = msgType; }
    
    const std::string& 
    user(void) const { return m_user; }
    
    void 
    user(const std::string& user) { m_user = user; }
    
protected:

    std::string m_msgType; 
    std::string m_user;
};

class LoadMessage : public BaseMessage
{
public:

    LoadMessage( void ) : BaseMessage(_LOAD_), m_randomise(1) {}
    virtual ~LoadMessage( void ) override = default;
    
    const std::string& 
    name(void) const { return m_name; }
    
    void 
    name(const std::string& network_name) { m_name = network_name; }
    
    int 
    randomise(void) const { return m_randomise; }
    
    void 
    randomise( int r ) { m_randomise = r; }
    
protected:

    int m_randomise;
    std::string m_name; 

};

class RunMessage : public BaseMessage
{
public:

    RunMessage( void ) : BaseMessage(_RUN_) {}
    virtual ~RunMessage( void ) override = default;
    

protected:


};

class DBMessage : public BaseMessage
{
public:

    enum Commands { SAVE, DELETE };
    
    DBMessage( void ) : BaseMessage(_DB_), m_cmd(0) {}
    virtual ~DBMessage( void ) override = default;
    

    const std::string& 
    name(void) const { return m_name; }
    
    void 
    name(const std::string& network_name) { m_name = network_name; }
    
    int 
    cmd(void) const { return m_cmd; }
    
    void 
    cmd( int r ) { m_cmd = r; }
    
protected:

    int m_cmd;
    std::string m_name; 


};


class OptimiseMessage : public BaseMessage
{
public:

    enum ObjFunc { VOL, SUM };
    
    OptimiseMessage( void ) : BaseMessage(_OPTIMISE_), m_iters(100), m_seed(64), m_objFuncType(VOL), m_objFuncParams({1.0, 1.0, 2.0})  {}
    virtual ~OptimiseMessage( void ) override = default;
    

    int 
    iters(void) const { return m_iters; }
    
    void 
    iters( int i ) { m_iters = i; }
    
    int 
    seed(void) const { return m_seed; }
    
    void 
    seed( int i ) { m_seed = i; }
    
    int 
    objFuncType(void) const { return m_objFuncType; }
    
    void 
    objFuncType( int i ) { m_objFuncType = i; }
    
    
    const std::vector<ObjType>& 
    objFuncParams(void) const { return m_objFuncParams; }
    
    void 
    objFuncParams( const std::vector<ObjType>& i ) { m_objFuncParams = i; }
    
    // a list of LLH that should *not* be used - empty vector implies use all LLHs
    const std::vector<int>& 
    llhs(void) const { return m_llhs; }
    
    void 
    llhs( const std::vector<int>& i ) { m_llhs = i; }
    
protected:

    int m_iters;
    int m_seed;
    int m_objFuncType;
    std::vector<ObjType> m_objFuncParams;
    std::vector<int> m_llhs;
    
    std::string m_name; 

};

class LookAheadMessage : public BaseMessage
{
public:

    LookAheadMessage( void ) : BaseMessage(_LOOKAHEAD_) {}
    virtual ~LookAheadMessage( void ) override = default;
    
    const std::string& 
    pipeID(void) const { return m_pipeID; }
    
    void 
    pipeID(const std::string& pid) { m_pipeID = pid; }
    
protected:

    std::string m_pipeID; 

};


class ChangeMessage : public BaseMessage
{
public:

    ChangeMessage( void ) : BaseMessage(_CHANGE_), m_value(-1) {}
    virtual ~ChangeMessage( void ) override = default;
    
    const std::string& 
    ID(void) const { return m_ID; }
    
    void 
    ID(const std::string& cid) { m_ID = cid; }
    
    HydType 
    hydType(void) const { return m_hydType; }
    
    void 
    hydType(HydType ht)  { m_hydType = ht; }
    
    int 
    value(void) const { return m_value; }
    
    void 
    value(int v)  { m_value = v; }
    
    
protected:

    HydType m_hydType;
    int m_value;
    std::string m_ID; 
    

};

#endif


