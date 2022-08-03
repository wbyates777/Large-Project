/* AFileFinder 27/08/2017

 $$$$$$$$$$$$$$$$$$$$$
 $   AFileFinder.h   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History: This code is a C++ version of:
 
 Copyright (c) 1992 The Geometry Center; University of Minnesota
 1300 South Second Street;  Minneapolis, MN  55454, USA;
 
 This file is part of geomview/OOGL. geomview/OOGL is free software;
 you can redistribute it and/or modify it only under the terms given in
 the file COPYING, which you should have received along with this file.
 This and other related software may be obtained via anonymous ftp from
 geom.umn.edu; email: software@geom.umn.edu. 
 
 
 Authors: Charlie Gunn, Stuart Levy, Tamara Munzner, Mark Phillips 
 
 $Header: /home/cvsroot/partiview/src/findfile.c,v 1.2 2001/03/13 08:10:49 slevy Exp $ 
 
 The version presented here has a similar interface but is more portable

 
  Example
 =========
 
 AFileFinder ff;
 
 ff.setFileDirs({"~/Projects/src/StarTrek/Universe/DigitalUniverse/data/milkyway/specks",
                 "/Users/bill/Projects/src/StarTrek/Universe/DigitalUniverse/data/milkyway"});
 
 std::cout << ff.findFile("stars.speck") << std::endl;
 std::cout << ff.findFile("mw.cf") << std::endl;
 std::cout << ff.findFile("bombard.bas","~/Projects/src/StarTrek/basic80/bombard.bas") << std::endl;
 std::cout << ff.findFile("$TEST/test2.txt") << std::endl;
 
 exit(1);
 
 TODO: rewrite using #include <filesystem>
 
*/


#ifndef __AFILEFINDER_H__
#define __AFILEFINDER_H__


#include <string>
#include <vector>
#include <stack>

#ifndef __SINGLETON_H__
#include "ASingleton.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif
   
class AFileFinder
{
public:
    AFileFinder( void ): m_filedirs(), m_stack() { m_logger.getLogLevel("AFileFinder"); }
    AFileFinder( const std::vector<std::string>& fd ): m_filedirs(fd), m_stack() { m_logger.getLogLevel("AFileFinder"); }
    ~AFileFinder( void ) {}

    void
    setFileDirs( const std::vector<std::string>& fd );
    
    std::string 
    findFile( const std::string& fname, const std::string& superfile = ""  );
    
    void
    clear( void );
    
    void
    addFileDir( const std::string& path );
        
private:
     
    std::string
    dirPrefix( const std::string& path );
    
    std::vector<std::string>
    getSubDirs( const std::string  &name );
    
    std::string 
    getEnvVar( const std::string  &key );
    
    
    std::vector<std::string>
    envExpand(const std::string& s);
    
    
    std::vector<std::string> m_filedirs;
    std::stack<std::string>  m_stack;
    mutable Logger m_logger;

};

#define theFF Singleton<AFileFinder>::getClass()

#endif


