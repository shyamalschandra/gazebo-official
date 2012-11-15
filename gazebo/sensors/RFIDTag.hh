/* Copyright (C) 2012
 *     Jonas Mellin & Zakiruz Zaman
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
 */
/* Desc: RFID Tag
 * Author: Jonas Mellin & Zakiruz Zaman
 * Date: 6th December 2011
 */

#ifndef _RFIDTAG_HH_
#define _RFIDTAG_HH_

#include <vector>
#include <string>

#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/sensors/Sensor.hh"
#include "gazebo/math/gzmath.hh"
#include "gazebo/physics/physics.hh"

namespace gazebo
{
  namespace sensors
  {
    /// \addtogroup gazebo_sensors
    /// \{

    /// \class RFIDTag RFIDTag.hh sensors/sensors.hh
    /// \brief RFIDTag to interact with RFIDTagSensors
    class RFIDTag: public Sensor
    {
      /// \brief Constructor.
      public: RFIDTag();

      /// \brief Destructor.
      public: virtual ~RFIDTag();

      /// \brief Load the sensor with SDF parameters.
      /// \param[in] _sdf SDF Sensor parameters.
      /// \param[in] _worldName Name of world to load from.
      public: virtual void Load(const std::string & _worldName,
                                sdf::ElementPtr &_sdf);

      /// \brief Load the sensor with default parameters.
      /// \param[in] _worldName Name of world to load from.
      public: virtual void Load(const std::string & _worldName);

      /// \brief Initialize the sensor.
      public: virtual void Init();

      /// \brief Update the sensor information.
      /// \param[in] _force True if update is forced, false if not.
      protected: virtual void UpdateImpl(bool _force);

      /// \brief Finalize the sensor.
      public: virtual void Fini();

      /// \brief Returns pose of tag in world coordinate.
      /// \return Pose of object.
      public: math::Pose GetTagPose() const
              {return entity->GetWorldPose();}

      /// \brief Pointer the entity that has the RFID tag.
      private: physics::EntityPtr entity;

      /// \brief Publisher for tag pose messages.
      private: transport::PublisherPtr scanPub;
    };
    /// \}
  }
}
#endif
