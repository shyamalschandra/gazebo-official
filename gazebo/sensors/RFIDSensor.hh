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
/* Desc: Gazebo RFID Sensor
 * Author: Jonas Mellin & Zakiruz Zaman
 * Date: 6th December 2011
 */

#ifndef _RFIDSENSOR_HH_
#define _RFIDSENSOR_HH_

#include <vector>
#include <string>

#include "gazebo/physics/PhysicsTypes.hh"

#include "gazebo/transport/TransportTypes.hh"

#include "gazebo/math/Pose.hh"

#include "gazebo/sensors/Sensor.hh"

namespace gazebo
{
  /// \ingroup gazebo_sensors
  /// \brief Sensors namespace
  namespace sensors
  {
    class RFIDTag;

    /// \addtogroup gazebo_sensors
    /// \{

    /// \class RFIDSensor RFIDSensor.hh sensors/sensors.hh
    /// \brief Sensor class for RFID type of sensor
    class RFIDSensor: public Sensor
    {
      /// \brief  Constructor
      public: RFIDSensor();

      /// \brief Destructor
      public: virtual ~RFIDSensor();

      /// \brief Load the sensor with SDF parameters
      /// \param[in] _sdf SDF Sensor parameters
      /// \param[in] _worldName Name of world to load from
      public: virtual void Load(const std::string &_worldName,
                                sdf::ElementPtr _sdf);

      /// \brief Load the sensor with default parameters
      /// \param[in] _worldName Name of world to load from
      public: virtual void Load(const std::string &_worldName);

      /// \brief  Initialize the sensor
      public: virtual void Init();

      /// \brief Add RFID tag.
      /// \param[in] _tag RFID tag to add.
      public: void AddTag(RFIDTag *_tag);

      protected: virtual void UpdateImpl(bool _force);

      /// \brief  Finalize the sensor
      public: virtual void Fini();

      /// \brief Iterates through all the RFID tags, and finds the one which
      /// are in range of the sensor.
      private: void EvaluateTags();

      /// \brief Check the range for one RFID tag.
      /// \param[in] _pose Pose of a tag.
      private: bool CheckTagRange(const math::Pose &_pose);

      /// \brief Checks if ray intersects RFID sensor.
      /// \param[in] _pose Pose to compare against.
      /// \return True if intersects, false if not.
      // private: bool CheckRayIntersection(const math::Pose &_pose);

      /// \brief Parent entity
      private: physics::EntityPtr entity;

      /// \brief Unused
      // private: physics::CollisionPtr laserCollision;

      /// \brief Unused
      // private: physics::RayShapePtr laserShape;

      /// \brief Publisher for RFID pose messages.
      private: transport::PublisherPtr scanPub;

      /// \brief All the RFID tags.
      private: std::vector<RFIDTag*> tags;
    };
    /// \}
  }
}
#endif
