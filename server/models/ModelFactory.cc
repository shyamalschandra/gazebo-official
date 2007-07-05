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
 *
 */
/*
 * Desc: Factory for creating models
 * Author: Andrew Howard
 * Date: 18 May 2003
 * CVS info: $Id$
 */

#include "Model.hh"
#include "ModelFactory.hh"

using namespace gazebo;

void RegisterLightSource();
//void RegisterObserverCam();
//void RegisterGraphics3d();

std::map<std::string, ModelFactoryFn> ModelFactory::models;

// Register all known models.
void ModelFactory::RegisterAll()
{
  // Register static models (macro generated by autoconf)
  RegisterLightSource();
//  RegisterObserverCam();
//  RegisterGraphics3d();
}


// Register a model class.  Use by dynamically loaded modules
void ModelFactory::RegisterModel(std::string type, std::string classname,
                                 ModelFactoryFn factoryfn)
{
  models[classname] = factoryfn;
}


// Create a new instance of a model.  Used by the world when reading
// the world file.
Model *ModelFactory::NewModel(const std::string &classname)
{  
  if (models[classname])
  {
    return (models[classname]) ();
  }

  return NULL;
}
