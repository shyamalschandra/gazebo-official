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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <libgazebo/gz.h>

libgazebo::Client *client = NULL;
libgazebo::SimulationIface *simIface = NULL;

std::string test_name="Pendulum Benchmark";
std::string xlabel = "Pendulum Count";
std::string ylabel = "Simtime / Realtime";
std::string data_filename = "/tmp/" + test_name + ".data";

void make_plot()
{
  std::ostringstream cmd;

  cmd << "echo \"";
  cmd << "set xlabel '" << xlabel << "'\n";
  cmd << "set ylabel '" << ylabel << "'\n";
  cmd << "set title '" << test_name << "'\n";
  cmd << "set terminal png\n";
  cmd << "set output '" << test_name << ".png'\n";
  cmd << "plot '" << data_filename << "' with lines\n";
  cmd << "\" | gnuplot";

  if (system(cmd.str().c_str() ) < 0)
    std::cerr << "Error\n";
}

int main(int argc, char **argv)
{
  client = new libgazebo::Client();
  simIface = new libgazebo::SimulationIface();

  try
  {
    client->ConnectWait(0, GZ_CLIENT_ID_USER_FIRST);
  }
  catch(std::string e)
  {
    std::cerr << "Gazebo Error: Unable to connect: " << e << "\n";
    return -1;
  }

  /// Open the sim iface
  try
  {
    simIface->Open(client, "default");
  }
  catch (std::string e)
  {
    std::cerr << "Gazebo error: Unable to connect to sim iface:" << e << "\n";
    return -1;
  }

  //FILE *out = fopen(data_filename.c_str(), "w");

  /*if (!out)
    std::cerr << "Unable to open log file";
    */

  //double simTime = 0;
  //double realTime = 0;

  libgazebo::Vec3 linearVel, angularVel, linearAccel, angularAccel;
  libgazebo::Pose modelPose;

  bool done = false;
  while (!done)
  {
    //simIface->SetAngularVel("box_model", libgazebo::Vec3(0,0,vel));
    
    simIface->GetState("base_model::swing_body", modelPose, linearVel, angularVel, 
                       linearAccel, angularAccel);
    printf("Pos[%4.2f %4.2f %4.2f] RPY[%4.2f %4.2f %4.2f] ", modelPose.pos.x, modelPose.pos.y, modelPose.pos.z, modelPose.roll, modelPose.pitch, modelPose.yaw);
    printf("LV[%4.2f %4.2f %4.2f] ",linearVel.x, linearVel.y, linearVel.z );
    printf("AV[%4.2f %4.2f %4.2f] ",angularVel.x, angularVel.y, angularVel.z);
    printf("LA[%4.2f %4.2f %4.2f] ",linearAccel.x, linearAccel.y, linearAccel.z);
    printf("AA[%4.2f %4.2f %4.2f]\n",angularAccel.x, angularAccel.y, angularAccel.z );
  }

  //double percent = simTime / realTime;
  //fprintf(out,"%f\n",percent);

  //fclose(out);

  //make_plot();

  simIface->Close();
  client->Disconnect();
}
