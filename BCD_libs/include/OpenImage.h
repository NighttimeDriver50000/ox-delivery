#ifndef OpenImage_H_
#define OpenImage_H_

/*************************************************************************
 *
 * @author: Kelly Benson
 * @email: bensonke@email.sc.edu
 * @date: May 16, 2015
 *
 * Header file for the OpenImage which opens an image.
 *
**/

#include <iostream>
#include <dirent.h> 
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "Leo/RegionData.h"

class OpenImage 
{

public:

    OpenImage();
    ~OpenImage(); 

    RegionData open(string directory, string fileName);
    set<string> findFiles();
    void setMapFiles();
    set<string> getMapFiles();
    void printMapFiles();

private:

    set<string> mapFiles;
    string dir;
    string file;

};
#endif
