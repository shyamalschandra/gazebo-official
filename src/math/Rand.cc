/*
 * Copyright 2011 Nate Koenig & Andrew Howard
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
/* Desc: Random number generator
 * Author: Nate Koenig
 * Date: 27 May 2009
 * SVN: $Id:$
 */

#include <ctime>
#include <sys/types.h>
#include <unistd.h>

#include "math/Rand.hh"

using namespace gazebo;
using namespace math;


// We don't seed with time for the cases when two processes are started the
// same time (this mostly happens with launch scripts that start a server
// and gui simultaneously).
GeneratorType *Rand::randGenerator = new GeneratorType(getpid());

///////////////////////////////////////////////////////////////////////////////
// Constructor
Rand::Rand()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
Rand::~Rand()
{
}

///////////////////////////////////////////////////////////////////////////////
/// Get a double from a uniform distribution
double Rand::GetDblUniform(double min, double max)
{
  URealGen gen(*randGenerator, UniformRealDist(min, max));
 
  return gen(); 
}

///////////////////////////////////////////////////////////////////////////////
/// Get a double from a normal distribution
double Rand::GetDblNormal(double mean, double sigma)
{
  NRealGen gen(*randGenerator, NormalRealDist(mean, sigma));
 
  return gen(); 
}

///////////////////////////////////////////////////////////////////////////////
/// Get a integer from a uniform distribution
int Rand::GetIntUniform(int min, int max)
{
  UIntGen gen(*randGenerator, UniformIntDist(min,max));
 
  return gen(); 
}

///////////////////////////////////////////////////////////////////////////////
/// Get a double from a normal distribution
int Rand::GetIntNormal(int mean, int sigma)
{
  NRealGen gen(*randGenerator, NormalRealDist(mean, sigma));
 
  return (int)(gen()); 
}
