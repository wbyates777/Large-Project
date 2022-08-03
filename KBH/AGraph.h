/* Graph 16/02/2013
 
 $$$$$$$$$$$$$$$$$$$$$$$$
 $   Graph.h - header   $
 $$$$$$$$$$$$$$$$$$$$$$$$
 
 by W.B. Yates    
 Copyright (c) W.B. Yates. All rights reserved.
 History: 
 
  A general storage class for an arbitrary graph as a list of unique edges; the graph may countain cycles
 
 
 
 */

#ifndef __GRAPH_H__
#define __GRAPH_H__

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <string>
#include <vector>
#include <iostream>
#include <deque>
#include <map>

typedef short GNode;

struct Edge
{
    Edge(void): first(-1), second(-1), label(-1) {}
    
    explicit Edge(int linkId): first(-1), second(-1), label(linkId) {}
    explicit Edge(int f, int s): first(f), second(s), label(-1) {}
    explicit Edge(const std::pair<int,int>& p): first(p.first), second(p.second), label(-1) {}
    explicit Edge(int linkId, int f, int s): first(f), second(s), label(linkId) {}
    

    
    GNode first;
    GNode second;
    GNode label;
};

inline std::ostream&
operator<<(std::ostream& ostr, const Edge& e)
{
    ostr << e.first << ' ' << e.second << " '" << e.label << "' ";
    return ostr;
}

typedef std::vector<Edge> Edges;

class Graph
{
public:
	
    Graph( void )=default;
    ~Graph( void ) { m_edge.clear(); }
	
    void
    clear( void ) { m_edge.clear(); }
    
	//
	// Vertex(s)
	//
	std::vector<int> // a list of parentless nodes; could be empty
	root( void ) const;
	
	std::vector<int> // a list of vertex nodes; could be empty
	nodes( void ) const;
	
    // which nodes are connected to this node
    std::vector<int>
    connectedTo( int node ) const;
    
	// immediate children only i.e nodes x that have p as a direct parent i.e edges (p,x)
	// for all children, including children of children, set decendants to true
	std::vector<int>
    successor( int parent, const bool decend = false ) const;
	
	//
	// Edges
	//	
	void // add an edge to the graph
	addEdge( int linkId, int start, int finish ) 
    { 
        Edge e(linkId, start, finish);
        m_edge.push_back( e ); 
    }
    
    void // add an edge to the graph
    addEdge( const Edge& e ) 
    { 
        m_edge.push_back( e ); 
    }
	
    int
    parentAt( int edgeIdx ) const { return m_edge[edgeIdx].first; }
    
	int
	childAt( int edgeIdx ) const { return m_edge[edgeIdx].second; }

    Edge
	edgeAt( int edgeIdx ) const { return m_edge[edgeIdx]; }
    
    // find the edge with edge label 'linkId'
    Edge
    edge( int linkId ) const;
    
    
    // find the first edge with start at node 'start' and finish at node 'finish'
    Edge
    edge( int start, int finish ) const;
    
    // find all edges with start at node 'start' and finish at node 'finish'
    std::vector<Edge>
    edges( int start, int finish ) const;
    
    // all edges
    Edges
	edges( void ) const { return m_edge; }
    
    // the index of edge e - return -1 if edge e not in graph
    int
    edgeIdx( const Edge& e ) const;
    
    // the index of an edge e = (start, finish) - return -1 if edge e not in graph
    int
    edgeIdx( int start, int finish ) const { return edgeIdx(Edge(start, finish)); }
    
    // number of edges in graph
	int 
	size( void ) const { return int(m_edge.size()); }
	
    static bool
    edgeEqual(const Edge& a, const Edge& b)
    {
        return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
    }
    
private:
	
	Edges m_edge;
    
    // cache stuff for speed
    typedef std::map<int,std::vector<int>> ConnectMap;
    typedef std::map<std::pair<int,int>,std::vector<Edge>> EdgeMap;
    typedef std::map<std::pair<int,int>,int> EdgeIndex;
    
    mutable ConnectMap m_connectedTo;
    mutable EdgeMap    m_edgeMap;
    mutable EdgeIndex  m_edgeIndex;
};















#endif

//

