#include "CAC.h"

#include <boost/graph/graph_utility.hpp>

#include <iostream>

#define DEBUG_CAC

/* helper predicates for list sort algorithm */
/* Comparison by X axis*/
bool compareByXCoords (const ReebEdge& e1, const ReebEdge& e2) { 
	unsigned int numPoints;
	double midX1 = 0.0;
	double midX2 = 0.0;
	if (numPoints == 0)
	{
		return false;
	}
	else if (numPoints == 1) 
	{
		midX1 = e1.topBoundary[0].xcoord();
		midX2 = e2.topBoundary[0].xcoord();
	}
	else
	{
		midX1 = e1.topBoundary[numPoints/2-1].xcoord();
		midX2 = e2.topBoundary[numPoints/2-1].xcoord();
	}
	return midX1 < midX2;
}

/* Comparison by Y axis*/
bool compareByYCoords (const ReebEdge& e1, const ReebEdge& e2) { 
	unsigned int numPoints;
	double midY1 = 0.0;
	double midY2 = 0.0;
	if (numPoints == 0)
	{
		return false;
	}
	else if (numPoints == 1) 
	{
		midY1 = (e1.topBoundary[0].ycoord()
				+ e1.bottomBoundary[0].ycoord()) / 2;
		midY2 = (e2.topBoundary[0].ycoord()
				+ e2.bottomBoundary[0].ycoord()) / 2;
	}
	else
	{
		midY1 = (e1.topBoundary[numPoints/2-1].ycoord()
				+ e1.bottomBoundary[numPoints/2-1].ycoord()) / 2;
		midY2 = (e2.topBoundary[numPoints/2-1].ycoord()
				+ e2.bottomBoundary[numPoints/2-1].ycoord()) / 2;
	}
	return midY1 < midY2;
}

/**==============================================================
 * \ref Coverage with Area Clustering algorithm
 * 
 * Input: Undirected weighted graph, number of routs k, the optimal one route 
 * optained by Chinese postman algorithm
 *
 * Output: k routs in the graph
 *
 * Notations:
 *
 * Algorithm:
 * 1. Sort cells by x and y axis
 *
 * \author Nare Karapetyan
 **==============================================================*/

CAC::CAC(const EulerTour& tour, ReebGraph graph, int k)
    : KChinesePostmen(), m_optimalPath(tour)
{
	m_graph = graph;
    Edge_Iter ei, ei_end;
    Vertex v_first, v_second;
    ReebEdge eprop;
    Edge currEdge;
    Out_Edge_Iter oi, oi_end;

    for (tie(ei, ei_end) = graph.getEdges(); ei != ei_end; ei++) {
        currEdge = *ei;
        eprop = graph.getEProp(currEdge);
        m_sortedGraphEdges.push_back(eprop);
    }
    m_sortedGraphEdges.sort(compareByYCoords);
    m_sortedGraphEdges.sort(compareByXCoords);

#ifdef DEBUG_CAC
        tie(v_first, v_second) = graph.getEndNodes(*ei);
        std::cout << eprop << ", attached to vertices: " << graph.getVProp(v_first).Vid
            << " & " << graph.getVProp(v_second).Vid << ", is null? "
            << (currEdge == ReebGraph::nullEdge()) << endl;
#endif
}

/**==============================================================
 * The solver for k Chinese postman problem with CAC algorithm
 * no return value, the results are stored in the class member variables
 * m_eulerTours, without the shortest paths
 *
 * \param none
 * \return none
 **==============================================================*/
void CAC::solve()
{
    
/*    double c_subR_last = 0.0, c_subR = 0.0;
    double c_R_last = 0.0, c_R = 0.0;
    kcpp::Vertex v_last = m_sourceVertex;
    Edge e_last;

    EulerTour::iterator ei= m_optimalPath.begin();

    for(int j =1; j<=m_k; ++j) {
        std::list<Edge> tour_j;
        std::list<ReebEdge> tour_jv;
        double L_j = (m_optimalCost-2*m_smax) * double(j)/double(m_k) + m_smax;

        double C_R_Vl_j = m_shortTravelDistances.at(v_last);*/

        /***********************************************
         * Finding the last vertex on the optimal tour 
         *  such that the cost of the tour is <= L_j   */
    /*
        while(ei!=m_optimalPath.end()) {
            Vertex v_first, v_second;
            tie(v_first, v_second) = m_graph.getEndNodes(*ei);

            kcpp::Vertex v1 = getVertex(v_first);
            kcpp::Vertex v2 = getVertex(v_second);

            ReebEdge edge = m_graph.getEProp(*ei);

            c_subR += edge.area; 
            c_R = c_subR + m_shortTravelDistances.at(v_last) + m_shortTravelDistances.at(v2); 
            tour_jv.push_back(edge);
            tour_j.push_back(*ei);

            if(c_R>L_j) {
                //--ei;
                c_subR = 0;
                    c_R = 0;

                // Determine which vertex of the edge to take: first or second one
                double r_j = L_j - c_R_last;
                double sum_1 = r_j + m_shortTravelDistances.at(v1);
                double sum_2 = edge.area  - r_j + m_shortTravelDistances.at(v2);
                if(sum_1 <= sum_2) {
                    --ei;
                    tour_j.pop_back();
                    tour_jv.pop_back();
                }
                ++ei;
                break;
            }

            c_subR_last = c_subR;
            c_R_last = c_R;

            ++ei;
        }

        if(ei!=m_optimalPath.end()) {

            Vertex v_first, v_second;

            tie(v_first, v_second) = m_graph.getEndNodes(*ei);

            //std::cout << "edge -> " << m_graph.getEProp(*ei).Eid;
            v_last = getVertex(v_second);
        }*/
        /***********************************************/
/*
        if(!tour_j.empty()) {

            m_eulerTours.push_back(tour_j);
        }
    }*/
}

/**==============================================================
 * Returns sorted by X and Y axis adjacent list of edges attached to
 * a specific v vertex
 *
 * \param Vertex
 * \return double cost
 **==============================================================*/
std::list<ReebEdge> CAC::getAdjacentList(Vertex v)
{
    ReebVertex vprop;
    ReebEdge eprop;
    Out_Edge_Iter oi, oi_end;
    unsigned int currDegree;
		std::list<ReebEdge> adjacentList;

    assert(v != ReebGraph::nullVertex() && "the vertex v is null in getAsjacentList");

    vprop = m_graph.getVProp(v);
    currDegree = m_graph.degree(v);

    if (currDegree > 0) 
    {
        for (tie(oi, oi_end) = m_graph.getEdges(v); oi != oi_end; oi++)
        {
            eprop = m_graph.getEProp(*oi);
            adjacentList.push_back(eprop);
        }
    }
    adjacentList.sort(compareByYCoords);
    adjacentList.sort(compareByXCoords);
    return adjacentList;
}


/*
void FredericksonKCPP::printKTours()
{
    std::cout << "distances " << std::endl;
    boost::graph_traits < UndirectedGraph >::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = boost::vertices(*m_coverageGraph); vi != vend; ++vi) {
        std::cout << "distance(" << *vi << ") = " << m_shortCoverDistance->at(*vi) << ", " << std::endl;
    }

    std::cout << std::endl;

   for(int j =0; j < m_lastVertices.size(); ++j) {
       std::cout << "--> " << m_lastVertices.at(j);
   }
   std::cout << std::endl;
}*/

