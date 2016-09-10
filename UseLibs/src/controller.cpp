/**==============================================================
 * \file Controller.cpp
 *
 * Contorller for initializing algorithms and running appropriate 
 * solvers for each one from one interface
 *
 * \author    Nare Karapetyan
 **==============================================================*/
#define DEBUG
#include "controller.h"
#include <fstream>
#include <KCPP/CAC.h>

Controller::Controller() : m_cppSolved(false), m_k(1) {}

/**==============================================================
 *!Runs BCD, CPP and if k is > 1 also KCPP algorithms 
 *
 * \param
 * \return void
 **==============================================================*/
void Controller::run(const std::string& directory, const std::string& image,
        int k, KCPP_MODE mod)
{

    ReebGraph graph;
    RegionData data;
    std::list<Edge> eulerCycle;
    vector<Point2D> wayPoints;

#ifdef DEBUG
//    std::cout << "Controller's run executed\n";
#endif

    checkInputParams(directory, image, k);

    m_directory = directory;
    m_image = image;
    m_k = k;

    //-------------------- BCD call -------------------------

#ifdef DEBUG
//    std::cerr << "BCD is starting...\n";
#endif

    BCD bcd(directory, image, data, graph);

#ifdef DEBUG
 //   std::cerr << "BCD is completed\n"; 
#endif

    //----------------- BCD call ended ----------------------

    ChinesePostman m_cpp(data, graph, eulerCycle, wayPoints);

#ifdef DEBUG
    /*std::cout << "After running cpp, checking the m_cpp var ...\n";
    std::cout << eulerCycle;
    std::cout << graph;
    std::cout << std::endl;
    std::cout << "Checking is passed!\n";*/
#endif

    m_cppSolved = true;

    runkCPP(m_k, graph, eulerCycle, data,  mod);
#ifdef DEBUG
    m_kcpp->printEulerianTours();
    //std::cout << std::endl;
#endif
    m_tours = m_kcpp->getKEulerianTours();
    generateWaypoints(data, graph, eulerCycle, wayPoints, mod);
}


/**==============================================================
 *!Runs the kcpp algorithm.
 *
 * \param
 * \return void
 **==============================================================*/
void Controller::runkCPP(int k, ReebGraph& graph, std::list<Edge>& eulerCycle, RegionData data, KCPP_MODE mod)
{
    assert(m_cppSolved && "runCPP must be called before runkCPP");

    try
    {
        if(mod == CRC_MODE) {
            m_kcpp = new FredericksonKCPP(eulerCycle, graph, k);
        } else {
            m_kcpp = new CAC(eulerCycle, data, graph, k);
        }
        m_kcpp->solve(true);
    }

    catch (const std::string& err) 
    {
        std::cout << "runkCPP error";
    }
}

/**==============================================================
 *!Checks the input parameters.
 *
 * \param
 * \return void
 **==============================================================*/
void Controller::checkInputParams(const std::string& directory,
        const std::string& image, int k)
{
    std::string message = "";

    if(k<1) 
    {
        message = "ERR:Number of robots must be positive integer!";
        throw std::invalid_argument(message);
    }

    /**TODO: Add also check for valid image format*/
    if( !std::ifstream((directory + image).c_str())) 
    {
        message = "ERR:There is no image at this path:  " + directory + image;
        throw std::invalid_argument(message);
    }
}

/**==============================================================
 *! Function for generating the waypoints from the generated tours
 *
 * TODO: must be divided further:
 * the waypoints creation and drawing must be separated,
 * also it actually doesn't have to recieve KCPP_MODE
 * \param 
 * \return void
 *
 * \Author Chris McKinney
 **==============================================================*/
void Controller::generateWaypoints(RegionData& data, ReebGraph& graph,
                std::list<Edge>& eulerCycle, vector<Point2D>& wayPoints, KCPP_MODE mod)
{
    WayPoints way(data, graph, eulerCycle, wayPoints);

#ifdef DEBUG
    cout << "----------------- Print Original EulerTour ------------";
    cout << "\n";
    std::list<Edge>::iterator itEC;
    for (itEC = eulerCycle.begin(); itEC != eulerCycle.end(); ++itEC) 
    {
        Edge edge = (*itEC);
        Vertex v1,v2;
        tie(v1,v2) = graph.getEndNodes(edge);

        ReebVertex vertexOne = graph.getVProp(v1);
        ReebVertex vertexTwo = graph.getVProp(v2);

        Point2D first = Point2D(vertexOne.x,
            (vertexOne.y1 + vertexOne.y2) / 2);
        Point2D second = Point2D(vertexTwo.x,
            (vertexTwo.y1 + vertexTwo.y2) / 2);

        std::cout << first;
        std::cout << " ";
        std::cout << second;
        ReebEdge redge = graph.getEProp(edge);
        std::cout << " " << redge.Eid << " | ";
    }
    cout << "----------------- End Original EulerTour ------------" << "\n\n";

    std::cout << "----------------- Print Original WayPoints ------------";
    cout << "\n";
    std::vector<Point2D>::iterator itW;
    for (itW = wayPoints.begin(); itW != wayPoints.end(); ++itW) 
    {
        cout << (*itW) << " ";
    }
    cout << "\n";
    std::cout << "----------------- End Original WayPoints ------------";
    cout << "\n\n";

#endif

    //Coverage edges are the ones you need to use to move
    std::vector<std::vector<Point2D> > tourPoints;

		int rmExt = m_image.find_last_of("."); 
		string img = m_image.substr(0, rmExt); 
		QString imageQS = QString(img.c_str());
		QString fileName = QString("%1.WayGraph.png").arg(imageQS);
		QImage qimage (QString((m_directory +"/"+ m_image).c_str()));
		srand(time(NULL));

    for (size_t i = 0; i < m_tours.size(); ++i)
    {
        EulerTour tour_i = m_tours.at(i);

        std::list<ReebEdge> ti;
        for (EulerTour::iterator it = tour_i.begin();
                it != tour_i.end(); ++it) {
            ti.push_back((m_kcpp->m_graph).getEProp(*it));
        }

#ifdef DEBUG
        std::cout << "---------------- (K)CPP Coverage Edges ------------";
        cout<<"\n";
        cerr << "Tour " << i << ":\n";
        std::list<Edge>::iterator it;
        for (EulerTour::iterator it = tour_i.begin();
                it != tour_i.end(); ++it) 
        {
            Vertex v1,v2;
            tie(v1,v2) = (m_kcpp->m_graph).getEndNodes(*it);

            ReebVertex vertexOne = (m_kcpp->m_graph).getVProp(v1);
            ReebVertex vertexTwo = (m_kcpp->m_graph).getVProp(v2);

            Point2D first = Point2D(vertexOne.x,
                (vertexOne.y1 + vertexOne.y2) / 2);
            Point2D second = Point2D(vertexTwo.x,
                (vertexTwo.y1 + vertexTwo.y2) / 2);

            std::cout << first;
            std::cout << " ";
            std::cout << second;
            ReebEdge edge = (m_kcpp->m_graph).getEProp(*it);
            std::cout << " " << edge.Eid << " | ";
        }
        cout << "\n";
        std::cout << "---------------- End (K)CPP Coverage Edges ------------";
        cout << "\n";
#endif

        //temporary graph that will be used for converting
        //  reebedges to a new reeb graph for each tour
        ReebGraph temporaryGraph;
        std::vector<Point2D> tempWayPoints;

        //only used in modifying temporaryGraph. Nothing else. 
        way.convertTourToReebGraph(ti, m_kcpp->m_graph, temporaryGraph); 

        std::list<Edge> tmpBcCpp = temporaryGraph.getEdgeList();

#ifdef DEBUG
      std::cout << "---------------- Converted Coverage Edges ------------";
        cout<<"\n";
        cerr << "Tour " << i << ":\n";
        for (EulerTour::iterator it = tmpBcCpp.begin();
                it != tmpBcCpp.end(); ++it) 
        {
            Vertex v1,v2;
            tie(v1,v2) = temporaryGraph.getEndNodes(*it);

            ReebVertex vertexOne = temporaryGraph.getVProp(v1);
            ReebVertex vertexTwo = temporaryGraph.getVProp(v2);

            Point2D first = Point2D(vertexOne.x,
                (vertexOne.y1 + vertexOne.y2) / 2);
            Point2D second = Point2D(vertexTwo.x,
                (vertexTwo.y1 + vertexTwo.y2) / 2);

            std::cout << first;
            std::cout << " ";
            std::cout << second;
            ReebEdge edge = temporaryGraph.getEProp(*it);
            std::cout << " " << edge.Eid << " | ";
        }
        cout << "\n";
        std::cout << "---------------- End Converted Coverage Edges ------------";
        cout << "\n";
#endif

        //should generate a new waypoints path for this route
        //  Done like this because the constructor automatically runs
        //  the functions necessary to create waypoints. 
        WayPoints tourWayPoints(data, temporaryGraph, tmpBcCpp, tempWayPoints);

#ifdef DEBUG
        cout<<"\n";
        std::cout << "----------------- Coverage WayPoints ------------";
        cout<<"\n";
        cerr << "Tour " << i << ":\n";
        vector<Point2D>::iterator iter;
        for (iter = tempWayPoints.begin(); iter != tempWayPoints.end(); ++iter)
        {
            cout << (*iter) << " ";
        }
        cout << "\n";
        std::cout << "----------------- End Coverage WayPoints ------------";

        std::cerr << "\n\nVerticies:\n";
        temporaryGraph.printVertex();
        std::cerr << "\nEdges:\n";
        temporaryGraph.printEdges();

        std::cerr << "\nEdge count: " << tmpBcCpp.size() << "\n";
        std::cerr << std::endl << "Graph Edges:" << std::endl;
        std::list<Edge>::iterator tbcit;
        for (tbcit = tmpBcCpp.begin(); tbcit != tmpBcCpp.end(); ++tbcit)
        {
            std::cerr << " Edge:" << std::endl;
            ReebEdge e = temporaryGraph.getEProp(*tbcit);

        }
        std::cerr << std::endl;
#endif

        //pushes each tours waypoints to store for future use
        tourPoints.push_back(tempWayPoints);

        //m_cpp.viewEulerGraph(fileName, data, graph, eulerCycle, wayPoints);
        //tourWayPoints.viewWaypoints(fileName, data, temporaryGraph, tmpBcCpp, tempWayPoints);

        QColor color(rand()%256, rand()%256, rand()%256); 
        DrawImage placeHolder(graph, data, tmpBcCpp, tempWayPoints);
        placeHolder.setImageBuffer(qimage);
        placeHolder.drawWaypoints(tempWayPoints, 0, 0, color);
        qimage = placeHolder.getImageBuffer(); 
        placeHolder.saveImageBuffer(fileName);
    }

#ifdef DEBUG
    std::ofstream outputFile;
    outputFile.open("tourLines.txt");
    for (unsigned i = 0; i < tourPoints.size(); ++i)
    {
        outputFile << "\n" << "Start Tour " << i << "\n"; 
        std::vector<Point2D> tempPoints = tourPoints.at(i);

        vector<Point2D>::iterator iter;
        for(iter = tempPoints.begin(); iter != tempPoints.end(); ++iter)
        {
            outputFile << (*iter) << " ";
        }

        outputFile << "\n" << "End Tour " << i << "\n";
    }
#endif
}
