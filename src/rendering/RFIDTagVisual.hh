/*   Copyright (C)
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
 *
 */
/* Desc: 
 * Author: Jonas Mellin & Zakiruz Zaman 
 * Date: 6th December 2011
 */

#ifndef RFIDTAGVISUAL_HH
#define RFIDTAGVISUAL_HH

#include <string>

#include "rendering/Visual.hh"
#include "msgs/MessageTypes.hh"
#include "transport/TransportTypes.hh"

namespace gazebo
{
  namespace rendering
  {
    class RFIDTagVisual : public Visual
    {
      public: RFIDTagVisual(const std::string &_name, VisualPtr _vis,
                            const std::string &_topicName);

      public: virtual ~RFIDTagVisual();

      private: void OnScan(ConstPosePtr &_msg);

      private: transport::NodePtr node;
      private: transport::SubscriberPtr laserScanSub;
    };
  }
}
#endif
