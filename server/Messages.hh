#ifndef MESSAGES_HH
#define MESSAGES_HH

#include <vector>

#include "Color.hh"
#include "Vector3.hh"
#include "Time.hh"
#include "Pose3d.hh"
#include "Plane.hh"
#include "Box.hh"


namespace gazebo
{
  class XMLConfigNode;

  enum MessageType{ INSERT_MODEL_MSG, VISUAL_MSG, LIGHT_MSG, POSE_MSG, SELECTION_MSG };

  class Message
  {
    public: Message(MessageType t) : type(t), stamp(Time::GetWallTime()) {}
    public: Message(const Message &m) : type(m.type), stamp(m.stamp) {}

    public: virtual Message *Clone() const { return new Message(*this); }

    public: MessageType type;
    public: Time stamp;
  };

  class InsertModelMsg : public Message
  {
    public: InsertModelMsg() : Message(INSERT_MODEL_MSG) {}
    public: InsertModelMsg(const InsertModelMsg &m) : Message(m), 
            xmlStr(m.xmlStr) {}
    public: virtual Message *Clone() const 
            { InsertModelMsg *msg = new InsertModelMsg(*this); return msg; }
    public: std::string xmlStr;
  };

  class LightMsg : public Message
  {
    public: enum ActionType {UPDATE, DELETE};
    public: enum LightType {POINT, SPOT, DIRECTIONAL};
    public: LightMsg();
    public: LightMsg(const LightMsg &m);
    public: virtual Message *Clone() const
            { LightMsg *msg = new LightMsg(*this); return msg; }
    public: void Load(XMLConfigNode *node);

    public: std::string id;
    public: ActionType action;
    public: LightType type;
    public: Pose3d pose;
    public: Color diffuse;
    public: Color specular;
    public: Vector3 attenuation;
    public: Vector3 direction;
    public: float range;
    public: bool castShadows;
    public: float spotInnerAngle;
    public: float spotOuterAngle;
    public: float spotFalloff;
  };

  class VisualMsg : public Message
  {
    public: enum ActionType {UPDATE, DELETE};
    public: enum RenderType {MESH_RESOURCE, POINTS, LINE_LIST, LINE_STRIP, TRIANGLE_FAN};

    public: VisualMsg();
    public: VisualMsg(const VisualMsg &m);
    public: virtual Message *Clone() const 
            { VisualMsg *msg = new VisualMsg(*this); return msg; }

    public: void Load(XMLConfigNode *node);

    public: std::string parentId;
    public: std::string id;
    public: ActionType action;
    public: RenderType render;
    public: std::string mesh;
    public: std::string material;
    public: bool castShadows;
    public: bool attachAxes;
    public: bool visible;
    public: float transparency;
    public: Box boundingbox;
    public: std::vector<Vector3> points;
    public: Pose3d pose;
    public: Plane plane;
    public: Vector3 size;
    public: float uvTile_x;
    public: float uvTile_y;
  };

  class SelectionMsg : public Message
  {
    public: SelectionMsg() : Message(SELECTION_MSG) {}
    public: SelectionMsg(const SelectionMsg &m) : Message(m), id(m.id), selected(m.selected) {}
    public: virtual Message *Clone() const 
            { SelectionMsg *msg = new SelectionMsg(*this); return msg; }

    public: std::string id;
    public: bool selected;
  };

  class PoseMsg : public Message
  {
    public: PoseMsg() : Message(POSE_MSG) {}
    public: PoseMsg(const PoseMsg &m) : Message(m), pose(m.pose), id(m.id) {}
    public: virtual Message *Clone() const 
            { PoseMsg *msg = new PoseMsg(*this); return msg; }

    public: Pose3d pose;
    public: std::string id;
  };
}

#endif
