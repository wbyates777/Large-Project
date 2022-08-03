/* HydGraph 22/01/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydGraph.cpp   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HYDGRAPH_H__
#include "HydGraph.h"
#endif


extern "C" {
#include "types.h"
#include "epanet2_2.h"
}

#ifndef _HYDNETWORK_H_
#include "HydNetwork.h"
#endif

#include <cmath>
#include <sstream>
#include <algorithm> 
#include <iostream>
#include <functional>


void
HydGraph::init( EN_Project proj )
// get a graph of the whole network using my indexes
// EPANET2 indexes start from 1 - our indexes/ids start from 0
// use the real number of pipes/nodes not just those 
// we can modify or measure for pressure calcs
{   
    m_proj = proj;
    
    int numPipes;
    EN_getcount(m_proj, EN_LINKCOUNT, &numPipes);
  
    int numNode;
    EN_getcount(m_proj, EN_NODECOUNT, &numNode);
    
    m_graph.clear();

    for (int i = 0; i < numPipes; ++i)
    {
        int startNode, endNode; // these are indexes *NOT* ids
        EN_getlinknodes(m_proj, i + 1, &startNode, &endNode);

        m_graph.addEdge(i + 1, startNode, endNode);

    }
    
    std::vector<int> nodes = m_graph.nodes();
}

/*
void
HydGraph::init( HydNetwork* net )
{
    const std::vector<HydNetComp*>& components =  net->getComponent();
    
    m_graph.clear();
    m_node.clear();
    m_link.clear();
    
    for (int  i = 0; i < components.size(); ++i)
    {
        HydNetComp* comp = components[i];
        if (comp->componentType() == HydType::Pipe || comp->componentType() == HydType::Pump || comp->componentType() == HydType::Valve )
        {
            HydLink* link = static_cast<HydLink*>(comp);
            
            HydNode* toNode = net->findNode( link->toNodeID() );
            HydNode* fromNode = net->findNode( link->fromNodeID() );
            m_graph.addEdge(fromNode->pos(), toNode->pos());
            m_link.push_back(i);
        }
    }
    
    std::vector<int> nodes = m_graph.nodes();
    
    for (int i = 0; i < nodes.size(); ++i) 
    {
        m_node.push_back(nodes[i]);
    }
    
}
*/

double 
HydGraph::pressure( int node_idx ) const 
{
    double d;
    EN_getnodevalue(m_proj, node_idx, EN_PRESSURE, &d);
    return d;
}
   

Graph 
HydGraph::streamNode( int start_node, bool up, int depth ) const
// get the up/downstream nodes starting from start_node up to depth
// depth = 1 retieves immediate children
{
    Graph up_tree;
    std::deque<int> queue(1, start_node);

    int thisLevel = 1;
    int nextLevel = 0;
    
    // get the upstream node
    while (!queue.empty() && depth > 0)
    {
        int mynode = queue.back();
        queue.pop_back(); 
        
        thisLevel--;
        
        // get the nodes connected to this node
        std::vector<int> subnodes = m_graph.connectedTo(mynode);
        
        auto cmp = [up](double x, double y)->bool{ return up ? x > y : x <= y; };
       
        // which nodes are up stream; children in this context
        double my_pressure = pressure(mynode);
        for (int i = 0; i < subnodes.size(); ++i)
        {
            if (cmp(pressure(subnodes[i]), my_pressure)) 
            {
                queue.push_front( subnodes[i] );
                up_tree.addEdge(i, mynode, subnodes[i]);
                nextLevel++;
            }
        }
        
        if (thisLevel == 0)
        {
            thisLevel = nextLevel;
            nextLevel = 0;
            depth--;
        }
    }
    
    return up_tree;
}


//
// Implementation lifted from:
// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph
// http://www.geeksforgeeks.org/greedy-algorithms-set-6-dijkstras-shortest-path-algorithm/
// I've rewitten bits and added STL 


int 
HydGraph::minDistance(const std::vector<int>& dist, const std::vector<bool>& sptSet) const 
// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
{
    // Initialize min value
    int min = INT_MAX;
    int min_index = -1;
    
    for (int v = 0; v < dist.size(); ++v)
    {
        if (sptSet[v] == false && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }
    
    return min_index;
}


std::vector<short> 
HydGraph::dijkstra(const Graph& graph, int src, int trg) const
// implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
// see https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
// complexity of the implementation is O(V^2)

// I have modified this code to work with my Graph class
{
    std::vector<int> nodes = graph.nodes();
    int N = *std::max_element(nodes.begin(), nodes.end()) + 1; //(int) nodes.size();
    
    // dist[i] will hold the shortest distance from src to i
    std::vector<int> dist(N, INT_MAX);     
    
    // sptSet[i] will true if vertex i is included in shortest cost path tree  from src to i 
    std::vector<bool> sptSet(N, false); 

    // path info
    std::vector<short> previous(N, -1);
    
    // distance of source vertex from itself is always 0
    dist[src] = 0;
    
    // find shortest path for all vertices
    for (int count = 0; count < N - 1; ++count)
    {
        int u = minDistance(dist, sptSet);
        
        if (trg == u) // we have found target
            break;
        
        // mark the picked vertex as processed
        sptSet[u] = true;
        
        // update dist value of the adjacent vertices of the picked vertex.
        auto isEdge = [&graph](int u, int v)->bool { return (graph.edgeIdx(u,v) != -1); };
        auto cost = [&graph](int u, int v)->int { return (graph.edgeIdx(u,v) != -1); };
        
        for (int v = 0; v < N; ++v)
        {  
            // update dist[v] only if is not in sptSet, there is an edge from 
            // u to v, and total weight of path from src to  v through u is 
            // smaller than current value of dist[v]
            if (!sptSet[v] && isEdge(u,v) && dist[u] != INT_MAX && dist[u] + cost(u,v) < dist[v])
            {
                dist[v] = dist[u] + cost(u,v);
                previous[v] = u;
            }
        }
    }
    
    // print the constructed distance array
    // std::cout << "Vertex   Distance from Source" << std::endl;
    // for (int i = 0; i < N; ++i)
    // {
    //    std::cout << i << "     " << dist[i] << std::endl;
    // }
    
    std::vector<short> shortPath;
    
    if (trg != -1) // we have a target
    {
        int u = trg;
        // construct the shortest path shortPath
        while (previous[u] != -1)                
        {
            shortPath.insert( shortPath.begin(), u );
            u = previous[u];                         
        } 
        
        if (shortPath.size() != 0)
            shortPath.insert( shortPath.begin(), src );
        
        return shortPath;
    }
    
    // no target; return vertex path list from src
    return previous;
}


std::vector<short>
HydGraph::path( int startNode, int endNode, const Graph& subgraph ) const
{
    return dijkstra(subgraph, startNode, endNode);
}

Graph
HydGraph::neighbourNode( int start_node, int depth) const
{
    Graph up_tree;
    std::deque<int> queue(1, start_node);

    int thisLevel = 1; // number of nodes to be processed
    int nextLevel = 0; // number of nodes to be processed
    
    // get connected nodes
    while (!queue.empty() && depth > 0)
    {
        int mynode = queue.back();
        queue.pop_back(); 
        
        thisLevel--;
        
        // get the nodes connected to this node
        std::vector<int> subnodes = m_graph.connectedTo(mynode);
        
        //  all connected nodes are children in this context
        for (int i = 0; i < subnodes.size(); ++i)
        {
            queue.push_front( subnodes[i] );
            nextLevel++;
            std::vector<Edge> e = m_graph.edges(mynode, subnodes[i]);
            for (int j = 0; j < e.size(); ++j)
                up_tree.addEdge(e[j]);
        }
        
        if (thisLevel == 0)
        {
            thisLevel = nextLevel;
            nextLevel = 0;
            depth--;
        }
    }
    
    return up_tree;
}



