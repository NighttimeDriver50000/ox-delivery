#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ChinesePostman/ChinesePostman.h>
#include <ChinesePostman/DrawImage.h>
#include <Boustrophedon/BCD.h>
#include <Boustrophedon/DrawImage.h>
#include <Leo/Point2D.h>
#include <Leo/ReebGraph.h>
#include <Leo/RegionData.h>

#include "fstream"
#include "ostream"
#include <algorithm>
#include <KCPP/KChinesePostmen.h>
#include <KCPP/FredericksonKCPP.h>
#include <KCPP/CAC.h>

/*class IncorrectInput : public excpetion 
  {
  virtual const char* what() const throw()
  {
  return "The input to the algorithm is incorrect\n";
  }
  } incorrectInput;
 */


class Controller : public QObject
{
	Q_OBJECT

	public:
		Controller();
		enum KCPP_MODE{ CRC_MODE, CAC_MODE };

		//Runs k rout finding solver on the image
		void run(const std::string& directory, const std::string& image, int k, KCPP_MODE mod);


	private:
		/** Chinese postman problem solvers **
		 * for k = 1 **/
		//ChinesePostman* runCPP(const ReebGraph& graph, const RegionData& data);

		/** and k > 1 **/
		void runkCPP(int k, ReebGraph& graph, std::list<Edge>& eulerCycle, RegionData data, KCPP_MODE mod);

        /** testing for possible k_vec values and writing the results in fileName*/
        void test(std::vector<int> k_vec, ReebGraph& graph, std::list<Edge>& eulerCycle, RegionData data, KCPP_MODE mod, std::string fileName);

        //Used to verify the input arguments
        void checkInputParams(const std::string& directory,
                const std::string& image, int k);

        // Waypoints generating function and the test results outputing one
        void generateWaypoints(RegionData& data, ReebGraph& graph,
                std::list<Edge>& eulerCycle, vector<Point2D>& wayPoints, KCPP_MODE mod);



	public:
		/** bulk testing **/
		// void runTests(int n, std::string filePrefix);

	private:
		bool m_cppSolved;

		std::string m_image; 
		std::string m_directory;

		//number of robots
		int m_k;
		int m_vertices;

		// vector of k tours
		std::vector<EulerTour> m_tours;
		// representation of the area as a boost graph
		std::vector<Graph> m_graph;

		KChinesePostmen* m_kcpp;
		ChinesePostman* m_cpp;
		RegionData m_data;
		int m_cnt;
		double m_maxCoverageCost;
};

#endif // CONTROLLER_H
