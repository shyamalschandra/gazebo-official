/*
 * Copyright 2012 Open Source Robotics Foundation
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

#include <vector>
#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "gazebo/common/Common.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/sdf/interface/Converter.hh"


using namespace sdf;

/////////////////////////////////////////////////
bool Converter::Convert(TiXmlDocument *_doc, const std::string &_toVersion,
                        bool _quiet)
{
  TiXmlElement *elem = _doc->FirstChildElement("gazebo");

  // Replace <gazebo> with <sdf>
  if (elem && boost::lexical_cast<double>(_toVersion) >= 1.3)
  {
    elem->SetValue("sdf");
    std::cout << "Set SDF value\n";
  }
  else if (!elem)
    elem = _doc->FirstChildElement("sdf");

  if (!elem || !elem->Attribute("version"))
  {
    gzerr << "  Unable to determine original SDF version\n";
    return false;
  }

  std::string origVersion = elem->Attribute("version");

  if (origVersion == _toVersion)
    return true;

  if (!_quiet)
  {
    std::cout << gzclr_start(33)
              << "  Version[" << origVersion << "] to Version[" << _toVersion
              << "]\n"
              << "  Please use the gzsdf tool to update your SDF files.\n"
              << "    $ gzsdf convert [sdf_file]\n"
              << gzclr_end;
  }

  elem->SetAttribute("version", _toVersion);

  std::string origVersionStr = origVersion;
  boost::replace_all(origVersion, ".", "_");

  std::string filename = gazebo::common::find_file(
      std::string("sdf/") + _toVersion + "/" + origVersion + ".convert");


  // Use convert file in the current sdf version folder for conversion. If file
  // does not exist, then find intermediate convert files and iteratively
  // convert the sdf elem. Ideally, users should use gzsdf convert so that the
  // latest sdf versioned file is written and no subsequent conversions are
  // necessary.
  TiXmlDocument xmlDoc;
  if (!xmlDoc.LoadFile(filename))
  {
    // find all sdf version dirs in gazebo resource path
    std::string sdfPath = gazebo::common::find_file(std::string("sdf/"), false);
    boost::filesystem::directory_iterator endIter;
    std::set<boost::filesystem::path> sdfDirs;
    if (boost::filesystem::exists(sdfPath)
        && boost::filesystem::is_directory(sdfPath))
    {
      for (boost::filesystem::directory_iterator dirIter(sdfPath);
          dirIter != endIter ; ++dirIter)
      {
        if (boost::filesystem::is_directory(dirIter->status()))
        {
          if (boost::algorithm::ilexicographical_compare(
              origVersionStr, (*dirIter).path().filename().string()))
          {
            sdfDirs.insert((*dirIter));
          }
        }
      }
    }

    // loop through sdf dirs and do the intermediate conversions
    for (std::set<boost::filesystem::path>::iterator it = sdfDirs.begin();
        it != sdfDirs.end(); ++it)
    {
      boost::filesystem::path convertFile
         = boost::filesystem::operator/((*it).string(), origVersion+".convert");
      if (boost::filesystem::exists(convertFile))
      {
        if (!xmlDoc.LoadFile(convertFile.string()))
        {
            gzerr << "Unable to load file[" << convertFile << "]\n";
            return false;
        }
        ConvertImpl(elem, xmlDoc.FirstChildElement("convert"));
        if ((*it).filename() == _toVersion)
          return true;

        origVersion = (*it).filename().string();
        boost::replace_all(origVersion, ".", "_");
      }
      else
      {
        continue;
      }
    }
    gzerr << "Unable to convert from SDF version " << origVersionStr
        << " to " << _toVersion << "\n";
    return false;
  }

  ConvertImpl(elem, xmlDoc.FirstChildElement("convert"));

  return true;
}

/////////////////////////////////////////////////
void Converter::Convert(TiXmlDocument *_doc, TiXmlDocument *_convertDoc)
{
  ConvertImpl(_doc->FirstChildElement(), _convertDoc->FirstChildElement());
}

/////////////////////////////////////////////////
void Converter::ConvertImpl(TiXmlElement *_elem, TiXmlElement *_convert)
{
  CheckDeprecation(_elem, _convert);

  for (TiXmlElement *convertElem = _convert->FirstChildElement("convert");
       convertElem; convertElem = convertElem->NextSiblingElement("convert"))
  {
    TiXmlElement *elem = NULL;
    elem = _elem->FirstChildElement(convertElem->Attribute("name"));
    while (elem)
    {
      ConvertImpl(elem, convertElem);
      elem = elem->NextSiblingElement(convertElem->Attribute("name"));
    }
  }

  for (TiXmlElement *renameElem = _convert->FirstChildElement("rename");
       renameElem; renameElem = renameElem->NextSiblingElement("rename"))
  {
    Rename(_elem, renameElem);
  }

  for (TiXmlElement *moveElem = _convert->FirstChildElement("move");
     moveElem; moveElem = moveElem->NextSiblingElement("move"))
  {
    Move(_elem, moveElem);
  }
}

/////////////////////////////////////////////////
void Converter::Rename(TiXmlElement *_elem, TiXmlElement *_renameElem)
{
  TiXmlElement *fromConvertElem = _renameElem->FirstChildElement("from");
  TiXmlElement *toConvertElem = _renameElem->FirstChildElement("to");

  const char *fromElemName = fromConvertElem->Attribute("element");
  const char *fromAttrName = fromConvertElem->Attribute("attribute");

  const char *toElemName = toConvertElem->Attribute("element");
  const char *toAttrName = toConvertElem->Attribute("attribute");

  const char *value = GetValue(fromElemName, fromAttrName, _elem);
  if (!value)
    return;

  if (!toElemName)
  {
    gzerr << "No 'to' element name specified\n";
    return;
  }

  TiXmlElement *replaceTo = new TiXmlElement(toElemName);
  if (toAttrName)
    replaceTo->SetAttribute(toAttrName, value);
  else
  {
    TiXmlText *text = new TiXmlText(value);
    replaceTo->LinkEndChild(text);
  }

  if (fromElemName)
  {
    TiXmlElement *replaceFrom = _elem->FirstChildElement(fromElemName);
    _elem->ReplaceChild(replaceFrom, *replaceTo);
  }
  else if (fromAttrName)
  {
    _elem->RemoveAttribute(fromAttrName);
    _elem->LinkEndChild(replaceTo);
  }
}

/////////////////////////////////////////////////
void Converter::Move(TiXmlElement *_elem, TiXmlElement *_moveElem)
{
  TiXmlElement *fromConvertElem = _moveElem->FirstChildElement("from");
  TiXmlElement *toConvertElem = _moveElem->FirstChildElement("to");

  const char *fromElemStr = fromConvertElem->Attribute("element");
  const char *fromAttrStr = fromConvertElem->Attribute("attribute");

  const char *toElemStr = toConvertElem->Attribute("element");
  const char *toAttrStr = toConvertElem->Attribute("attribute");

  // tokenize 'from' and 'to' strs
  std::string fromStr = "";
  if (fromElemStr)
    fromStr = fromElemStr;
  else if (fromAttrStr)
    fromStr = fromAttrStr;
  std::string toStr = "";
  if (toElemStr)
    toStr = toElemStr;
  else if (toAttrStr)
    toStr = toAttrStr;
  std::vector<std::string> fromTokens;
  std::vector<std::string> toTokens;
  boost::algorithm::split_regex(fromTokens, fromStr, boost::regex("::"));
  boost::algorithm::split_regex(toTokens, toStr, boost::regex("::"));

  if (fromTokens.size() == 0)
  {
    gzerr << "Incorrect 'from' string format\n";
    return;
  }
  if (toTokens.size() == 0)
  {
    gzerr << "Incorrect 'to' string format\n";
    return;
  }

  // get value of the 'from' element/attribute
  TiXmlElement *fromElem = _elem;
  for (unsigned int i = 0; i < fromTokens.size()-1; ++i)
  {
    fromElem = fromElem->FirstChildElement(fromTokens[i]);
    if (!fromElem)
    {
      gzerr << "Cannot find element: '" << fromTokens[i]
          << "' in from string: '" << fromStr << "'\n";
      return;
    }
  }

  const char *fromName = fromTokens[fromTokens.size()-1].c_str();
  const char *value = NULL;
  if (fromElemStr)
    value = GetValue(fromName, NULL, fromElem);
  else if (fromAttrStr)
    value = GetValue(NULL, fromName, fromElem);

  if (!value)
  {
    gzerr << "Element/attribute: '" << fromName << "' does not have a value\n";
    return;
  }

  // get the new element/attribute name
  const char *toName = toTokens[toTokens.size()-1].c_str();
  TiXmlElement *toElem = _elem;
  for (unsigned int i = 0; i < toTokens.size()-1; ++i)
  {
    toElem = toElem->FirstChildElement(toTokens[i]);
    if (!toElem)
    {
      gzerr << "Cannot find element: '"<< toTokens[i] << "' in to string: '"
          << toStr << "'\n";
      return;
    }
  }

  // move by creating a new element/attribute and deleting the old one
  if (toElemStr)
  {
    TiXmlElement *moveTo = new TiXmlElement(toName);
    TiXmlText *text = new TiXmlText(value);
    moveTo->LinkEndChild(text);
    toElem->LinkEndChild(moveTo);
  }
  else if (toAttrStr)
  {
    toElem->SetAttribute(toName, value);
  }
  if (fromElemStr)
  {
    TiXmlElement *moveFrom =
        fromElem->FirstChildElement(fromName);
    fromElem->RemoveChild(moveFrom);
  }
  else if (fromAttrStr)
  {
    fromElem->RemoveAttribute(fromName);
  }
}

/////////////////////////////////////////////////
const char *Converter::GetValue(const char *_valueElem, const char *_valueAttr,
                                TiXmlElement *_elem)
{
  if (_valueElem)
  {
    // Check to see if the element that is being converted has the value
    if (!_elem->FirstChildElement(_valueElem))
      return NULL;

    if (_valueAttr)
      return _elem->FirstChildElement(_valueElem)->Attribute(_valueAttr);
    else
      return _elem->FirstChildElement(_valueElem)->GetText();
  }
  else if (_valueAttr)
  {
    return _elem->Attribute(_valueAttr);
  }

  return NULL;
}

/////////////////////////////////////////////////
void Converter::CheckDeprecation(TiXmlElement *_elem, TiXmlElement *_convert)
{
  // Process deprecated elements
  for (TiXmlElement *deprecatedElem = _convert->FirstChildElement("deprecated");
      deprecatedElem;
      deprecatedElem = deprecatedElem->NextSiblingElement("deprecated"))
  {
    std::string value = deprecatedElem->GetText();
    std::vector<std::string> valueSplit;
    boost::split(valueSplit, value, boost::is_any_of("/"));

    bool found = false;
    TiXmlElement *e = _elem;
    std::ostringstream stream;

    std::string prefix = "";
    for (unsigned int i = 0; i < valueSplit.size() && !found; ++i)
    {
      if (e->FirstChildElement(valueSplit[i]))
      {
        if (stream.str().size() != 0)
        {
          stream << ">\n";
          prefix += "  ";
        }

        stream << prefix << "<" << valueSplit[i];
        e = e->FirstChildElement(valueSplit[i]);
      }
      else if (e->Attribute(valueSplit[i]))
      {
        stream << " " << valueSplit[i] << "='"
               << e->Attribute(valueSplit[i].c_str()) << "'";
        found = true;
      }
    }

    gzwarn << "Deprecated SDF Values in original file:\n"
           << stream.str() << "\n\n";
  }
}
