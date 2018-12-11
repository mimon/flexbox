#pragma once
#include <iostream>
#include "OgreHlmsUnlitDatablock.h"
#include "OgreMovableObject.h"
#include "OgreRenderable.h"

namespace flexbox {
///  What does this class do?
class flexbox_vertex_buffer : public Ogre::MovableObject, public Ogre::Renderable {
  public:
  flexbox_vertex_buffer(Ogre::IdType id, Ogre::ObjectMemoryManager *objectMemoryManager,
                        Ogre::SceneManager *manager, Ogre::uint8 renderQueueId, Ogre::HlmsUnlitDatablock *block);
  ~flexbox_vertex_buffer();
  flexbox_vertex_buffer(const flexbox_vertex_buffer &) = delete;

  void createBuffers(void);

  //Overrides from MovableObject
  virtual const Ogre::String &getMovableType(void) const;

  //Overrides from Renderable
  virtual const Ogre::LightList &getLights(void) const;
  virtual void getRenderOperation(Ogre::v1::RenderOperation &op, bool casterPass);
  virtual void getWorldTransforms(Ogre::Matrix4 *xform) const;
  virtual bool getCastsShadows(void) const;
};
}