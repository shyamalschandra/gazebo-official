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
/*
 * Desc: Gazebo Console messages
 * Author: Nathan Koenig
 * Date: 09 June 2007
 */

//#include <time.h>
//#include <string.h>
#include <sstream>

#include "common/Param.hh"
#include "common/XMLConfig.hh"
#include "common/Exception.hh"
#include "common/Console.hh"

using namespace gazebo;
using namespace common;

Console *Console::myself = NULL;

////////////////////////////////////////////////////////////////////////////////
/// Default constructor
Console::Console()
{
  this->msgStream = &std::cout;
  this->errStream = &std::cerr;

  Param::Begin(&this->parameters);
  this->quietP = new ParamT<int>("quiet",false,0);
  this->logDataP = new ParamT<bool>("log_data",false,0);
  Param::End();
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
Console::~Console()
{
  delete this->quietP;
  delete this->logDataP;
}

////////////////////////////////////////////////////////////////////////////////
/// Return an instance to this class
Console *Console::Instance()
{
  if (myself == NULL)
    myself = new Console();

  return myself;
}

////////////////////////////////////////////////////////////////////////////////
// Load the Message parameters
void Console::Load(XMLConfigNode *node)
{
  char logFilename[50];

  this->quietP->Load(node);
  this->logDataP->Load(node);

  if (**(this->logDataP))
  {
    time_t t;
    struct tm *localTime;
    char baseFilename[50];

    time(&t);
    localTime = localtime(&t);

    strftime(baseFilename, sizeof(baseFilename),
             "gazebo-%Y_%m_%d_%H_%M", localTime);

    snprintf(logFilename, sizeof(logFilename), "%s.log", baseFilename);
  }
  else
  {
    strcpy(logFilename,"/dev/null");
  }

  this->logStream.open(logFilename, std::ios::out);
}

////////////////////////////////////////////////////////////////////////////////
// Save in xml format
void Console::Save(std::string &prefix, std::ostream &stream)
{
  stream << prefix << *(this->quietP) << "\n";
  stream << prefix << *(this->logDataP) << "\n";
}

////////////////////////////////////////////////////////////////////////////////
/// Set the verbosity
void Console::SetQuiet( bool q )
{
  this->quietP->SetValue( q );
}

////////////////////////////////////////////////////////////////////////////////
/// Get the message stream
std::ostream &Console::ColorMsg(const std::string &lbl, int color)
{
  if (**this->quietP)
    return this->nullStream;
  else
  {
    *this->msgStream << "\033[1;" << color << "m" << lbl <<  "\033[0m ";
    return *this->msgStream;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Get the error stream
std::ostream &Console::ColorErr(const std::string &lbl, const std::string &file, unsigned int line, int color)
{
  int index = file.find_last_of("/") + 1;

  *this->errStream << "\033[1;" << color << "m" << lbl << " [" << file.substr( index , file.size() - index)<< ":" << line << "]\033[0m ";

  return *this->errStream;
}

////////////////////////////////////////////////////////////////////////////////
// Log a message
std::ofstream &Console::Log()
{
  return this->logStream;
}
