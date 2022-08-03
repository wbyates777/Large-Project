/* HydGraph 22/01/2021

 $$$$$$$$$$$$$$$$$$
 $   HydGraph.h   $
 $$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A few graph based functions to help navigate the EPANET network
 The graph uses the EPANET indexes (1,2,...) which are unique for links
 and unique for nodes (but not both - that is we can have link_index = 1 and a node_index = 1)
 
 Deals with multiple links between two nodes
 
 TODO: convert to work with HydNetComp positions pos() - then all our stuff is insulated from EPANET
 
*/


#ifndef __HYDGRAPH_H__
#define __HYDGRAPH_H__



#include <string>
#include <vector>
#include <map>

 
#ifndef __GRAPH_H__
#include "AGraph.h"
#endif


//class HydNetwork;
struct Project;

class HydGraph
{
public:

    HydGraph( void )=default;
    ~HydGraph( void )=default;

    void
    init( Project* proj );
    
   // void
   // init( HydNetwork* net );
    

    // the whole network; could be big
    const Graph& 
    operator()(void) const { return m_graph; }

    // depth of 1 will return immediate children
    Graph 
    upstreamNode( int start_node, int depth = 1 ) const { return streamNode( start_node, true, depth ); }
    
    Graph 
    downstreamNode( int start_node, int depth = 1 ) const { return streamNode( start_node, false, depth ); }
    
    // depth of 1 will return immediate children
    Graph
    neighbourNode( int node, int depth = 1) const;
    
    std::vector<short>
    path( int startNode, int endNode, const Graph& subgraph ) const;
    
private:

    HydGraph( const HydGraph& )=delete;

    HydGraph&
    operator=( const HydGraph& )=delete;

    int 
    minDistance(const std::vector<int>& dist, const std::vector<bool>& sptSet) const;

    std::vector<short> 
    dijkstra(const Graph& graph, int src, int trg) const;
    
    double 
    pressure( int node_idx ) const;
    
    Graph 
    streamNode( int start_node, bool up, int depth ) const;
   
    
    Project* m_proj;
    Graph m_graph;

};

#endif


