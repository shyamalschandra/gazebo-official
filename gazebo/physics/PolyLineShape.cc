#include <vector>

#include "gazebo/math/Vector3.hh"
#include "gazebo/physics/PolyLineShape.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
PolyLineShape::PolyLineShape(CollisionPtr _parent) : Shape(_parent)
{
  this->AddType(Base::POLYLINE_SHAPE);
}

//////////////////////////////////////////////////
PolyLineShape::~PolyLineShape()
{
}

//////////////////////////////////////////////////
void PolyLineShape::Init()
{
  this->SetHeight(this->sdf->Get<double>("height"));
}

//////////////////////////////////////////////////
void PolyLineShape::SetHeight(const double &_height)
{
  this->sdf->GetElement("height")->Set(_height);
}

//////////////////////////////////////////////////
double PolyLineShape::GetHeight() const
{
  return this->sdf->Get<double>("height");
}
//////////////////////////////////////////////////
void PolyLineShape::SetScale(const math::Vector3 &_scale)
{
  if (_scale.x < 0 || _scale.y < 0 || _scale.z < 0)
    return;

  if (_scale == this->scale)
    return;


  this->scale = _scale;
}

////////////////////////////////////////////////////
void PolyLineShape::SetVertices(const msgs::Geometry &_msg)
{
  int i;
  sdf::ElementPtr pointElem = this->sdf->GetElement("point");
  for( i =0; i<_msg.polyline().point_size(); i++)
  { 
     
    math::Vector2d point( _msg.polyline().point(i).x(),
                          _msg.polyline().point(i).y());
    pointElem->Set(point);
    pointElem = pointElem->GetNextElement("point");
  }
}

//////////////////////////////////////////////////
void PolyLineShape::FillMsg(msgs::Geometry &_msg)
{
  _msg.set_type(msgs::Geometry::POLYLINE);
  _msg.mutable_polyline()->set_height(this->GetHeight());
  sdf::ElementPtr pointElem = this->sdf->GetElement("point");
  while (pointElem)
  { 
     math::Vector2d point = pointElem->Get<math::Vector2d>();
     pointElem = pointElem->GetNextElement("point");
     msgs::Vector2d *ptMsg = _msg.mutable_polyline()->add_point();
     msgs::Set(ptMsg, point);
  }
}

//////////////////////////////////////////////////
void PolyLineShape::ProcessMsg(const msgs::Geometry &_msg)
{
  this->SetHeight(_msg.polyline().height());
  this->SetVertices(_msg);
}


