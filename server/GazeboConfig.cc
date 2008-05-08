/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/* Desc: Local Gazebo configuration 
 * Author: Jordi Polo
 * Date: 3 May 2008
 * SVN: $Id:$
 */
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "XMLConfig.hh"
#include "GazeboConfig.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
GazeboConfig::GazeboConfig()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
GazeboConfig::~GazeboConfig()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Loads the configuration file 
void GazeboConfig::Load()
{
  std::ifstream cfgFile;

  std::string rcFilename = getenv("HOME");
  rcFilename += "/.gazeborc";

  cfgFile.open(rcFilename.c_str(), std::ios::in);

  if (cfgFile)
  {
    XMLConfig rc;
    XMLConfigNode *node;
    rc.Load(rcFilename);

    node = rc.GetRootNode()->GetChild("gazeboPath");
    while (node)
    {
      std::cout << "Gazebo Path[" << node->GetValue() << "]\n";
      this->gazeboPaths.push_back(node->GetValue());
      node = node->GetNext("gazeboPath");
    }

    node = rc.GetRootNode()->GetChild("ogrePath");
    while (node)
    {
      std::cout << "Ogre Path[" << node->GetValue() << "]\n";
      this->ogrePaths.push_back( node->GetValue() );
      node = node->GetNext("ogrePath");
    }
    this->RTTMode = rc.GetRootNode()->GetString("RTTMode", "PBuffer");

  }
  else
  {
    std::cout << "Unable to find the file ~/.gazeborc. Using default paths. This may cause OGRE to fail.\n";
    this->gazeboPaths.push_back("/usr/local/share/gazebo");
    this->ogrePaths.push_back("/usr/local/lib/OGRE");
    this->ogrePaths.push_back("/usr/lib/OGRE");
    this->RTTMode="PBuffer";
  }
}

std::list<std::string> GazeboConfig::GetGazeboPaths() const
{
  return this->gazeboPaths;
}

std::list<std::string> GazeboConfig::GetOgrePaths() const
{
  return this->ogrePaths;
}

std::string GazeboConfig::GetRTTMode() const
{
  return this->RTTMode;
}

