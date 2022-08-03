/* KBHeuristic 17/03/2021

 $$$$$$$$$$$$$$$$$$$$$$$
 $   KBHeuristic.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __KBHEURISTIC_H__
#include "KBHeuristic.h"
#endif

#ifndef __HYDEPANET_H__
#include "HydEPANET.h"
#endif

extern "C" {
#include "types.h"
#include "funcs.h"
#include "epanet2_2.h"
}


#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine

void
KBHeuristic::init( HydProblem *problem )
{
    assert(problem);
    m_problem = problem;  
    m_epanet  = m_problem->getEPANET();
    m_graph.init(m_epanet->getProject());
    m_nodes = m_graph().nodes();
    m_logger.getLogLevel( "KBHeuristic" );
}


int
KBHeuristic::roulette( int num )
{
    int diam = -1;
    std::vector<double> probs(num);
 
    if (num)
    {
        for (int i = 0; i < num - 1; ++i)
            probs[i] = (1.0 / std::pow(2.0, (i)));
        
        probs[num-1] = (1.0 / std::pow(2.0, (num) - 1));
        
       // for (int i = 0; i < num; ++i)
        //    probs[i] = (1.0 / std::pow(2.0, (i))) ;
        
        diam = roulette( probs );
    }
    
    return diam;
}


int
KBHeuristic::roulette( const std::vector<double>& obj_vals )
// roulette wheel selection
{          
    if (obj_vals.empty())
        return -1;
    
    if (obj_vals.size() == 1)
        return 0;
    
    double total = 0.0;
    for (int i = 0; i < obj_vals.size(); ++i) 
        total += obj_vals[i];

    double prob  = m_ran.ran() * total;   
    double sum   = 0.0;

    for (int i = 0; i < obj_vals.size(); ++i) 
    {
        sum += obj_vals[i];
        if (sum > prob)
            return i;
    }
    
    return -1;
}

void 
KBHeuristic::shuffle_edges( std::vector<Edge>& edges )
// randomize the order in which the edges are presented 
{  
    int i, j;
    double x;

    if (edges.size() == 1)
        return;
    
    if (edges.size() == 2)
    {
        if (m_ran.ran() > 0.5)
            std::swap(edges[0], edges[1]);
    }
    else
    {
        for (i = (int) edges.size() - 1; i != 0; --i) 
        {
            x = m_ran.ran() * static_cast<double>(i);
            j = static_cast<int>(x);
            std::swap(edges[i], edges[j]);
        }
    }
}

void
KBHeuristic::kb_bottleneck( HHSolution& solution, double param )
{
    int scenario_index = 0;
    // get deficits; 

    m_head_diff.resize(m_problem->constraints().size(), 0.0); 
    bool isDeficit = false;
    for (int i = 0; i < m_problem->constraints().size();  ++i)
    {
        double h = m_epanet->head(m_problem->constraints()[i].index());
        m_head_diff[i] = m_problem->constraints()[i].values()[scenario_index] - h;
        if (m_head_diff[i] > 0.0)
            isDeficit = true;
            
    }
    
    // we have nodes in deficit
    if (isDeficit)
    {
        std::vector<int>    deficitIdx;
        std::vector<double> deficitValue;
      
        for (int i = 0; i < m_head_diff.size(); ++i)
        {
            if (m_head_diff[i] > 0.0)
            {
                deficitIdx.push_back(m_problem->constraints()[i].index());
                deficitValue.push_back(m_head_diff[i]);
            }
        }
        
        Graph up_tree;
        int depth = 3; // results in max path length of depth + 1
        int trys = 3;
        int deficitNode = -1;
        for (int i = 0; i < trys; ++i)
        { 
            // choose a node in deficit -  roulette wheel
            deficitNode = deficitIdx[roulette(deficitValue)];  
            up_tree = m_graph.upstreamNode( deficitNode, depth );
            if (up_tree.size() != 0) // we have upstream nodes
                break;
        }
        
        if (up_tree.size() == 0) // no upstream nodes
            return;
        
        // get the shortest path from the deficit node to each upstream node
        // we work with the small graph of upstream nodes not the whole network
        std::vector<int> up_nodes = up_tree.nodes();
        std::vector<std::vector<short>> paths(up_nodes.size());
        for (int i = 0; i < up_nodes.size(); ++i)
            paths[i] = m_graph.path(deficitNode, up_nodes[i], up_tree);
        
        // find the best, shortest path
        int best_path = -1;
        for (int len = 2; len < depth + 1; ++len)
        {
            double deficit_max = 0.0;
            
            for (int i = 0; i < paths.size(); ++i)
            {
                if (paths[i].size() == len)
                {
                    int modifiable = 0;
                    
                    for (int j = 1; j < len; ++j)
                    {    
                        std::vector<Edge> edges = m_graph().edges(paths[i][j-1], paths[i][j]);
                        
                        for (int k = 0; k < edges.size(); ++k)
                        {
                            assert(edges[k].label != -1);
                        
                            // -1 indicates pipe not modifiable
                            if (m_problem->dvar2spec()[edges[k].label] != -1)
                                modifiable += 1;
                        }
                    }
                    
                    if (modifiable)
                    {
                        int node = m_problem->cons2spec()[paths[i].back()];
                        // if the node has a constraint
                        if (node != -1)
                        {
                            double d = m_head_diff[node]; 
                            if (d > deficit_max)
                            {
                                deficit_max = d;
                                best_path = i;
                            }
                        }
                    }
                }
            }
            
            if (best_path != -1)
                break;
        }
        
        if (best_path != -1)
        {
            Edge edge;
            std::vector<short>& path =  paths[best_path];
            for (int i = 1; i < path.size(); ++i)
            //for (int i = (int) paths[best_path].size()-1; i > 0; --i)
            {
                std::vector<Edge> edges = m_graph().edges(path[i-1], path[i]);

                // if there are several links between two nodes shuffle them so we 'pick one at random' 
                shuffle_edges(edges); 
                
                for (int j = 0; j < edges.size(); ++j)
                {
                    assert(edges[j].label != -1);
                
                    // -1 indicates pipe not modifiable
                    if (m_problem->dvar2spec()[edges[j].label] != -1)
                    {
                        edge = edges[j];
                        break;
                    }
                }
                
                if (edge.label != -1)
                    break;
            }
            
            // is the link in our list of modifiable pipes
            if (edge.label != -1)
            {
                int idx = m_problem->dvar2spec()[edge.label];
                int diam = solution[idx];
                
                int pos = diam + 1, num = 0;
                if (pos < m_problem->options()[idx].size())
                {
                    num = (int) m_problem->options()[idx].size() - pos;
                    diam = roulette( num ) + pos;
                }
                
                solution[idx] = diam;
                //   static int count = 0;
                //   std::cout << "KBH OK " << count++ << std::endl;
             
            }
        }
    }
    // no nodes in deficit
    else ///if (deficitValue.empty()) 
    {
        std::vector<int>    deficitIdx;
        std::vector<double> deficitValue;
        for (int i = 0; i < m_head_diff.size(); ++i)
        {
            if (m_head_diff[i] <= 0.0)
            {
                deficitIdx.push_back(m_problem->constraints()[i].index());
                deficitValue.push_back(-m_head_diff[i]);
            }
        }
        
        //int excessNode = deficitIdx[roulette(deficitValue)];
       // Graph up_tree = m_graph.upstreamNode( excessNode, 1 );

        Graph up_tree;
        int depth = 1; // results in max path length of depth + 1
        int trys = 1;
        int excessNode = -1;
        for (int i = 0; i < trys; ++i)
        { 
            // choose a node in excess -  roulette wheel
            excessNode = deficitIdx[roulette(deficitValue)];  
            up_tree = m_graph.upstreamNode( excessNode, depth );
            if (up_tree.size() != 0) // we have upstream nodes
                break;
        }
        
        if (up_tree.size() == 0) // no upstream nodes
            return;
        
        Edge best_pipe;
        double deficit_min = 0.0;
        for (int j = 0; j < up_tree.size(); ++j)
        {    
            Edge edge = up_tree.edgeAt(j);
            assert(edge.label != -1);
            
            // -1 indicates pipe not modifiable
            if (m_problem->dvar2spec()[edge.label] != -1)
            {
                int node = (edge.second == excessNode) ? edge.first : edge.second ; 
                
                int node_idx = m_problem->cons2spec()[node];
                if (node_idx != -1)
                {
                    double d = m_head_diff[node_idx]; 
                    
                    if (d < deficit_min)
                    {
                        deficit_min = d;
                        best_pipe = edge;
                    }
                }
            }
        }
        
        if (best_pipe.label != -1)
        {
            int idx  = m_problem->dvar2spec()[best_pipe.label];
            int diam = solution[idx];
            diam = roulette( diam + 1 ); // select a smaller pipe
            solution[idx] = diam;
        }
    }
}


void
KBHeuristic::kb_pipesmooth( HHSolution& solution, double param)
{
    // evalauteObj( solution );
    
    double upsum = 0.0;
    double downsum = 0.0;
    int ridx = -1;
    int trys = 3;
    for (int i = 0; i < trys; ++i)
    {
        ridx = rndType( static_cast<int>(HydType::Pipe) );
        int pipe_id = m_problem->dvariables()[ridx].index();
        
        Edge pipe = m_graph().edge( pipe_id );
        
        int upNode = pipe.first, downNode = pipe.second;
        if (m_epanet->pressure(downNode) > m_epanet->pressure(upNode))
            std::swap(upNode, downNode);

        Graph con_tree = m_graph.neighbourNode( upNode, 1 );
     
        upsum = 0.0;
        downsum = 0.0;
        for (int j = 0; j < con_tree.size(); ++j)
        { 
            Edge edge = con_tree.edgeAt(j);
            
            // don't add the link we plan to modify 
            if (!Graph::edgeEqual(edge, pipe))
            {
                int n = (edge.first == upNode) ? edge.second : edge.first;
                
                if (m_epanet->pressure(n) > m_epanet->pressure(upNode)) 
                    upsum += m_epanet->diameter(edge.label); 
                else downsum += m_epanet->diameter(edge.label); 
            }
        }
        
        if (upsum > 0 && downsum > 0)
            break;
    }
    
    if (upsum > 0 && downsum > 0)
    {
        double Dmax = upsum - downsum;
        if (Dmax > 0.1)
        {
            // allowable range of diameters
            int num = 0;
            for ( ; num < m_problem->options()[ridx].size(); ++num)
            {
                HydFloat diam = m_problem->options()[ridx].value(num);
                if (diam > Dmax + 0.1)
                    break;
            }
            
            int diamIdx = -1;
            // roulette wheel selection; favour larger diameters smaller than this diameter
            if (num == 0)
                diamIdx = 0;
            else diamIdx = num - roulette( num ) - 1;
            
            // mutate diam to any value less than Dmax
            solution[ridx] = diamIdx;

         //  static int count = 0;
         //  std::cout << "KBH OK " << count++ << std::endl;
        }
    }
}

void
KBHeuristic::kb_block_creep( HHSolution& solution, double param )
// increase or decrease a randomly selected group of pipe diameters by one pipe size.
{
    int node_id = m_nodes[rndInt((int)m_nodes.size())]; 
    
    int depth = param + 1;
    
    if (depth > 5)
        depth = 5;
    
    Graph subgraph = m_graph.neighbourNode(node_id, depth);
    
    double r = m_ran.ran();
    
    for (int i = 0; i < subgraph.size(); ++i)
    {
        int id = subgraph.edgeAt(i).label; // EPANET index
        int pipe_idx = m_problem->dvar2spec()[id]; // solution vector index
            
        if (pipe_idx != -1)
        {
            int diam = solution[pipe_idx];

            if (r > 0.5)
                diam++;
            else diam--;
            
            if (diam < 0)
                diam = 0;
            
            int maxDiams = (int) m_problem->options()[pipe_idx].size();
            if (diam >= maxDiams)
                diam = maxDiams - 1;
            
            solution[pipe_idx] = diam;
        }
    }
}
