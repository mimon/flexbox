#include "flexbox_node.h"
#include "test/ogre_fixture.h"

#include "test/catch.hpp"
TEST_CASE("flexbox_node") {
  SECTION("Test") {
    flexbox::ogre_fixture ogre;
    flexbox::flexbox_node root(ogre.ogre_scene_mgr);
    root.layout_and_paint(10, 10);
    CHECK(root.computed_layout_dimensions == Ogre::Vector2(10, 10));
  }
  SECTION("padding") {
    flexbox::ogre_fixture ogre;
    flexbox::flexbox_node root(ogre.ogre_scene_mgr);
    auto                  child    = root.create_child();
    child->stylesheet.width        = 12;
    child->stylesheet.height       = 12;
    child->stylesheet.padding_left = 5;
    root.layout_and_paint(100, 100);

    auto yoga_node = child->layout_node.get();
    CHECK(YGNodeGetMeasureFunc(yoga_node) != nullptr);
    CHECK(YGNodeLayoutGetWidth(yoga_node) == 12.0f);
    CHECK(YGNodeLayoutGetHeight(yoga_node) == 12.0f);
    CHECK(child->computed_layout_dimensions == Ogre::Vector2(12, 12));
  }
  SECTION("Make sure nothing is rendered on screen unless render() has been called") {
    flexbox::ogre_fixture ogre;
    flexbox::flexbox_node root(ogre.ogre_scene_mgr);
    auto                  child_node = root.create_child();

    child_node->stylesheet.width  = 10;
    child_node->stylesheet.height = 10;

    Ogre::SceneNode *    root_scene_node = root.scene_node.get();
    Ogre::MovableObject *root_movable    = root.vertex_buffer.get();
    Ogre::MovableObject *child_movable   = child_node->vertex_buffer.get();

    CHECK(root_scene_node->numAttachedObjects() == 1);
    CHECK(root_movable->isVisible() == false);
    CHECK(child_movable->isVisible() == false);

    root.layout_and_paint(100, 100);
    CHECK(root_movable->isVisible() == false);
    CHECK(child_movable->isVisible() == true);
  }
}
