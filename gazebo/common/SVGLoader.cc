/*
 * Copyright (C) 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <algorithm>
#include <tinyxml.h>

#include <gazebo/common/Console.hh>
#include <gazebo/common/Assert.hh>

#include "SVGLoaderPrivate.hh"
#include "SVGLoader.hh"

using namespace gazebo;
using namespace common;


/*
/////////////////////////////////////////////////
math::Vector2d SVGTransform::Transform(const math::Vector2d &_p)
{
  math::Vector2d r = _p;

  return r;
}
*/

/////////////////////////////////////////////////
std::string lowercase(const std::string &_in)
{
  std::string out = _in;
  std::transform(out.begin(), out.end(), out.begin(), ::tolower);
  return out;
}

/////////////////////////////////////////////////
std::string lowercase(const char *_in)
{
  std::string ins = _in;
  return lowercase(ins);
}

/////////////////////////////////////////////////
std::vector<std::string> &split(const std::string &_s,
                                char _delim,
                                std::vector<std::string> &_elems)
{
  std::stringstream ss(_s);
  std::string item;
  while (std::getline(ss, item, _delim))
  {
    _elems.push_back(item);
  }
  return _elems;
}

/*
/////////////////////////////////////////////////
SVGTransform::SVGTransform(const std::string &_trans)
{
  gzerr << "SVGTransform::SVGTransform " << _trans << std::endl;
  std::vector <math::Matrix3> transforms;

  // splits "rotate(45) translate(0,0)" into
  // "rotate(45" and " translate(0,0"
  std::vector<std::string> transformsStr;
  split(_trans, ')', transformsStr);

  for (size_t i = 0; i < transformsStr.size(); ++i)
  {
    std::string &t = transformsStr[i];
    math::Matrix3 m = this->GetTransformationMatrix(t);
    transforms.push_back(m);
  }
}

/////////////////////////////////////////////////
math::Matrix3 SVGTransform::GetTransformationMatrix(const std::string &_t)
{
  // splits "matrix(0,0,0,0"  into "matrix" and "0,0,0,0"
  std::vector<std::string> tx;
  split(_t, '(', tx);

  std::string &transform = tx[0];
  std::vector<std::string> numbers;
  split(tx[1], ',', numbers);

  // how to unpack the values into 3x3 matrices
  // http://www.w3.org/TR/SVG/coords.html#TransformAttribute
  if (transform.find("matrix") != std::string::npos)
  {
    gzmsg << "matrix" << std::endl;
    double v00 = stod(numbers[0]);
    double v10 = stod(numbers[1]);
    double v01 = stod(numbers[2]);
    double v11 = stod(numbers[3]);
    double v02 = stod(numbers[4]);
    double v12 = stod(numbers[5]);
    math::Matrix3 m(v00, v01, v02, v10, v11, v12, 0, 0, 1);
    return m;
  }

  if (transform.find("skewX") != std::string::npos)
  {
    gzmsg << "skewX" << std::endl;

    math::Matrix3 m;
    return m;
  }

  if (transform.find("skewY") != std::string::npos)
  {
    gzmsg << "skewY" << std::endl;
    math::Matrix3 m;
    return m;
  }

  if (transform.find("scale") != std::string::npos)
  {
    gzmsg << "scale" << std::endl;
    math::Matrix3 m;
    return m;
  }

  if (transform.find("translate") != std::string::npos)
  {
    gzmsg << "translate" << std::endl;
    math::Matrix3 m;
    return m;
  }

  if (transform.find("rotate") != std::string::npos)
  {
    gzmsg << "rotate" << std::endl;
    math::Matrix3 m;
    return m;
  }

  gzwarn << "Unsupported transformation: " << &transform << std::endl;
  math::Matrix3 m;
  return m;
}
*/

/////////////////////////////////////////////////
math::Vector2d bezierInterpolate(double _t,
                                 const math::Vector2d &_p0,
                                 const math::Vector2d &_p1,
                                 const math::Vector2d &_p2,
                                 const math::Vector2d &_p3)
{
  double t_1 = 1.0 - _t;
  double t_1_2 = t_1 * t_1;
  double t_1_3 = t_1_2 * t_1;
  double t2 = _t * _t;
  double t3 = t2 * _t;

  math::Vector2d p;
  p.x = t_1_3 * _p0.x + 3 * _t *  t_1_2 * _p1.x + 3 * t2 * t_1 * _p2.x +
        t3 * _p3.x;
  p.y = t_1_3 * _p0.y + 3 * _t *  t_1_2 * _p1.y + 3 * t2 * t_1 * _p2.y +
        t3 * _p3.y;
  return p;
}

/////////////////////////////////////////////////
void cubicBezier(const math::Vector2d &_p0,
                 const math::Vector2d &_p1,
                 const math::Vector2d &_p2,
                 const math::Vector2d &_p3,
                 double _step,
                 std::vector<math::Vector2d> &_points)
{
  // we don't start at t = 0, but t = step...
  // so we assume that the first point is there (from the last move)
  double t = _step;
  while (t < 1.0)
  {
    auto p = bezierInterpolate(t, _p0, _p1, _p2, _p3);
    _points.push_back(p);
    t += _step;
  }
  // however we close the loop with the last point (t = 1)
  _points.push_back(_p3);
}

static double sqr(float x) { return x*x; }

static float vecang(float _ux, float _uy, float _vx, float _vy)
{
  double ux = _ux;
  double uy = _uy;
  double vx = _vx;
  double vy = _vy;

  double uMag = sqrt(ux * ux + uy * uy);
  double vMag = sqrt(vx * vx + vy * vy);
  double r = (ux * vx + uy * vy) / (uMag * vMag);

  if (r < -1.0)
  {
    r = -1.0;
  }
  else if (r > 1.0)
  {
    r = 1.0;
  }

  double a = acos(r);
  if (ux * vy < uy * vx)
  {
    return -a;
  }
  else
  {
    return a;
  }
}

/////////////////////////////////////////////////
void arcPath(const math::Vector2d &_p0,
             const double _rx,
             const double _ry,
             const double _rotxDeg,
             const size_t _largeArc,
             const size_t _sweepDirection,
             const math::Vector2d &_pEnd,
             const double _step,
             std::vector<math::Vector2d> &_points)
{
  // Ported from canvg (https://code.google.com/p/canvg/)
  double rx = _rx;
  double ry = _ry;
  double rotx = _rotxDeg / 180.0 * M_PI;

  double x1, y1, x2, y2, cx, cy, dx, dy, d;
  double x1p, y1p, cxp, cyp, s, sa, sb;
  double ux, uy, vx, vy, a1, da;
  double x, y, tanx, tany, a, px = 0, py = 0, ptanx = 0, ptany = 0, t[6];
  double sinrx, cosrx;
  double hda, kappa;

  x1 = _p0.x;
  y1 = _p0.y;
  x2 = _pEnd.x;
  y2 = _pEnd.y;

  dx = x1 - x2;
  dy = y1 - y2;
  d = sqrt(dx*dx + dy*dy);
  if (d < 1e-6 || rx < 1e-6 || ry < 1e-6)
  {
    // The arc degenerates to a line
    _points.push_back(_pEnd);
    return;
  }

  sinrx = sin(rotx);
  cosrx = cos(rotx);

  // Convert to center point parameterization.
  // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
  // 1) Compute x1', y1'
  x1p = cosrx * dx / 2.0 + sinrx * dy / 2.0;
  y1p = -sinrx * dx / 2.0 + cosrx * dy / 2.0;
  d = sqr(x1p) / sqr(rx) + sqr(y1p) / sqr(ry);
  if (d > 1)
  {
    d = sqrt(d);
    rx *= d;
    ry *= d;
  }
  // 2) Compute cx', cy'
  s = 0.0;
  sa = sqr(rx) * sqr(ry) - sqr(rx) * sqr(y1p) - sqr(ry) * sqr(x1p);
  sb = sqr(rx) * sqr(y1p) + sqr(ry) * sqr(x1p);
  if (sa < 0.0)
    sa = 0.0;
  if (sb > 0.0)
    s = sqrt(sa / sb);

  if (_largeArc == _sweepDirection)
  {
    s = -s;
  }

  cxp = s * rx * y1p / ry;
  cyp = s * -ry * x1p / rx;

  // 3) Compute cx,cy from cx',cy'
  cx = (x1 + x2) / 2.0 + cosrx * cxp - sinrx * cyp;
  cy = (y1 + y2) / 2.0 + sinrx * cxp + cosrx * cyp;

  // 4) Calculate theta1, and delta theta.
  ux = (x1p - cxp) / rx;
  uy = (y1p - cyp) / ry;
  vx = (-x1p - cxp) / rx;
  vy = (-y1p - cyp) / ry;
  // initial angle
  a1 = vecang(1.0, 0.0, ux, uy);
  // delta angle
  da = vecang(ux, uy, vx, vy);

  if (_largeArc)
  {
    // Choose large arc
    if (da > 0.0)
      da = da - 2 * M_PI;
    else
      da = 2 * M_PI + da;
  }

  // rounding errors for half circles
  if (M_PI - fabs(da) < 0.001)
  {
    if (_sweepDirection)
      da = M_PI;
    else
      da = -M_PI;
  }

  // Approximate the arc using cubic spline segments.
  t[0] = cosrx;
  t[1] = sinrx;
  t[2] = -sinrx;
  t[3] = cosrx;
  t[4] = cx;
  t[5] = cy;

  // Split arc into max 90 degree segments.
  // The loop assumes an iteration per end point
  // (including start and end), this +1.
  size_t ndivs = static_cast<int>(fabs(da) / (M_PI * 0.5) + 1.0);
  hda = (da / ndivs) / 2.0;
  kappa = fabs(4.0 / 3.0 * (1.0 - cos(hda)) / sin(hda));
  if (da < 0.0)
    kappa = -kappa;

  for (size_t i = 0; i <= ndivs; i++)
  {
    a = a1 + da * (1.0 * i /ndivs);
    dx = cos(a);
    dy = sin(a);
    // position  xform point
    double pox = dx * rx;
    double poy = dy * ry;
    x = pox * t[0] + poy * t[2] + t[4];
    y = pox * t[1] + poy * t[3] + t[5];
    // tangent  xform vec
    double tx = -dy * rx * kappa;
    double ty = dx * ry * kappa;
    tanx = tx * t[0] + ty * t[2];
    tany = tx * t[1] + ty * t[3];

    if (i > 0)
    {
      math::Vector2d p0(px, py);
      math::Vector2d p1(px + ptanx, py + ptany);
      math::Vector2d p2(x - tanx, y - tany);
      math::Vector2d p3(x, y);
      cubicBezier(p0, p1, p2, p3, _step, _points);
    }
    px = x;
    py = y;
    ptanx = tanx;
    ptany = tany;
  }
}

/////////////////////////////////////////////////
SvgError::SvgError(const std::string &_what)
  : std::runtime_error(_what)
{
}

/////////////////////////////////////////////////
math::Vector2d SVGLoader::SubpathToPolyline(
                            const std::vector<SVGCommand> &_subpath,
                            math::Vector2d _last,
                            std::vector<math::Vector2d> &_polyline)
{
  GZ_ASSERT(_polyline.size() == 0, "polyline not empty");
  for (SVGCommand cmd: _subpath)
  {
    size_t i = 0;
    size_t count = cmd.numbers.size();

    switch (cmd.cmd)
    {
      case 'm':
      case 'l':
        while (i < count)
        {
          math::Vector2d p;
          p.x = cmd.numbers[i+0];
          p.y = cmd.numbers[i+1];
          // m and l cmds are relative to the last point
          p.x += _last.x;
          p.y += _last.y;
          _polyline.push_back(p);
          _last = p;
          i += 2;
        }
        break;
      case 'M':
      case 'L':
        while (i < count)
        {
          math::Vector2d p;
          p.x = cmd.numbers[i+0];
          p.y = cmd.numbers[i+1];
          _polyline.push_back(p);
          _last = p;
          i += 2;
        }
        break;
      case 'C':
        while (i < count)
        {
          math::Vector2d p0 = _last;
          math::Vector2d p1, p2, p3;
          p1.x = cmd.numbers[i+0];
          p1.y = cmd.numbers[i+1];
          p2.x = cmd.numbers[i+2];
          p2.y = cmd.numbers[i+3];
          p3.x = cmd.numbers[i+4];
          p3.y = cmd.numbers[i+5];
          cubicBezier(p0, p1, p2, p3, this->dataPtr->resolution, _polyline);
          _last = p3;
          i += 6;
        }
        break;
      case 'c':
        while (i < count)
        {
          math::Vector2d p0 = _last;
          math::Vector2d p1, p2, p3;
          p1.x = cmd.numbers[i+0] + _last.x;
          p1.y = cmd.numbers[i+1] + _last.y;
          p2.x = cmd.numbers[i+2] + _last.x;
          p2.y = cmd.numbers[i+3] + _last.y;
          p3.x = cmd.numbers[i+4] + _last.x;
          p3.y = cmd.numbers[i+5] + _last.y;
          cubicBezier(p0, p1, p2, p3, this->dataPtr->resolution, _polyline);
          _last = p3;
          i += 6;
        }
        break;
      case 'A':
        while (i < count)
        {
          math::Vector2d p0 = _last;
          double rx = cmd.numbers[i+0];
          double ry = cmd.numbers[i+1];
          double xRot = cmd.numbers[i+2];
          unsigned int arc(cmd.numbers[i+3]);
          unsigned int sweep(cmd.numbers[i+4]);
          math::Vector2d pEnd;
          pEnd.x = cmd.numbers[i+5];
          pEnd.y = cmd.numbers[i+6];
          arcPath(p0, rx, ry, xRot, arc, sweep, pEnd,
                  this->dataPtr->resolution, _polyline);
          _last = pEnd;
          i += 7;
        }
        break;
      case 'a':
        while (i < count)
        {
          math::Vector2d p0 = _last;
          double rx = cmd.numbers[i+0];
          double ry = cmd.numbers[i+1];
          double xRot = cmd.numbers[i+2];
          unsigned int arc(cmd.numbers[i+3]);
          unsigned int sweep(cmd.numbers[i+4]);
          math::Vector2d pEnd;
          pEnd.x = cmd.numbers[i+5] + _last.x;
          pEnd.y = cmd.numbers[i+6] + _last.y;
          arcPath(p0, rx, ry, xRot, arc, sweep, pEnd,
                  this->dataPtr->resolution, _polyline);
          _last = pEnd;
          i += 7;
        }
      // Z and z indicate closed path.
      // just add the first point to the list
      case 'Z':
      case 'z':
        {
          math::Vector2d pEnd;
          pEnd = _polyline[0];
          _polyline.push_back(pEnd);
          break;
        }
      default:
        gzerr << "Unexpected SVGCommand value: " << cmd.cmd << std::endl;
    }
  }
  return _last;
}

/////////////////////////////////////////////////
SVGLoader::SVGLoader(unsigned int _samples)
{
  this->dataPtr = new SVGLoaderPrivate();
  if (_samples == 0)
  {
    std::string m("The number of samples cannot be 0");
    SvgError e(m);
    throw e;
  }
  this->dataPtr->resolution = 1.0/_samples;
}

/////////////////////////////////////////////////
SVGLoader::~SVGLoader()
{
  delete(this->dataPtr);
}

/////////////////////////////////////////////////
void SVGLoader::SplitSubpaths(const std::vector<SVGCommand> &_cmds,
                              std::vector< std::vector<SVGCommand> > &_subpaths)
{
  if (_cmds.empty())
  {
    std::ostringstream os;
    os << "SVGPath has no commands";
    SvgError x(os.str());
    throw x;
  }

  for (SVGCommand cmd: _cmds)
  {
    if (tolower(cmd.cmd) == 'm')
    {
      // the path contains a subpath
      std::vector<SVGCommand> sub;
      _subpaths.push_back(sub);
    }
    // get a reference to the latest subpath
    std::vector<SVGCommand> &subpath = _subpaths.back();
    // give the cmd to the latest
    subpath.push_back(cmd);
  }
}

/////////////////////////////////////////////////
void SVGLoader::ExpandCommands(
                  const std::vector< std::vector<SVGCommand> > &_subpaths,
                  SVGPath &_path)
{
  for (std::vector<SVGCommand> compressedSubpath :_subpaths)
  {
    // add new subpath
    _path.subpaths.push_back(std::vector<SVGCommand>());
    // get a reference
    std::vector<SVGCommand> &subpath = _path.subpaths.back();
    // copy the cmds with repeating commands, grouping the numbers
    for (SVGCommand xCmd : compressedSubpath)
    {
      unsigned int numberCount = 0;
      if (tolower(xCmd.cmd) == 'a')
        numberCount = 7;
      if (tolower(xCmd.cmd) == 'c')
        numberCount = 6;
      if (tolower(xCmd.cmd) == 'm')
        numberCount = 2;
      if (tolower(xCmd.cmd) == 'l')
        numberCount = 2;
      if (tolower(xCmd.cmd) == 'v')
        numberCount = 1;
      if (tolower(xCmd.cmd) == 'h')
        numberCount = 1;
      if (tolower(xCmd.cmd) == 'z')
        subpath.push_back(xCmd);
      // group numbers together and repeat the command
      // for each group
      unsigned int n = 0;
      size_t size = xCmd.numbers.size();
      while (n < size)
      {
        subpath.push_back(SVGCommand());
        SVGCommand &cmd = subpath.back();
        cmd.cmd = xCmd.cmd;
        for (size_t i = 0; i < numberCount; ++i)
        {
          cmd.numbers.push_back(xCmd.numbers[i+n]);
        }
        n += numberCount;
      }
    }
  }
}

/////////////////////////////////////////////////
void SVGLoader::GetPathCommands(const std::vector<std::string> &_tokens,
                                  SVGPath &_path)
{
  std::vector <SVGCommand> cmds;
  std::string lookup = "aAcCmMqQlLvVhHzZ";
  char lastCmd = 'x';
  std::vector<double> numbers;

  for (std::string token: _tokens)
  {
    // new command?
    if (lookup.find(token[0]) == std::string::npos)
    {
      // its just numbers
      std::vector<std::string> numberStrs;
      split(token, ',', numberStrs);
      for (std::string numberStr : numberStrs)
      {
        double f = atof(numberStr.c_str());
        numbers.push_back(f);
       }
     }
     else
     {
      if (lastCmd != 'x')
      {
        SVGCommand c;
        c.cmd = lastCmd;
        c.numbers = numbers;
        cmds.push_back(c);
       }
       // its new command
       lastCmd = token[0];
       numbers.resize(0);
     }
  }
  // the last command
  if (lastCmd != 'x')
  {
    SVGCommand c;
    c.cmd = lastCmd;
    c.numbers = numbers;
    cmds.push_back(c);
  }
  // split the commands into sub_paths
  std::vector< std::vector< SVGCommand> > subpaths;
  this->SplitSubpaths(cmds, subpaths);
  this->ExpandCommands(subpaths, _path);
  // the starting point for the subpath
  // it is the end point of the previous one
  math::Vector2d p;
  p.x = 0;
  p.y = 0;
  for (std::vector<SVGCommand> subpath : subpaths)
  {
    _path.polylines.push_back(std::vector<math::Vector2d>());
    std::vector<math::Vector2d> &polyline = _path.polylines.back();
    p = this->SubpathToPolyline(subpath, p, polyline);
  }
}

/////////////////////////////////////////////////
void SVGLoader::GetPathAttribs(TiXmlElement *_pElement, SVGPath &_path)
{
  GZ_ASSERT(_pElement, "empty XML element where a path was expected");
  TiXmlAttribute *pAttrib = _pElement->FirstAttribute();
  while (pAttrib)
  {
    std::string name = lowercase(pAttrib->Name());
    std::string value = pAttrib->Value();
    if (name == "style")
    {
      _path.style = value;
    }
    else if (name == "id")
    {
      _path.id = value;
    }
    else if (name == "transform")
    {
      _path.transform = value;
      gzwarn << "transform attribute \"" << name
        << "\" not implemented yet"  << std::endl;
    }
    else if (name == "d")
    {
      // this attribute contains a list of coordinates
      std::vector<std::string> tokens;
      split(value, ' ', tokens);
      this->GetPathCommands(tokens, _path);
    }
    else
    {
      gzwarn << "Ignoring attribute \"" << name  << "\" in path"  << std::endl;
    }
    pAttrib = pAttrib->Next();
  }
}

/////////////////////////////////////////////////
void SVGLoader::GetSvgPaths(TiXmlNode *_pParent, std::vector<SVGPath> &_paths)
{
  if (!_pParent)
    return;

  TiXmlNode *pChild;
  int t = _pParent->Type();
  std::string name;
  if ( t == TiXmlNode::TINYXML_ELEMENT)
  {
    name = lowercase(_pParent->Value());
    if (name == "path")
    {
      TiXmlElement *element = _pParent->ToElement();
      SVGPath p;
      this->GetPathAttribs(element, p);
      _paths.push_back(p);
    }
    // skip defs node that can contain path
    // elements that are not actual paths.
    if (name == "defs")
    {
      return;
    }
  }

  for (pChild = _pParent->FirstChild();
       pChild != 0;
       pChild = pChild->NextSibling())
  {
    this->GetSvgPaths(pChild, _paths);
  }
}

/////////////////////////////////////////////////
bool SVGLoader::Parse(const std::string &_filename,
    std::vector<SVGPath> &_paths)
{
  try
  {
    // load the named file and dump its structure to STDOUT
    TiXmlDocument doc(_filename.c_str());
    bool loadOkay = doc.LoadFile();
    if (!loadOkay)
    {
      std::ostringstream os;
      gzerr << "Failed to load file " <<  _filename << std::endl;
      gzerr << os.str() << std::endl;
    }

    this->GetSvgPaths(&doc, _paths);
    return true;
  }
  catch(SvgError &e)
  {
    gzerr << e.what() << std::endl;
  }
  return false;
}

/////////////////////////////////////////////////
void SVGLoader::DumpPaths(const std::vector<SVGPath> &_paths,
                          std::ostream &_out) const
{
  // this prints an html document that allows to debug
  // SVG parsing issues. The points are generated in
  // a loop between the header and footer.
  std::string header = R"***(
<!DOCTYPE html>
<html>

<script type="text/javascript">

)***";
  std::string footer = R"***(
</script>

<script>

var x0 = 0;
var y0 = 0;
var scale = 1.;

function xx(x)
{
  var r = x0 + scale * x;
  return r;
}

function yy(y)
{
  var r =  - (y0 + scale * (-y) );
  return r;
}

function drawPoint(ctx, x, y)
{
  ctx.beginPath();
  ctx.arc(x, y, 5, 0, 2 * Math.PI, true);
  ctx.strokeStyle= style;
  ctx.stroke();
}

function drawPath(ctx, path, style, x0, y0, scale, showCtrlPoints )
{
  console.log('drawPath ' + path.name);

  ctx.beginPath();
  for (var j = 0; j <  path.subpaths.length; j++)
  {
    var points = path.subpaths[j];
    console.log(points.length + ' points in subpath, (' + style + ')');
    if (points.length < 2)
    {
      console.log("not enough points in subpath " + j);
      return;
    }
    ctx.moveTo(xx(points[0][0]), yy(points[0][1]));
    for (var i = 1; i < points.length; ++i)
    {
      var x= xx(points[i][0]);
      var y= yy(points[i][1]);
      ctx.lineTo(x, y);
    }
    ctx.strokeStyle= style;
    ctx.stroke();

    // draw points
    if (showCtrlPoints)
    {
      var styles = ["black", "orange", "grey"];
      for (var i = 0; i < points.length; ++i)
      {
        var x= xx(points[i][0]);
        var y= yy(points[i][1]);
        var m = " [" + points[i][0] + ", " + points[i][1];
        m += "]  [" + x + ", " + y + "]";
        console.log(m);
        ctx.beginPath();
        if (i == 0)
        {
          ctx.arc(x, y, 4, 0, 2 * Math.PI, true);
          ctx.strokeStyle = "red";
          ctx.fill();
        }
        else if (i == 1)
        {
          ctx.arc(x, y, 2, 0, 2 * Math.PI, true);
          ctx.strokeStyle= "red";
        }
        else
        {
          ctx.arc(x, y, 2, 0, 2 * Math.PI, true);
          ctx.strokeStyle= styles[i % styles.length ];
        }
        ctx.stroke();
       }
    }
  }
}


function draw(showCtrlPoints)
{
  var canvas = document.getElementById("myCanvas");
  var ctx = canvas.getContext("2d");
  var styles = ["red", "green", "blue"];

  ctx.clearRect(0, 0, canvas.width, canvas.height);
  x0 = Number(document.getElementsByName("xoff_in")[0].value);
  y0 = Number(document.getElementsByName("yoff_in")[0].value);
  scale = Number(document.getElementsByName("scale_in")[0].value);

  for (var i =0; i < svg.length; ++i)
  {
    var path = svg[i];
    console.log("path: " + path.name);
    drawPath(ctx, path, styles[i%3], x0, y0, scale, showCtrlPoints);
  }
}

  console.log("number of paths: " + svg.length);

  document.addEventListener("DOMContentLoaded", function(event)
  {
    draw();
  });

</script>


<body>

  <div>

  Xoff: <input type="text" name="xoff_in" value="0"><br>
  Yoff: <input type="text" name="yoff_in" value="0"><br>
  Scale: <input type="text" name="scale_in" value="1.0"><br>

  <button onclick="draw(true);">Draw</button>
  </div>

  <canvas
    id="myCanvas"
    width="1024"
    height="768"
    style="border:1px solid #d3d3d3;">
    Your browser does not support the canvas element.
  </canvas>

</body>
</html>

)***";

  _out << header << std::endl;
  _out << "var svg = [];" << std::endl;
  for (SVGPath path : _paths)
  {
    _out << "svg.push({name:\"" << path.id;
    _out <<  "\", subpaths:[], style: \"";
    _out << path.style << "\"}); " << std::endl;
    _out << "svg[svg.length-1].subpaths = [";
    char psep = ' ';

    for (unsigned int i = 0; i < path.polylines.size(); ++i)
    {
      std::vector<math::Vector2d> poly = path.polylines[i];
      _out << psep <<  "[" << std::endl;
      psep = ',';
      char sep = ' ';
      for ( math::Vector2d p : poly)
      {
        _out << " " << sep << " [" <<  p.x << ", " << p.y << "]" <<std::endl;
        sep = ',';
      }
      _out << " ] " << std::endl;
    }
    _out << "];" << std::endl;
    _out << "\n\n";
  }
  _out << footer << std::endl;
}

