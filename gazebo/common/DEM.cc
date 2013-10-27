/*
 * Copyright 2012-2013 Open Source Robotics Foundation
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

#include "gazebo/common/Assert.hh"
#include "gazebo/common/DEM.hh"
#include "gazebo/common/Exception.hh"
#include "gazebo/math/Angle.hh"

using namespace gazebo;
using namespace common;

#ifdef HAVE_GDAL

//////////////////////////////////////////////////
DEM::DEM()
{
  GDALAllRegister();
}

//////////////////////////////////////////////////
DEM::~DEM()
{
  GDALClose(reinterpret_cast<GDALDataset *>(this->dataSet));
}

//////////////////////////////////////////////////
void DEM::Load(const std::string &_filename)
{
  this->dataSet = reinterpret_cast<GDALDataset *>(GDALOpen(
    _filename.c_str(), GA_ReadOnly));

  // Sanity check
  if (this->dataSet == NULL)
    gzthrow("Unable to find DEM file [" + _filename + "]\n");

  int nBands = this->dataSet->GetRasterCount();
  if (nBands != 1)
    gzthrow("Unsupported number of bands in file [" << _filename + "]. Found "
       << nBands << " but only 1 is a valid value\n");

  // Set the pointer to the band
  this->band = this->dataSet->GetRasterBand(1);

  int xSize = this->dataSet->GetRasterXSize();
  int ySize = this->dataSet->GetRasterYSize();
  double upLeftX = 0.0;
  double upLeftY = 0.0;
  double upRightX = xSize;
  double upRightY = 0.0;
  double lowLeftX = 0.0;
  double lowLeftY = ySize;
  double gUpLeftX, gUpLeftY;
  double gUpRightX, gUpRightY;
  double gLowLeftX, gLowLeftY;

  // Calculate the georeferenced coordinates of the terrain corners
  this->GetGeoReference(upLeftX, upLeftY, gUpLeftX, gUpLeftY);
  this->GetGeoReference(upRightX, upRightY, gUpRightX, gUpRightY);
  this->GetGeoReference(lowLeftX, lowLeftY, gLowLeftX, gLowLeftY);

  // Set the world width and height
  this->worldWidth = this->Distance(gUpLeftY, gUpLeftX, gUpRightY, gUpRightX);
  this->worldHeight = this->Distance(gUpLeftY, gUpLeftX, gLowLeftY, gLowLeftX);

  // Set the min/max heights
  float *buffer = new float[xSize * ySize];
  this->band->RasterIO(GF_Read, 0, 0, xSize, ySize, buffer, xSize, ySize,
      GDT_Float32, 0, 0);
  this->minElevation = *std::min_element(buffer, buffer + xSize * ySize);
  this->maxElevation = *std::max_element(buffer, buffer + xSize * ySize);
  if (buffer)
    delete[] buffer;

  // Set the width/height (after the padding)
  if (math::isPowerOfTwo(ySize - 1))
    this->height = ySize;
  else
    this->height = math::roundUpPowerOfTwo(ySize) + 1;

  if (math::isPowerOfTwo(xSize - 1))
    this->width = xSize;
  else
    this->width = math::roundUpPowerOfTwo(xSize) + 1;

  GZ_ASSERT(this->width == this->height, "DEM after padding must be square");
}

//////////////////////////////////////////////////
void DEM::GetData(float **_data, unsigned int &_count) const
{
  unsigned int nXSize = this->dataSet->GetRasterXSize();
  unsigned int nYSize = this->dataSet->GetRasterYSize();

  if (*_data)
    delete [] *_data;

  // Allocate memory for the array that will contain all the data
  _count = this->GetWidth() * this->GetHeight();
  *_data = new float[_count];

  // Read the whole raster data and convert it to a GDT_Float32 array
  float *buffer = new float[nXSize * nYSize];
  this->band->RasterIO(GF_Read, 0, 0, nXSize, nYSize, buffer, nXSize, nYSize,
      GDT_Float32, 0, 0);

  // Fill the array aligning the data
  float *p = *_data;
  for (unsigned int y = 0; y < this->GetHeight(); ++y)
  {
    for (unsigned int x = 0; x < this->GetWidth(); ++x)
    {
      // Add the padding
      if ((y >= nYSize) || (x >= nXSize))
      {
        p[0] = 0;
      }
      else
      {
        p[0] = buffer[y * nXSize + x];
      }
      p++;
    }
  }

  // Release the temporary buffer
  if (buffer)
    delete[] buffer;
}

//////////////////////////////////////////////////
unsigned int DEM::GetHeight() const
{
  return this->height;
}

//////////////////////////////////////////////////
unsigned int DEM::GetWidth() const
{
  return this->width;
}

//////////////////////////////////////////////////
float DEM::GetMinElevation() const
{
  return minElevation;
}

//////////////////////////////////////////////////
float DEM::GetMaxElevation() const
{
  return maxElevation;
}

//////////////////////////////////////////////////
double DEM::GetWorldWidth() const
{
  return this->worldWidth;
}

//////////////////////////////////////////////////
double DEM::GetWorldHeight() const
{
  return this->worldHeight;
}

//////////////////////////////////////////////////
void DEM::GetGeoReference(double _x, double _y, double &_xGeo, double &_yGeo)
{
  double geoTransf[6];
  if (this->dataSet->GetGeoTransform(geoTransf) == CE_None)
  {
    _xGeo = geoTransf[0] + _x * geoTransf[1] + _y * geoTransf[2];
    _yGeo = geoTransf[3] + _x * geoTransf[4] + _y * geoTransf[5];
  }
  else
    gzthrow("Unable to obtain the georeferenced values for line [" << _y <<
            "] and pixel [" << _x << "]\n");
}

//////////////////////////////////////////////////
/// Based on OGRXPlane_Distance() (ogr_xplange_geo_utils.cpp, gdal).
double DEM::Distance(double _latAdeg, double _lonAdeg,
                     double _latBdeg, double _lonBdeg)
{
  double Rad2Meter = (180. / M_PI) * 60. * 1852.;
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

  // Crop
  if (cosAngle > 1)
    cosAngle = 1;
  else if (cosAngle < - 1)
    cosAngle = -1;

  return acos(cosAngle) * Rad2Meter;
}

//////////////////////////////////////////////////
void DEM::FillHeightMap(int _subSampling, unsigned int _vertSize,
    const math::Vector3 &_size, const math::Vector3 &_scale, bool _flipY,
    std::vector<float> &_heights)
{
  unsigned int x, y;
  float h = 0;
  float h1 = 0;
  float h2 = 0;

  // Resize the vector to match the size of the vertices.
  _heights.resize(_vertSize * _vertSize);

  float *data = NULL;
  unsigned int count;
  this->GetData(&data, count);

  double yf, xf, dy, dx;
  int y1, y2, x1, x2;
  double px1, px2, px3, px4;

  // Iterate over all the vertices
  for (y = 0; y < _vertSize; ++y)
  {
    yf = y / static_cast<double>(_subSampling);
    y1 = floor(yf);
    y2 = ceil(yf);
    if (y2 >= this->height)
      y2 = this->height - 1;
    dy = yf - y1;

    for (x = 0; x < _vertSize; ++x)
    {
      xf = x / static_cast<double>(_subSampling);
      x1 = floor(xf);
      x2 = ceil(xf);
      if (x2 >= this->width)
        x2 = this->width - 1;
      dx = xf - x1;

      px1 = data[y1 * this->width + x1];
      px2 = data[y1 * this->width + x2];
      h1 = (px1 - ((px1 - px2) * dx));

      px3 = data[y2 * this->width + x1];
      px4 = data[y2 * this->width + x2];
      h2 = (px3 - ((px3 - px4) * dx));

      h = (h1 - ((h1 - h2) * dy) - this->GetMinElevation()) * _scale.z;

      // invert pixel definition so 1=ground, 0=full height,
      //   if the terrain size has a negative z component
      //   this is mainly for backward compatibility
      if (_size.z < 0)
        h *= -1;

      // Store the height for future use
      if (!_flipY)
        _heights[y * _vertSize + x] = h;
      else
        _heights[(_vertSize - y - 1) * _vertSize + x] = h;
    }
  }

  if (data)
    delete [] data;
}
#endif
