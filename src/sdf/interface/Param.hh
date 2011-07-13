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
/* Desc: A parameter
 * Author: Nate Koenig
 * Date: 14 Aug 2008
 */

#ifndef SDF_PARAM_HH
#define SDF_PARAM_HH

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include <string>

#include "common/Console.hh"
#include "common/Color.hh"
#include "math/Vector3.hh"
#include "math/Pose.hh"
#include "math/Quaternion.hh"

namespace sdf
{
  class Param;
  typedef boost::shared_ptr< Param > ParamPtr;
  typedef std::vector< ParamPtr > Param_V;

  class Param
  {
    /// \brief Constructor
    public: Param(Param *_newParam);
  
    /// \brief Destructor
    public: virtual  ~Param();

    /// \brief Begin a block of "new Param<*>"
    public: static void Begin(std::vector<Param*> *_params);
  
    /// \brief End a block of "new Param<*>"
    public: static void End();

    /// \brief Find a parameter by name
    public: static ParamPtr Find(Param_V &_params, const std::string &key);

    /// \brief Get the type
    public: virtual std::string GetAsString() const 
            {return std::string();}
  
    public: virtual std::string GetDefaultAsString() const 
            {return std::string();}

    /// \brief Set the parameter value from a string
    public: virtual bool SetFromString(const std::string &) 
            {return true;}

    /// \brief Reset the parameter
    public: virtual void Reset() = 0;

    public: const std::string &GetKey() const {return this->key;} 

    public: std::string GetTypeName() const;
    public: bool GetRequired() const { return this->required; }

    /// \brief Return true if the parameter has been set
    public: bool GetSet() const { return this->set; }

    public: virtual boost::shared_ptr<Param> Clone() const = 0;

    public: bool IsBool() const;
    public: bool IsInt() const;
    public: bool IsUInt() const;
    public: bool IsFloat() const;
    public: bool IsDouble() const;
    public: bool IsChar() const;
    public: bool IsStr() const;
    public: bool IsVector3() const;
    public: bool IsQuaternion() const;
    public: bool IsPose() const;
    public: bool IsColor() const;

    public: bool Set(const bool &_value);
    public: bool Set(const int &_value);
    public: bool Set(const unsigned int &_value);
    public: bool Set(const float &_value);
    public: bool Set(const double &_value);
    public: bool Set(const char &_value);
    public: bool Set(const std::string &_value);
    public: bool Set(const char *_value);
    public: bool Set(const gazebo::math::Vector3 &_value);
    public: bool Set(const gazebo::math::Quaternion &_value);
    public: bool Set(const gazebo::math::Pose &_value);
    public: bool Set(const gazebo::common::Color &_value);
 
    public: bool Get(bool &_value);
    public: bool Get(int &_value);
    public: bool Get(unsigned int &_value);
    public: bool Get(float &_value);
    public: bool Get(double &_value);
    public: bool Get(char &_value);
    public: bool Get(std::string &_value);
    public: bool Get(gazebo::math::Vector3 &_value);
    public: bool Get(gazebo::math::Quaternion &_value);
    public: bool Get(gazebo::math::Pose &_value);
    public: bool Get(gazebo::common::Color &_value);
 
    /// List of created parameters
    private: static std::vector<Param*> *params;
  
    protected: std::string key; 
    protected: bool required;
    protected: bool set;
    protected: std::string typeName;
  };
  
  
  template< typename T>
  class ParamT : public Param
  {
    /// \brief Constructor
    public: ParamT(const std::string &_key, const std::string &_default, 
                   bool _required)
            : Param(this)
    {
      this->key = _key;
      this->required = _required;
      this->typeName = typeid(T).name();

      this->Set(_default);
      this->defaultValue = this->value;
      this->set = false;
    }
   
    /// \brief Destructor
    public: virtual ~ParamT() {}
 
    /// \brief Get the parameter value as a string
    public: virtual std::string GetAsString() const
    {
       return boost::lexical_cast<std::string>(this->value);
    }

    /// \brief Set the parameter value from a string
    public: virtual bool SetFromString(const std::string &_value) 
    { return this->Set(_value); }


    public: virtual std::string GetDefaultAsString() const
    {
      return boost::lexical_cast<std::string>(this->defaultValue);
    }

    /// \brief Set the parameter value from a string
    public: virtual bool Set( const std::string &_str )
    {
      if (_str.empty() && this->required)
      {
        gzerr << "Empty string used when setting a required parameter. Key[" << this->GetKey() << "]\n";
        return false;
      }
      else if (_str.empty())
      {
        this->value = this->defaultValue;
        return true;
      }

      std::string tmp = _str;
      std::string lowerTmp = _str;
      boost::to_lower(lowerTmp);

      // "true" and "false" doesn't work properly
      if (lowerTmp == "true")
        tmp = "1";
      else if (lowerTmp == "false")
        tmp = "0";

      try
      {
        this->value = boost::lexical_cast<T>(tmp);
      }
      catch (boost::bad_lexical_cast &e)
      {
        if (_str == "inf" || _str == "-inf")
        {
          // in this case, the parser complains, but seems to assign the 
          // right values
          gzmsg << "INFO [sdf::Param]: boost throws when lexical casting inf's, but the values are usually passed through correctly\n";
        }
        else
        {
          gzerr << "Unable to set value [" <<  _str << "] for key[" << this->key << "]\n";
          return false;
        }
      }

      this->set = true;
      return this->set;
    } 

    /// \brief Get the value
    public: T GetValue() const
    {
      return this->value;
    }

    /// \brief Set the value of the parameter
    public: void SetValue(const T &_value)
    {
      this->value = _value;
      this->set = true;
    }

    /// \brief Reset to default value
    public: virtual void Reset()
    {
      this->value = this->defaultValue;
      this->set = false;
    }

    public: virtual boost::shared_ptr<Param> Clone() const
            {
              boost::shared_ptr<ParamT<T> > clone(new ParamT<T>(this->GetKey(),this->GetDefaultAsString(), this->required ) );
              return clone;
            }

    public: inline T operator*() const {return value;}
  
    public: friend std::ostream &operator<<( std::ostream &_out, 
                                             const ParamT<T> &_p)
            {
              _out << _p.value;
              return _out;
            }
  
    private: T value;
    private: T defaultValue;
  };

}
#endif
