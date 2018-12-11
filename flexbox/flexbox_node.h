#pragma once
#include <memory>
#include <vector>
#include "OgreHlmsUnlitDatablock.h"
#include "OgrePrerequisites.h"
#include "OgreVector3.h"
#include "Yoga.h"
#include "flexbox_canvas.h"
#include "flexbox_vertex_buffer.h"
namespace flexbox {

typedef std::unique_ptr<Ogre::HlmsUnlitDatablock, std::function<void(Ogre::HlmsUnlitDatablock *)>> unique_datablock;
typedef std::unique_ptr<flexbox_vertex_buffer, std::function<void(flexbox_vertex_buffer *)>>       unique_vertex_buffer;
typedef std::unique_ptr<Ogre::SceneNode, std::function<void(Ogre::SceneNode *)>>                   unique_scene_node;

class flexbox_node {
  public:
  flexbox_node(flexbox_stylesheet const &style = flexbox_stylesheet(), Ogre::uint8 render_queue_id = 200);
  flexbox_node(Ogre::SceneManager *scene_mgr, flexbox_stylesheet const &style = flexbox_stylesheet(), Ogre::uint8 render_queue_id = 200);
  flexbox_node(flexbox_node *parent, flexbox_stylesheet const &style = flexbox_stylesheet());

  std::shared_ptr<flexbox_node> create_child(const std::shared_ptr<flexbox_canvas> &r = std::shared_ptr<flexbox_canvas>());
  std::shared_ptr<flexbox_node> create_child(const std::shared_ptr<flexbox_canvas> &r, const flexbox_stylesheet &style);

  void enable_rendering_order_by_creation();

  void setup_renderable_and_datablock(Ogre::uint8 render_queue_id);

  void layout(int available_width, int available_height);

  void layout_apply_style();

  void layout_mark_dirty();

  void set_view_matrix(int available_width, int available_height);

  void transform_layout_to_view_space(const Ogre::Vector3 &relative_to = Ogre::Vector3(0, 0, 0));

  void scale_dimensions_to_view_space();

  void layout_and_paint(int available_width, int available_height, const Ogre::Vector3 &origin = Ogre::Vector3(0, 0, 0));

  Ogre::Matrix3 view_matrix = Ogre::Matrix3::IDENTITY;

  unique_scene_node    scene_node;
  unique_datablock     datablock;
  unique_vertex_buffer vertex_buffer;

  std::shared_ptr<flexbox_canvas> canvas;
  flexbox_stylesheet              stylesheet;
  Ogre::TexturePtr                ogre_texture;

  std::unique_ptr<YGNode, decltype(&YGNodeFree)> layout_node;
  std::vector<std::shared_ptr<flexbox_node>> children;
  flexbox_node *                             parent = nullptr;

  Ogre::Vector2 computed_layout_position, computed_layout_dimensions;

  Ogre::HlmsMacroblock macro_block;
  Ogre::HlmsBlendblock blend_block;
  Ogre::uint8          render_queue_id = 200;

  static unsigned long texture_count;
};
}