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
/* Desc: A diagnostic class
 * Author: Nate Koenig
 * Date: 2 Feb 2011
 */

#ifndef DIAGNOSTICMANAGER_HH
#define DIAGNOSTICMANAGER_HH

#include "SingletonT.hh"
#include "Timer.hh"

namespace gazebo
{
  class DiagnosticTimer;

  /// \brief A diagnostic manager class
  class DiagnosticManager : public SingletonT<DiagnosticManager>
  {
    /// \brief Constructor
    private: DiagnosticManager();

    /// \brief Destructor
    private: virtual ~DiagnosticManager();

    /// \brief A diagnostic timer has started
    public: void TimerStart(DiagnosticTimer *timer);

    /// \brief A diagnostic timer has stoped
    public: void TimerStop(DiagnosticTimer *timer);

    /// \brief Get the number of timers
    public: int GetTimerCount() const;

    /// \brief Get a specific time
    public: Time GetTime(int index) const;

    /// \brief Get a time based on a label
    public: Time GetTime(const std::string &label) const;

    /// \brief Get a label for a timer
    public: std::string GetLabel(int index) const;

    private: std::map<std::string, Time> timers;

    //Singleton implementation
    private: friend class DestroyerT<DiagnosticManager>;
    private: friend class SingletonT<DiagnosticManager>;
  };

  /// \brief A timer designed for diagnostics
  class DiagnosticTimer : public Timer
  {
    /// \brief Constructor
    public: DiagnosticTimer(const std::string &name) : Timer() 
            {
              this->Start();
              this->name = name; 
              DiagnosticManager::Instance()->TimerStart(this);
            }

    /// \brief Destructor
    public: virtual ~DiagnosticTimer() 
            { 
              DiagnosticManager::Instance()->TimerStop(this);
            }

    public: std::string GetName() const 
            { return this->name; }

    private: std::string name;
  };
}
 
#endif
