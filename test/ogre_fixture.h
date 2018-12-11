#include "Ogre.h"
#include "OgreHlmsUnlit.h"
#include "OgreNULLPlugin.h"

namespace flexbox {
class ogre_fixture {
  public:
  ogre_fixture() {

    ogre_root.installPlugin(&ogre_null_plugin);

    Ogre::RenderSystem *rs = ogre_root.getRenderSystemByName("NULL Rendering Subsystem");
    ogre_root.setRenderSystem(rs);

    ogre_root.initialise(true);

    // Ogre::HlmsNull<Ogre::Hlms *pbs   = OGRE_NEW Ogre::HlmsNull(Ogre::HlmsTypes::HLMS_PBS, "pbs");
    Ogre::HlmsUnlit *unlit = OGRE_NEW Ogre::HlmsUnlit(nullptr, nullptr);

    ogre_root.getHlmsManager()->registerHlms(unlit);
    // ogre_root.getHlmsManager()->registerHlms(pbs);

    ogre_scene_mgr = ogre_root.createSceneManager(Ogre::ST_GENERIC, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD);
  }
  // ~ogre_fixture() {
  //   ogre_root.uninstallPlugin(&ogre_null_plugin);
  // };
  Ogre::NULLPlugin    ogre_null_plugin;
  Ogre::Root          ogre_root;
  Ogre::SceneManager *ogre_scene_mgr;
};
}