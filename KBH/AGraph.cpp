//
//  AGraph.cpp
//  EXE_SSHH
//
//  Created by bill on 24/02/2021.
//  Copyright © 2021 La Coruña. All rights reserved.
//

#include <stdio.h>

#ifndef __GRAPH_H__
#include "AGraph.h"
#endif


#ifndef __UTILS_H__
#include "AUtils.h"
#endif


std::vector<int>
Graph::nodes( void ) const
{
    std::map<int,int> vert;
    for ( int i = 0; i < m_edge.size(); ++i )
    {
        vert.insert( typename std::map<int,int>::value_type( parentAt(i), 1 ) );
        vert.insert( typename std::map<int,int>::value_type( childAt(i), 1 ) );
    }
    return util::toKeyVector( vert );
}
//
//
//
// return -1 if edge e not in graph


int
Graph::edgeIdx( const Edge& e ) const
{
    auto fidx = m_edgeIndex.find(std::pair<int,int>(e.first,e.second));
    if (fidx != m_edgeIndex.end())
        return fidx->second;

    fidx = m_edgeIndex.find(std::pair<int,int>(e.second,e.first));
    if (fidx != m_edgeIndex.end())
        return fidx->second;
    
    for ( int i = 0; i < m_edge.size(); ++i )
    {
        if (edgeEqual(e, m_edge[i]))
        {
            m_edgeIndex.insert( EdgeIndex::value_type(std::pair<int,int>(e.first,e.second), i ) );
            return i;
        }
    }
    
    return -1;
}

Edge
Graph::edge( int linkId ) const
{
    for ( int i = 0; i < m_edge.size(); ++i )
        if (linkId ==  m_edge[i].label)
            return m_edge[i];
    return Edge();
}

Edge
Graph::edge( int start, int finish ) const
{
    Edge e(start,finish);
    for ( int i = 0; i < m_edge.size(); ++i )
    {
        if (edgeEqual(e, m_edge[i]))
            return m_edge[i];
    }
    return Edge();
}

std::vector<Edge>
Graph::edges( int start, int finish ) const
{
    auto fidx = m_edgeMap.find(std::pair<int,int>(start,finish));
    if (fidx != m_edgeMap.end())
        return fidx->second;

    fidx = m_edgeMap.find(std::pair<int,int>(finish,start));
    if (fidx != m_edgeMap.end())
        return fidx->second;
    
    std::vector<Edge> retVal;
    Edge e(start,finish);
    for ( int i = 0; i < m_edge.size(); ++i )
    {
        if (edgeEqual(e, m_edge[i]))
            retVal.push_back( m_edge[i] );
    }
    
    m_edgeMap.insert( EdgeMap::value_type(std::pair<int,int>(start,finish), retVal ) );

    return retVal;
}

std::vector<int>
Graph::connectedTo( int node ) const
{
    auto fidx = m_connectedTo.find(node);
    if (fidx != m_connectedTo.end())
        return fidx->second;
    
    std::vector<int> retVal;
    for ( int i = 0; i < m_edge.size(); ++i )
    {
        const Edge& e = m_edge[i];
        if (node == e.first || node == e.second)
        {
            int idx = (e.first == node) ? e.second : e.first;
            retVal.push_back(idx);
        }
    }
    m_connectedTo.insert( ConnectMap::value_type(node, retVal ) );
    return retVal;
}


std::vector<int>
Graph::root( void ) const
{
    std::map<int,int> retVal;
    
    for ( int i = 0; i < m_edge.size(); ++i )
    {
        bool found = false;
        
        for ( int j = 0; j < m_edge.size(); ++j )
        {
            if (parentAt(i) == childAt(j))
            {
                found = true;
                break;
            }
        }
        
        if (found == false)
            retVal.insert( typename std::map<int,int>::value_type(parentAt(i),1) );
    }
    
    return util::toKeyVector( retVal );    
}


std::vector<int>
Graph::successor( int p, bool decend ) const
{
    std::vector<int> retVal;
    std::map<int,int> ancestor;
    std::deque<int> queue;
    
    queue.push_front( p );
    
    while (!queue.empty())
    {
        int par = queue.back();
        queue.pop_back();
        
        std::pair<typename std::map<int,int>::iterator, bool> result;
        result = ancestor.insert( typename std::map<int,int>::value_type( par, 1 ) );
        
        if (result.second) // if true we have a new node
        {
            for ( int i = 0; i < m_edge.size(); ++i )
            {
                if (par == parentAt(i))
                {
                    retVal.push_back( childAt(i) );
                    queue.push_front( childAt(i) );
                }
            }
        }
        
        if (!decend)
            queue.clear();
    }
    return retVal;
}
