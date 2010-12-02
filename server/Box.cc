#include "Box.hh"

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
Box::Box (const Vector3 min, const Vector3 max)
  : min(min), max(max)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
Box::~Box()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Get the length along the x dimension
double Box::GetXLength()
{
  return fabs(max.x - min.x);
}

////////////////////////////////////////////////////////////////////////////////
/// Get the length along the y dimension
double Box::GetYLength()
{
  return fabs(max.y - min.y);
}

////////////////////////////////////////////////////////////////////////////////
/// Get the length along the z dimension
double Box::GetZLength()
{
  return fabs(max.z - min.z);
}
