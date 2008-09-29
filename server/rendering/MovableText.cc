/**
 * File: MovableText.cpp
 *
 * description: This create create a billboarding object that display a text.
 *
 * @author  2003 by cTh see gavocanov@rambler.ru
 * @update  2006 by barraq see nospam@barraquand.com
 * @update  2007 by independentCreations see independentCreations@gmail.com
 */

#include "MovableText.hh"

#include <OgreFontManager.h>
#include <OgrePrerequisites.h>

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

using namespace gazebo;

////////////////////////////////////////////////////////////////////////////////
// Constructor
MovableText::MovableText()
    : camera(NULL),
    renderWindow(NULL) ,
    viewportAspectCoef (0.75),
    font(NULL) ,
    spaceWidth(0) ,
    updateColors(true) ,
    vertAlign(V_BELOW) ,
    horizAlign(H_LEFT) ,
    onTop(false) ,
    baseline(0.0) 
{
  this->renderOp.vertexData = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
MovableText::~MovableText()
{
  if (this->renderOp.vertexData)
    delete this->renderOp.vertexData;
}

////////////////////////////////////////////////////////////////////////////////
//Loads the text to display and select the font
void MovableText::Load(const std::string &name,
                       const Ogre::UTFString &text,
                       const std::string &fontName,
                       float charHeight,
                       const Ogre::ColourValue &color)
{
  this->text=text;
  this->color=color;
  this->fontName=fontName;
  this->charHeight=charHeight;
  this->mName = name;

  if (this->mName == "")
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS,
                          "Trying to create MovableText without name",
                          "MovableText::MovableText");

  if (this->text == "")
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS,
                          "Trying to create MovableText without text",
                          "MovableText::MovableText");


  this->SetFontName(this->fontName);
  this->_setupGeometry();

}


////////////////////////////////////////////////////////////////////////////////
// Set the font name
void MovableText::SetFontName(const std::string &newFontName)
{
  if ((Ogre::MaterialManager::getSingletonPtr()->resourceExists(this->mName + "Material")))
  {
    Ogre::MaterialManager::getSingleton().remove(this->mName + "Material");
  }

  if (this->fontName != newFontName || this->material.isNull() || !this->font)
  {
    this->fontName = newFontName;

    this->font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(this->fontName).getPointer();

    if (!this->font)
    {
      throw Ogre::Exception(Ogre::Exception::ERR_ITEM_NOT_FOUND,
                            "Could not find font " + fontName,
                            "MovableText::setFontName");
    }

    this->font->load();

    if (!this->material.isNull())
    {
      Ogre::MaterialManager::getSingletonPtr()->remove(this->material->getName());
      this->material.setNull();
    }

    this->material = this->font->getMaterial()->clone(this->mName + "Material");

    if (!this->material->isLoaded())
      this->material->load();

    this->material->setDepthCheckEnabled(!this->onTop);
    this->material->setDepthBias(!this->onTop, 0);
    this->material->setDepthWriteEnabled(this->onTop);
    this->material->setLightingEnabled(false);

    this->needUpdate = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set the caption
void MovableText::SetText(const Ogre::UTFString &newText)
{
  if (this->text != newText)
  {
    this->text = newText;
    this->needUpdate = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set the color
void MovableText::SetColor(const Ogre::ColourValue &newColor)
{
  if (this->color != newColor)
  {
    this->color = newColor;
    this->updateColors = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set the character height
void MovableText::SetCharHeight(float height)
{
  if (this->charHeight != height)
  {
    this->charHeight = height;
    this->needUpdate = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set the width of the space between characters
void MovableText::SetSpaceWidth(float width)
{
  if (this->spaceWidth != width)
  {
    this->spaceWidth = width;
    this->needUpdate = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set alignment of the text
void MovableText::SetTextAlignment(const HorizAlign &h, const VertAlign &v)
{
  if (this->horizAlign != h)
  {
    this->horizAlign = h;
    this->needUpdate = true;
  }

  if (this->vertAlign != v)
  {
    this->vertAlign = v;
    this->needUpdate = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set additional height
void MovableText::SetBaseline( float base )
{
  if ( this->baseline != base )
  {
    this->baseline = base;
    this->needUpdate = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set whether the text should be shown on top
void MovableText::SetShowOnTop(bool show)
{
  if (this->onTop != show && !this->material.isNull())
  {
    this->onTop = show;

    this->material->setDepthBias(!this->onTop, 0);
    this->material->setDepthCheckEnabled(!this->onTop);
    this->material->setDepthWriteEnabled(this->onTop);
  }
}

////////////////////////////////////////////////////////////////////////////////
// True=text is displayed on top
bool MovableText::GetShowOnTop() const
{
  return this->onTop;
}

////////////////////////////////////////////////////////////////////////////////
// Get the axis aligned bounding box
Ogre::AxisAlignedBox MovableText::GetAABB(void)
{
  return this->aabb;
}

////////////////////////////////////////////////////////////////////////////////
// Setup the billboard that renders the text
void MovableText::_setupGeometry()
{
  assert(this->font);
  assert(!this->material.isNull());

  Ogre::VertexDeclaration *decl = NULL;
  Ogre::VertexBufferBinding *bind = NULL;
  Ogre::HardwareVertexBufferSharedPtr ptbuf;
  Ogre::HardwareVertexBufferSharedPtr cbuf;
  float *pVert = NULL;
  float largestWidth = 0;
  float left = 0;
  float top = 0;
  size_t offset = 0;
  float maxSquaredRadius = 0.0f;
  bool first = true;
  Ogre::UTFString::iterator i;
  bool newLine = true;
  float len = 0.0f;

  // for calculation of AABB
  Ogre::Vector3 min, max, currPos;

  unsigned int vertexCount = static_cast<unsigned int>(this->text.size() * 6);


  if (this->renderOp.vertexData)
  {
    // Removed this test as it causes problems when replacing a caption
    // of the same size: replacing "Hello" with "hello"
    // as well as when changing the text alignment
    //if (this->renderOp.vertexData->vertexCount != vertexCount)
    {
      delete this->renderOp.vertexData;
      this->renderOp.vertexData = NULL;
      this->updateColors = true;
    }
  }

  if (!this->renderOp.vertexData)
    this->renderOp.vertexData = new Ogre::VertexData();

  this->renderOp.indexData = 0;
  this->renderOp.vertexData->vertexStart = 0;
  this->renderOp.vertexData->vertexCount = vertexCount;
  this->renderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
  this->renderOp.useIndexes = false;

  decl = this->renderOp.vertexData->vertexDeclaration;
  bind = this->renderOp.vertexData->vertexBufferBinding;

  // create/bind positions/tex.ccord. buffer
  if (!decl->findElementBySemantic(Ogre::VES_POSITION))
    decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT3,
                     Ogre::VES_POSITION);

  offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

  if (!decl->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES))
    decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2,
                     Ogre::VES_TEXTURE_COORDINATES, 0);

  ptbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            decl->getVertexSize(POS_TEX_BINDING),
            this->renderOp.vertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

  bind->setBinding(POS_TEX_BINDING, ptbuf);

  // Colours - store these in a separate buffer because they change less often
  if (!decl->findElementBySemantic(Ogre::VES_DIFFUSE))
    decl->addElement(COLOUR_BINDING, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

  cbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
           decl->getVertexSize(COLOUR_BINDING),
           this->renderOp.vertexData->vertexCount,
           Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

  bind->setBinding(COLOUR_BINDING, cbuf);

  pVert = static_cast<float*>(ptbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

  // Derive space width from a capital A
  if (this->spaceWidth == 0)
    this->spaceWidth = this->font->getGlyphAspectRatio('A') * this->charHeight * 2.0;

  if (this->vertAlign == MovableText::V_ABOVE)
  {
    // Raise the first line of the caption
    top += this->charHeight;

    for (i = this->text.begin(); i != this->text.end(); ++i)
    {
      if (*i == '\n')
        top += this->charHeight * 2.0;
    }
  }

  for ( i = this->text.begin(); i != this->text.end(); ++i )
  {
    if ( newLine )
    {
      len = 0.0;
      for ( Ogre::UTFString::iterator j = i; j != this->text.end(); j++ )
      {
        Ogre::Font::CodePoint character = j.getCharacter();
        if (character == 0x000D // CR
            || character == 0x0085) // NEL
        {
          break;
        }
        else if (character == 0x0020) // space
        {
          len += this->spaceWidth;
        }
        else
        {
          len += this->font->getGlyphAspectRatio(character) * this->charHeight * 2.0 * this->viewportAspectCoef;
        }
      }

      newLine = false;
    }

    Ogre::Font::CodePoint character = i.getCharacter();

    if (character == 0x000D // CR
        || character == 0x0085) // NEL
    {
      top -= this->charHeight * 2.0;
      newLine = true;

      // Also reduce tri count
      this->renderOp.vertexData->vertexCount -= 6;
      continue;
    }
    else if (character == 0x0020) // space
    {
      // Just leave a gap, no tris
      left += this->spaceWidth;

      // Also reduce tri count
      this->renderOp.vertexData->vertexCount -= 6;
      continue;
    }

    float horiz_height = this->font->getGlyphAspectRatio(character) * this->viewportAspectCoef ;

    const Ogre::Font::UVRect& uvRect = this->font->getGlyphTexCoords(character);

    // each vert is (x, y, z, u, v)
    //------------------------------------------------------------------------
    // First tri
    //
    // Upper left
    if (this->horizAlign == MovableText::H_LEFT)
      *pVert++ = left;
    else
      *pVert++ = left - (len/2.0);

    *pVert++ = top;
    *pVert++ = 0;
    *pVert++ = uvRect.left;
    *pVert++ = uvRect.top;

    // Deal with bounds
    if (this->horizAlign == MovableText::H_LEFT)
      currPos = Ogre::Vector3(left,top,0);
    else
      currPos = Ogre::Vector3(left - (len/2.0), top, 0);

    if (first)
    {
      min = max = currPos;
      maxSquaredRadius = currPos.squaredLength();
      first = false;
    }
    else
    {
      min.makeFloor(currPos);
      max.makeCeil(currPos);
      maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
    }

    top -= this->charHeight * 2.0;

    // Bottom left
    if (this->horizAlign == MovableText::H_LEFT)
      *pVert++ = left;
    else
      *pVert++ = left - (len / 2.0);

    *pVert++ = top;
    *pVert++ = 0;
    *pVert++ = uvRect.left;
    *pVert++ = uvRect.bottom;

    // Deal with bounds
    if (this->horizAlign == MovableText::H_LEFT)
      currPos = Ogre::Vector3(left,top,0);
    else
      currPos = Ogre::Vector3(left - (len/2), top, 0);

    min.makeFloor(currPos);
    max.makeCeil(currPos);
    maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());


    top += this->charHeight * 2.0;
    left += horiz_height * this->charHeight * 2.0;

    // Top right
    if (this->horizAlign == MovableText::H_LEFT)
      *pVert++ = left;
    else
      *pVert++ = left - (len/2.0);

    *pVert++ = top;
    *pVert++ = 0;
    *pVert++ = uvRect.right;
    *pVert++ = uvRect.top;

    // Deal with bounds
    if (this->horizAlign == MovableText::H_LEFT)
      currPos = Ogre::Vector3(left,top,0);
    else
      currPos = Ogre::Vector3(left - (len/2), top, 0);

    min.makeFloor(currPos);
    max.makeFloor(currPos);
    maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());


    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Second tri
    //
    // Top right (again)
    if (this->horizAlign == MovableText::H_LEFT)
      *pVert++ = left;
    else
      *pVert++ = left - (len/2.0);
    *pVert++ = top;
    *pVert++ = 0;
    *pVert++ = uvRect.right;
    *pVert++ = uvRect.top;

    // Deal with bounds
    currPos = Ogre::Vector3(left,top,0);
    min.makeFloor(currPos);
    max.makeFloor(currPos);
    maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());


    top -= this->charHeight * 2.0;
    left -= horiz_height  * this->charHeight * 2.0;

    // Bottom left (again)
    if (this->horizAlign == MovableText::H_LEFT)
      *pVert++ = left;
    else
      *pVert++ = left - (len/2.0);
    *pVert++ = top;
    *pVert++ = 0;
    *pVert++ = uvRect.left;
    *pVert++ = uvRect.bottom;

    // Deal with bounds
    currPos = Ogre::Vector3(left,top,0);
    min.makeFloor(currPos);
    max.makeFloor(currPos);
    maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());


    left += horiz_height  * this->charHeight * 2.0;

    // Bottom right
    if (this->horizAlign == MovableText::H_LEFT)
      *pVert++ = left;
    else
      *pVert++ = left - (len/2.0);
    *pVert++ = top;
    *pVert++ = 0;
    *pVert++ = uvRect.right;
    *pVert++ = uvRect.bottom;

    // Deal with bounds
    currPos = Ogre::Vector3(left,top,0);
    min.makeFloor(currPos);
    max.makeFloor(currPos);
    maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

    //-------------------------------------------------------------------------


    // Go back up with top
    top += this->charHeight * 2.0;

    float currentWidth = (left + 1.0)/2.0;
    if (currentWidth > largestWidth)
    {
      largestWidth = currentWidth;
    }
  }

  // Unlock vertex buffer
  ptbuf->unlock();


  /*  min.x=min.y=min.z=-10000;
    max.x=max.y=max.z = 10000;
    */

  // update AABB/Sphere radius
  this->aabb = Ogre::AxisAlignedBox(min, max);
  this->radius = Ogre::Math::Sqrt(maxSquaredRadius);

  if (this->updateColors)
    this->_updateColors();

  this->needUpdate = false;
}

////////////////////////////////////////////////////////////////////////////////
// Update the colors
void MovableText::_updateColors(void)
{
  Ogre::RGBA color;
  Ogre::HardwareVertexBufferSharedPtr vbuf;
  Ogre::RGBA *pDest=NULL;
  unsigned int i;

  assert(this->font);
  assert(!this->material.isNull());

  // Convert to system-specific
  Ogre::Root::getSingleton().convertColourValue(this->color, & color);

  vbuf = this->renderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);

  pDest = static_cast<Ogre::RGBA*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

  for (i = 0; i < this->renderOp.vertexData->vertexCount; ++i)
  {
    *pDest++ = color;
  }

  vbuf->unlock();
  this->updateColors = false;
}

////////////////////////////////////////////////////////////////////////////////
const Ogre::Quaternion & MovableText::getWorldOrientation(void) const
{
  assert(this->camera);
  return const_cast<Ogre::Quaternion&>(this->camera->getDerivedOrientation());
  //return mParentNode->_getDerivedOrientation();
}

////////////////////////////////////////////////////////////////////////////////
const Ogre::Vector3 & MovableText::getWorldPosition(void) const
{
  assert(mParentNode);
  return mParentNode->_getDerivedPosition();
}

////////////////////////////////////////////////////////////////////////////////
const Ogre::AxisAlignedBox &MovableText::getBoundingBox(void) const
{
  return this->aabb;
}

////////////////////////////////////////////////////////////////////////////////
const Ogre::String &MovableText::getMovableType() const
{
  static Ogre::String movType = "MovableText";
  return movType;
}

////////////////////////////////////////////////////////////////////////////////
void MovableText::getWorldTransforms(Ogre::Matrix4 * xform) const
{
  if (this->isVisible() && this->camera)
  {
    Ogre::Matrix3 rot3x3, scale3x3 = Ogre::Matrix3::IDENTITY;

    // store rotation in a matrix
    this->camera->getDerivedOrientation().ToRotationMatrix(rot3x3);
    //mParentNode->_getDerivedOrientation().ToRotationMatrix(rot3x3);

    // parent node position
    Ogre::Vector3 ppos = mParentNode->_getDerivedPosition() + Ogre::Vector3::UNIT_Y * this->baseline;

//    std::cout << "Parent Pos[" << ppos << "]\n";

    // apply scale
    scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
    scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
    scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;

    // apply all transforms to xform
    *xform = (rot3x3 * scale3x3);
    xform->setTrans(ppos);
  }
}

////////////////////////////////////////////////////////////////////////////////
float MovableText::getBoundingRadius() const
{
  return this->radius;
}

////////////////////////////////////////////////////////////////////////////////
float MovableText::getSquaredViewDepth(const Ogre::Camera *cam) const
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void MovableText::getRenderOperation(Ogre::RenderOperation & op)
{
  if (this->isVisible())
  {
    if (this->needUpdate)
      this->_setupGeometry();
    if (this->updateColors)
      this->_updateColors();
    op = this->renderOp;
  }
}

////////////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr &MovableText::getMaterial(void) const
{
  assert(!this->material.isNull());
  return this->material;
}

////////////////////////////////////////////////////////////////////////////////
//
const Ogre::LightList &MovableText::getLights(void) const
{
  return this->lightList;
}

////////////////////////////////////////////////////////////////////////////////
void MovableText::_notifyCurrentCamera(Ogre::Camera *cam)
{
  this->camera = cam;
}

////////////////////////////////////////////////////////////////////////////////
void MovableText::_updateRenderQueue(Ogre::RenderQueue* queue)
{
  if (this->isVisible())
  {
    if (this->needUpdate)
      this->_setupGeometry();

    if (this->updateColors)
      this->_updateColors();

    queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Method to allow a caller to abstractly iterate over the Renderable instances
void MovableText::visitRenderables( Ogre::Renderable::Visitor* /*visitor*/,
                                 bool /*debug*/) 
{
  return;
}


