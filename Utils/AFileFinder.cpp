/* AFileFinder 27/08/2017

 $$$$$$$$$$$$$$$$$$$$$$$
 $   AFileFinder.cpp   $
 $$$$$$$$$$$$$$$$$$$$$$$

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

*/

#ifndef __AFILEFINDER_H__
#include "AFileFinder.h"
#endif


//#if defined(unix) || defined(__unix)
#include <unistd.h>		/* needed for access() */
#include <dirent.h>     /* needed for readdir() */

//#else	/* Win32 */
//#include <io.h>
//#define R_OK  4		/* No Windows include file defines this?! */
//#endif

void
AFileFinder::clear( void )
{
    m_filedirs.clear();
}

void
AFileFinder::addFileDir( const std::string& path )
{
    // perhaps we should add expanded path;
    //envExpand(path);
    
    m_filedirs.push_back( path );
    if (m_logger.level() > 1)
    {
        Message txt1;
        txt1 << "Adding path " << path;
        m_logger.logMsg( txt1 );
        m_logger.flush();
    } 
}
/*-----------------------------------------------------------------------
 * Function:	filedirs
 * Description:	set the list of directories to search for files
 * Args:	dirs: NULL-terminated array of pointers to directory
 *		  strings
 * Author:	mbp
 * Date:	Wed Feb 12 14:09:48 1992
 * Notes:	This function sets the list of directories searched by
 *		findfile().   It makes an internal copy of these directories
 *		and expands all environment variables in them.
 */
void
AFileFinder::setFileDirs( const std::vector<std::string>& fd )
{
    if (m_logger.level() > 0)
    {
        Message txt1;
        txt1 << "Loading paths";
        m_logger.logMsg( txt1 );
        m_logger.flush();
    }
    
    clear();
    
    for (int i = 0; i < fd.size(); ++i) 
    {
        addFileDir(fd[i]); 
    }
    
    if (m_logger.level() > 0)
    {
        Message txt1;
        txt1 << m_filedirs.size() << " file paths set ";
        m_logger.logMsg( txt1 );
        m_logger.flush();
    }
}


/*-----------------------------------------------------------------------
 * Function:	dirprefix
 * Description:	get the directory prefix from a pathname
 * Args:	*path: the pathname
 *		*dir: pointer to location where answer is to be stored
 * Author:	mbp
 * Date:	Wed Feb 12 14:17:36 1992
 * Notes:	Answer always ends with a '/' if path contains a '/',
 *		otherwise dir is set to "".
 */
std::string
AFileFinder::dirPrefix( const std::string& path )
{
    long end = path.size() - 1;
    for (; end >= 0 && path[end] != '/'; --end) ;
    return path.substr(0, end + 1);
}





std::vector<std::string>
AFileFinder::getSubDirs( const std::string& dname )
{
    std::vector<std::string> names;
    struct dirent *mydirinfo;
    DIR *mydir = opendir(dname.c_str());
    
    if (mydir != nullptr)
    {    
        while ((mydirinfo = readdir(mydir)) != nullptr)
        {
            if (mydirinfo->d_type == DT_DIR)
            {
                std::string nm(mydirinfo->d_name, mydirinfo->d_namlen);
                if (nm != "." && nm != "..")
                    names.push_back(nm); 
            }
        }
        closedir(mydir);
        //int status = closedir(mydir); // status == 0 means everything OK
    }
    
    return names;
}


/*-----------------------------------------------------------------------
 * Function:    envexpand
 * Description:    expand environment variables in a string
 * Args:    *s: the string
 * Returns:    s
 * Author:    mbp
 * Date:    Fri Feb 14 09:46:22 1992
 * Notes:    expansion is done inplace; there better be enough room!
 */
std::string 
AFileFinder::getEnvVar( const std::string& key ) 
{
    char *val = getenv( key.c_str() );
    return (val == nullptr) ? std::string("") : std::string(val);
}


std::vector<std::string>
AFileFinder::envExpand( const std::string& s )
{
    std::vector<std::string> paths;
    std::string path = s;
    std::string env = getEnvVar("HOME");
    
    int i = 0;
    
    if (path[0] == '~' && !env.empty()) 
    {
        path =  env + s.substr(1);
        i = (int) env.size();
    }
    
    m_stack.push( path );
    
    while (!m_stack.empty())
    {
        path = m_stack.top();
        m_stack.pop();
        while (i < path.size()) 
        {
            if (path[i] == '$') 
            {
                ++i;
                std::string key;
                
                for ( ; isalnum(path[i]) || path[i] == '_'; ++i)
                    key += std::string(1, path[i]);
                
                
                env = getEnvVar(key);
                
                if (env.empty() && m_logger.level() > 0)
                {
                    Message txt1;
                    txt1 << "No environment value for $" << key << " in " << s;
                    m_logger.logMsg( txt1 );
                    m_logger.flush();
                }  

                path += env + path.substr(i);
            }
            else if (path[i] == '*')
            {
                std::string key(path.begin(), path.begin() + i);
                std::vector<std::string> subpath = getSubDirs( key );
                
                for (int j = 0; j < subpath.size(); ++j)
                {
                    subpath[j].insert(subpath[j].begin(), key.begin(), key.end());
                    if (i + 1 < path.size())
                    {
                        subpath[j].append(path.begin() + i + 1, path.end());
                    }
                    m_stack.push(subpath[j]);
                }
                
                if (i + 1 < path.size())
                    path = key + path.substr(i+1);
                else path = key;
                
                ++i;
            } 
            else ++i;

        }
        paths.push_back(path);
    }
    
    return paths;
}

/*-----------------------------------------------------------------------
 * Function:	findfile
 * Description:	resolve a filename into a pathname
 * Args:	*superfile: containing file
 *		*file: file to look for
 * Returns:	pointer to resolved pathname, or NULL if not found
 * Author:	mbp
 * Date:	Wed Feb 12 14:11:47 1992
 * Notes:
 *
 * findfile() tries to locate a (readable) file in the following way.
 *
 *    If file begins with a '/' it is assumed to be an absolute path.  In
 *    this case we expand any environment variables in file and test for
 *    existence, returning a pointer to the expanded path if the file is
 *    readable, NULL otherwise.
 *
 *    Now assume file does not begin with a '/'.
 *
 *    If superfile is non-NULL, we assume it is the pathname of a file
 *    (not a directory), and we look for file in the directory of that
 *    path.  Environment variables are expanded in file but not in
 *    superfile.
 *
 *    If superfile is NULL, or if file isn't found superfile directory,
 *    we look in each of the directories in the array last passed to
 *    filedirs().  Environment variables are expanded in file and in
 *    each of the directories last passed to filedirs().
 *
 *    We return a pointer to a string containing the entire pathname of
 *    the first location where file is found, or NULL if it is not found.
 *
 *    In all cases the returned pointer points to dynamically allocated
 *    space which will be freed on the next call to findfile().
 *
 *    File existence is tested with a call to access(), checking for read
 *    permission.
 */



std::string
AFileFinder::findFile(const std::string& file, const std::string& superfile )
{
    std::string path;
    int trydot = 1;
    
    if (file.empty()) 
        return std::string();
    
    if (file[0] == '/' || file[0] == '$') 
    {
        std::vector<std::string> paths = envExpand(file); 
        
        for (int i = 0; i < paths.size(); ++i)
            if (access(paths[i].c_str(), R_OK) == 0)
                return paths[i];
        
        return std::string();
    }
    
    if (!superfile.empty()) 
    {
        path = dirPrefix(superfile);
        if (path.empty() || (path == "."))
            trydot = 0;

        std::vector<std::string> paths = envExpand(path + file);  
        
        for (int i = 0; i < paths.size(); ++i)
            if (access(paths[i].c_str(), R_OK) == 0)
                return paths[i];
    }
    
    for (int i = 0; i < m_filedirs.size(); ++i) 
    {
        if (m_filedirs[i][0] == '.') 
            trydot = 0;
        
        std::vector<std::string> paths = envExpand(m_filedirs[i] + "/" + file);
        
        for (int j = 0; j < paths.size(); ++j)
            if (access(paths[j].c_str(), R_OK) == 0)
                return paths[j];
    }

    // Implicitly add "." at end of search path
    if (trydot && access(file.c_str(), R_OK) == 0)
        return file;
    
    return std::string();
}

   

