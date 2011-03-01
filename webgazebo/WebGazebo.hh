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

/* Desc: HTTP portal to libgazebo
 * Author: Brian Gerkey, Richard Vaughan, Nate Koenig, Abbas Sadat
 * Date: 9 March 2009
 * SVN: $Id: gazebo.h 7398 2009-03-09 07:21:49Z natepak $
 */

#include <gazebo.h>
#include <websim.hh>

class WebGazebo : public websim::WebSim
{
public:
  WebGazebo(const std::string& fedfile,
	    const std::string& host, unsigned short port,
	    double dtol, double atol);
  virtual ~WebGazebo();

  bool Go(double t);
  
  // start WebSim Interface ===================================================
  
  virtual std::string IdentificationString()
  { return "WebGazebo"; }
  
  virtual std::string VersionString()
  {  return "0.1"; }
  
  virtual bool CreateModel(const std::string& name, 
			   const std::string& type,
			   std::string& error);

  virtual bool DeleteModel(const std::string& name,
			   std::string& error);

  virtual bool SetModelPVA(const std::string& name, 
			   const websim::Pose& p,
			   const websim::Velocity& v,
			   const websim::Acceleration& a,
			   std::string& error);

  virtual bool GetModelPVA(const std::string& name, 
			   websim::Time &t,
			   websim::Pose& p,
			   websim::Velocity& v,
			   websim::Acceleration& a,
			   std::string& error);

  virtual bool GetModelType(const std::string& name, std::string& type);

  virtual bool GetModelData(const std::string& name, 
			    std::string& response,
			    websim::Format format,
			    void* xmlnode );
  
  virtual bool GetModelChildren(const std::string& model, 
				std::vector<std::string>& children);
  
  virtual bool GetModelGeometry(const std::string& name,
			      double& bx,
			      double& by,
			      double& bz,
			      websim::Pose& center,
			      std::string& response);

  virtual websim::Time GetTime();

  virtual bool ClockStart() { simIface->Unpause(); return true;}

  virtual bool ClockStop() { simIface->Pause(); return true;}

  virtual bool ClockRunFor( uint32_t msec );

  // end WebSim Interface ====================================================


private: // all private members are specific to WebGazebo


  //bool WaitForResponse();

  double distance_tolerance;
  double angle_tolerance;
  
  boost::mutex goMutex;
  boost::condition goCond;
  
  gazebo::Client *client;
  gazebo::SimulationIface *simIface;
  gazebo::FactoryIface *factoryIface;

  std::map<std::string,gazebo::Iface*> interfaces;

  // Available models
  std::map<std::string,int> models;

  //bool CheckTolerances(gazebo::Pose p, gazebo::Pose q);

  bool GetModel(const std::string& name,
		const std::string& type,
		std::string& xmldata,
		std::string& response);

  void GoCallback();
};

