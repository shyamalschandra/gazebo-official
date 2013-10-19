/*
 * Copyright 2013 Open Source Robotics Foundation
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

#include <algorithm>
#include <gdal/ogr_spatialref.h>

#include "gazebo/common/Console.hh"
#include "gazebo/common/SDTS.hh"
#include "gazebo/math/Angle.hh"

#define RAD2METER ((180. / M_PI) * 60. * 1852.)

using namespace gazebo;
using namespace common;

//////////////////////////////////////////////////
SDTS::SDTS(const std::string &_filename)
{
  GDALAllRegister();

  this->poDataset = reinterpret_cast<GDALDataset *>
      (GDALOpen(_filename.c_str(), GA_ReadOnly));

  if (this->poDataset == NULL)
    gzthrow("Unable to find SDTS file [" + _filename + "]\n");

  if (this->poDataset->GetRasterCount() != 1 &&
      this->poDataset->GetRasterCount() != 3)
    gzthrow("Unsupported band number in file [" << _filename + "]. Found " <<
       this-> poDataset->GetRasterCount() <<
       " but only 1 or 3 are valid values\n");


  // Define Geographic coordinate system - set it to WGS84.
  /*OGRSpatialReference *poSRS_Geog = new OGRSpatialReference();
  poSRS_Geog->importFromEPSG(4326); // WGS84

  // Define Projected coordinate system - set to the GeoTransform.
  const char *sProj = this->poDataset->GetProjectionRef();
  OGRSpatialReference *poSRS_Proj = new OGRSpatialReference(sProj);

  // Set up the coordinate transform (projected-to-geographic).
  OGRCoordinateTransformation *poCT_Proj2Geog;
  poCT_Proj2Geog = OGRCreateCoordinateTransformation(poSRS_Proj, poSRS_Geog);

  // Now everything is set up and we set transforming coordinates!
  // Pass Lon/Lat coordinates to the Transform function:
  double x = 0;
  double y = 0;
  poCT_Proj2Geog->Transform( 1, &x, &y );*/

  double upperLeftX = 0.0;
  double upperLeftY = 0.0;
  double upperRightX = this->poDataset->GetRasterXSize();
  double upperRightY = 0.0;
  double lowerLeftX = 0.0;
  double lowerLeftY = this->poDataset->GetRasterYSize();
  double upperLeftXgeo, upperLeftYgeo;
  double upperRightXgeo, upperRightYgeo;
  double lowerLeftXgeo, lowerLeftYgeo;

  // Calculate the georeferenced coordinates
  this->GetGeoReference(upperLeftX, upperLeftY, upperLeftXgeo, upperLeftYgeo);
  this->GetGeoReference(upperRightX, upperRightY, upperRightXgeo, upperRightYgeo);
  this->GetGeoReference(lowerLeftX, lowerLeftY, lowerLeftXgeo, lowerLeftYgeo);
  
  /*std::cout << "Upper left: (" << upperLeftXgeo << "," << upperLeftYgeo << ")\n";
  std::cout << "Upper right: (" << upperRightXgeo << "," << upperRightYgeo << ")\n";
  std::cout << "Lower left: (" << lowerLeftXgeo << "," << lowerLeftYgeo << ")\n";*/

  // Set the world width and height
  this->worldWidth = this->OgrDistance(upperLeftYgeo, upperLeftXgeo,
                                       upperRightYgeo, upperRightXgeo);

  this->worldHeight = this->OgrDistance(upperLeftYgeo, upperLeftXgeo,
                                        lowerLeftYgeo, lowerLeftXgeo);

  /*std::cout << "World width: " << this->worldWidth << std::endl;
  std::cout << "World height: " << this->worldHeight << std::endl;*/
}

void SDTS::GetGeoReference(double _pixel, double _line,
                           double &_xGeo, double &_yGeo)
{
  double adfGeoTransform[6];
  if (this->poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
  {
    _xGeo = adfGeoTransform[0] +
            _pixel * adfGeoTransform[1] + _line * adfGeoTransform[2];
    _yGeo = adfGeoTransform[3] +
            _pixel * adfGeoTransform[4] + _line * adfGeoTransform[5];
  }  
}

//////////////////////////////////////////////////
SDTS::~SDTS()
{
}

//////////////////////////////////////////////////
unsigned int SDTS::GetBPP() const
{
  return sizeof(unsigned char) * 3;
}

//////////////////////////////////////////////////
void SDTS::GetData(unsigned char **_data, unsigned int &_count) const
{
  GDALRasterBand  *poBand;
  int nXSize = this->poDataset->GetRasterXSize();
  int nYSize = this->poDataset->GetRasterYSize();
  int nBands = poDataset->GetRasterCount();
  std::vector<float*> data_v;

  if (*_data)
    delete [] *_data;

  // The first band starts with index 1
  for (int i = 1; i <= nBands; ++i)
  {
    // Get a pointer to the current band
    poBand = poDataset->GetRasterBand(i);

    // Read the whole data from the current band
    float *buffer = new float[nXSize * nYSize];
    poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer, nXSize, nYSize,
        GDT_Float32, 0, 0);

    // Store the pointer to this band's data for the future
    data_v.push_back(buffer);
  }

  // Allocate memory for the array that will contain all the data
  _count = this->GetWidth() * this->GetHeight() * this->GetBPP();
  *_data = new unsigned char[_count];

  // Fill the array aligning the data
  unsigned char *p = *_data;
  for (unsigned int i = 0; i < this->GetHeight(); ++i)
  {
    for (unsigned int j = 0; j < this->GetWidth(); ++j)
    {
      if ((i >= nXSize) || (j >= nYSize))
      {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
      }
      else
      {
        int index = i * this->poDataset->GetRasterXSize() + j;
        if (nBands == 1)
        {
          p[0] = static_cast<unsigned char>(data_v[0][index]);
          p[1] = p[0];
          p[2] = p[0];
        }
        else if (nBands == 3)
        {
          p[0] = static_cast<unsigned char>(data_v[0][index]);
          p[1] = static_cast<unsigned char>(data_v[1][index]);
          p[2] = static_cast<unsigned char>(data_v[2][index]);
        }
        else
        {
          gzerr << "Found " << nBands << " bands and only 1 or 3 are supported\n";
        }
      }
      p += 3 * sizeof(unsigned char);
    }
  }

  // Release the temporal array containing the data for each band
  for (unsigned int i = 0; i < data_v.size(); ++i)
  {
    delete[] data_v[i];
  }
}

//////////////////////////////////////////////////
unsigned int SDTS::GetHeight() const
{
  return math::roundUpPowerOfTwo(this->poDataset->GetRasterYSize()) + 1;
}

//////////////////////////////////////////////////
Color SDTS::GetMaxColor()
{
  Color maxClr;
  GDALRasterBand *poBand = NULL;
  std::vector<double> max_v;

  maxClr.Set(0, 0, 0, 0);

  // Read dimension sizes
  int xsize = this->poDataset->GetRasterXSize();
  int ysize = this->poDataset->GetRasterYSize();
  float *buffer = new float[xsize * ysize];
  int nBands = poDataset->GetRasterCount();

  // The first band starts with index 1
  for (int i = 1; i <= nBands; ++i)
  {
    // Get a pointer to the current band
    poBand = poDataset->GetRasterBand(i);

    // Read the whole data from the current band
    poBand->RasterIO(GF_Read, 0, 0, xsize, ysize, buffer, xsize, ysize,
        GDT_Float32, 0, 0);

    // Store the maximum value of this band
    float max = *std::max_element(buffer, buffer + xsize * ysize);
    max_v.push_back(max);
  }

  // Create a normalized color with the max values of each band
  if (max_v.size() == 1)
  {
    double v = max_v[0] / poBand->GetMaximum();
    maxClr.Set(v, v, v);
  }
  else if (max_v.size() == 3)
  {
    maxClr.Set(max_v[0] / 255.0, max_v[1] / 255.0, max_v[2] / 255.0);
  }
  else
    gzerr << "Found " << nBands << " bands and only 1 or 3 are supported\n";

  return maxClr;
}

//////////////////////////////////////////////////
int SDTS::GetPitch() const
{
  return this->GetWidth() * this->GetBPP();
}

//////////////////////////////////////////////////
unsigned int SDTS::GetWidth() const
{
  return math::roundUpPowerOfTwo(this->poDataset->GetRasterXSize()) + 1;
}

//////////////////////////////////////////////////
double SDTS::GetWorldWidth()
{
  return this->worldWidth;
}

//////////////////////////////////////////////////
double SDTS::GetWorldHeight()
{
  return this->worldHeight;
}

//////////////////////////////////////////////////
double SDTS::OgrDistance(double _latAdeg, double _lonAdeg,
                         double _latBdeg, double _lonBdeg)
{
  double latArad, latBrad;
  double cosA, cosB, sinA, sinB, cosP;
  double cosAngle;
 
  cosP = cos(GZ_DTOR(_lonBdeg - _lonAdeg));
  latArad = GZ_DTOR(_latAdeg);
  latBrad = GZ_DTOR(_latBdeg);
  cosA = cos(latArad);
  sinA = sin(latArad);
  cosB = cos(latBrad);
  sinB = sin(latBrad);
  cosAngle = sinA * sinB + cosA * cosB * cosP;

  return this->SafeAcos(cosAngle) * RAD2METER;
}

//////////////////////////////////////////////////
double SDTS::SafeAcos(double _x)
{
  if (_x > 1)
    _x = 1;
  else if (_x < -1)
    _x = -1;
  return acos(_x);
}
