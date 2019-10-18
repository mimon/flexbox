#include "flexbox_node.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsUnlit.h"
#include "OgreHlmsUnlitDatablock.h"
#include "OgreRoot.h"
#include "OgreSceneManager.h"
#include "OgreTextureManager.h"

namespace flexbox {
using namespace Ogre;

namespace {
  static YGSize measure_size(YGNodeRef     node,
                             float         width,
                             YGMeasureMode widthMode,
                             float         height,
                             YGMeasureMode heightMode) {
    void *        ptr = YGNodeGetContext(node);
    flexbox_node *rn  = static_cast<flexbox_node *>(ptr);

    // Don't take up space if we haven't anything to render
    if (!rn->canvas) {
      return YGSize{ 0, 0 };
    }

    Ogre::Vector2 s = rn->canvas->measure(rn->stylesheet, width, widthMode, height, heightMode);

    return YGSize{ s.x, s.y };
  }

  void destroy_vertex_buffer(flexbox_vertex_buffer *obj) {
    auto node = obj->getParentSceneNode();
    if (node != nullptr) {
      node->detachObject(obj);
    }
    delete obj;
  }
}

unsigned long flexbox_node::texture_count = 0;

flexbox_node::flexbox_node(flexbox_stylesheet const &style, Ogre::uint8 render_queue_id)
  : scene_node(nullptr, [](SceneNode *node) { node->getCreator()->destroySceneNode(node); }),
    layout_node(nullptr, &YGNodeFree),
    datablock(nullptr, [](Ogre::HlmsUnlitDatablock *d) { d->getCreator()->destroyDatablock(d->getName()); }),
    stylesheet(style),
    vertex_buffer(nullptr, &destroy_vertex_buffer),
    render_queue_id(render_queue_id) {
}

flexbox_node::flexbox_node(Ogre::SceneManager *scene_mgr, flexbox_stylesheet const &style, Ogre::uint8 render_queue_id)
  : flexbox_node(style, render_queue_id) {
  this->scene_node.reset(scene_mgr->getRootSceneNode()->createChildSceneNode());
  this->layout_node.reset(YGNodeNew());
  this->setup_renderable_and_datablock(render_queue_id);
  this->enable_rendering_order_by_creation();
}

flexbox_node::flexbox_node(flexbox_node *parent, flexbox_stylesheet const &style)
  : flexbox_node(style, parent->render_queue_id) {
  this->parent = parent;
  this->layout_node.reset(YGNodeNew());

  SceneNode *   sn = parent->scene_node.get();
  SceneManager *sm = sn->getCreator();

  this->scene_node.reset(sm->getRootSceneNode()->createChildSceneNode());
  this->setup_renderable_and_datablock(this->render_queue_id);
}

void flexbox_node::enable_rendering_order_by_creation() {
  Ogre::SceneManager *mgr = this->scene_node->getCreator();
  mgr->getRenderQueue()->setSortRenderQueue(this->render_queue_id, Ogre::RenderQueue::DisableSort);
  mgr->getRenderQueue()->setRenderQueueMode(this->render_queue_id, Ogre::RenderQueue::FAST);
}

void flexbox_node::setup_renderable_and_datablock(Ogre::uint8 render_queue_id) {
  SceneManager *sm                   = this->scene_node->getCreator();
  this->macro_block.mDepthCheck      = true;
  this->macro_block.mDepthWrite      = false;
  this->macro_block.mCullMode        = Ogre::CULL_NONE;
  this->blend_block.mDestBlendFactor = Ogre::SBF_ONE_MINUS_SOURCE_ALPHA;

  Ogre::HlmsManager *hlms_manager = Ogre::Root::getSingleton().getHlmsManager();
  Ogre::HlmsUnlit *  hlms_unlit   = static_cast<Ogre::HlmsUnlit *>(
      hlms_manager->getHlms(Ogre::HLMS_UNLIT));

  auto a = hlms_unlit->createDatablock(Ogre::Id::generateNewId<flexbox_node>(),
                                       "Ogre::Id::generateNewId<flexbox_node>()",
                                       macro_block, blend_block, Ogre::HlmsParamVec());
  auto b = static_cast<Ogre::HlmsUnlitDatablock *>(a);
  this->datablock.reset(b);

  vertex_buffer.reset(new flexbox_vertex_buffer(Ogre::Id::generateNewId<Ogre::MovableObject>(),
                                                &sm->_getEntityMemoryManager(Ogre::SCENE_DYNAMIC),
                                                sm, 0, b));
  this->vertex_buffer->setUseIdentityProjection(true);
  this->vertex_buffer->setRenderQueueGroup(this->render_queue_id);
  this->scene_node->attachObject(this->vertex_buffer.get());
  this->vertex_buffer->setVisible(false);
}

std::shared_ptr<flexbox_node> flexbox_node::create_child(const std::shared_ptr<flexbox_canvas> &renderable) {
  flexbox_stylesheet style = create_and_inherit(this->stylesheet);
  return this->create_child(renderable, style);
}
std::shared_ptr<flexbox_node> flexbox_node::create_child(const std::shared_ptr<flexbox_canvas> &renderable,
                                                         const flexbox_stylesheet &             style) {
  // We are not longer a leaf
  YGNodeSetMeasureFunc(this->layout_node.get(), nullptr);

  auto child = std::make_shared<flexbox_node>(this, style);

  if (!renderable) {
    child->canvas = child->canvas.make_shared();
  } else {
    child->canvas = renderable;
  }
  auto &c = this->children;
  c.push_back(child);
  YGNodeInsertChild(this->layout_node.get(), child->layout_node.get(), c.size() - 1);
  YGNodeSetContext(child->layout_node.get(), static_cast<void *>(child.get()));
  YGNodeSetMeasureFunc(child->layout_node.get(), &measure_size);

  return child;
}

void flexbox_node::set_view_matrix(int available_width, int available_height) {
  Ogre::Vector3 X, Y, Z, W;
  X = Ogre::Vector3(2.0f / available_width, 0, 0);
  Y = Ogre::Vector3(0, -2.0f / available_height, 0);
  Z = Ogre::Vector3(-1.0, 1.0, 0);
  W = Ogre::Vector3(-1, -1.0, 0);
  this->view_matrix.FromAxes(X, Y, Z);
}

void flexbox_node::layout(int available_width, int available_height) {
  this->set_view_matrix(available_width, available_height);
  this->layout_apply_style();
  for (auto child : this->children) {
    child->layout(available_width, available_height);
  }
  if (this->parent == nullptr) {
    YGNodeCalculateLayout(this->layout_node.get(), available_width, available_height, YGDirectionLTR);
  }
}

void flexbox_node::layout_apply_style() {
  const auto &rs   = this->stylesheet;
  YGNode *    node = this->layout_node.get();

  if (rs.width.value >= 0) {
    if (rs.width.is_pixel) {
      YGNodeStyleSetWidth(node, rs.width.value);
    } else {
      YGNodeStyleSetWidthPercent(node, rs.width.value);
    }
  }
  if (rs.height.value >= 0) {
    if (rs.height.is_pixel) {
      YGNodeStyleSetHeight(node, rs.height.value);
    } else {
      YGNodeStyleSetHeightPercent(node, rs.height.value);
    }
  }

  YGNodeStyleSetPosition(node, YGEdgeLeft, rs.left);
  YGNodeStyleSetPosition(node, YGEdgeTop, rs.top);

  YGNodeStyleSetMargin(node, YGEdgeTop, this->stylesheet.margin_top);
  YGNodeStyleSetMargin(node, YGEdgeLeft, this->stylesheet.margin_left);
  YGNodeStyleSetMargin(node, YGEdgeBottom, this->stylesheet.margin_bottom);
  YGNodeStyleSetMargin(node, YGEdgeRight, this->stylesheet.margin_right);

  YGNodeStyleSetPadding(node, YGEdgeTop, this->stylesheet.padding_top);
  YGNodeStyleSetPadding(node, YGEdgeLeft, this->stylesheet.padding_left);
  YGNodeStyleSetPadding(node, YGEdgeBottom, this->stylesheet.padding_bottom);
  YGNodeStyleSetPadding(node, YGEdgeRight, this->stylesheet.padding_right);

  YGNodeStyleSetFlexDirection(node, this->stylesheet.flex_direction);
  YGNodeStyleSetFlexGrow(node, this->stylesheet.flex_grow);
  YGNodeStyleSetFlexShrink(node, this->stylesheet.flex_shrink);
  YGNodeStyleSetJustifyContent(node, this->stylesheet.justify_content);
  YGNodeStyleSetAlignItems(node, this->stylesheet.align_items);
  YGNodeStyleSetOverflow(node, YGOverflowHidden);
  YGNodeStyleSetPositionType(node, this->stylesheet.position_type);
}

void flexbox_node::transform_layout_to_view_space(const Ogre::Vector3 &relative_to) {
  Ogre::Vector3 position;
  position.x = YGNodeLayoutGetLeft(this->layout_node.get());
  position.y = YGNodeLayoutGetTop(this->layout_node.get());
  position.z = 1.0;

  this->computed_layout_position = Ogre::Vector2(position.x, position.y) + Ogre::Vector2(relative_to.x, relative_to.y);

  Ogre::Vector3 derived_position(this->computed_layout_position.x, this->computed_layout_position.y, position.z);
  position = this->view_matrix * derived_position;

  this->scene_node->setPosition(Ogre::Vector3(position.x, position.y, position.z));
}

void flexbox_node::scale_dimensions_to_view_space() {
  Ogre::Vector2 dimensions;
  dimensions.x = YGNodeLayoutGetWidth(this->layout_node.get());
  dimensions.y = YGNodeLayoutGetHeight(this->layout_node.get());

  Ogre::Vector3 scale_to_dimensions = this->view_matrix * Ogre::Vector3(dimensions.x, -dimensions.y, 0);
  auto          s                   = Ogre::Vector3(scale_to_dimensions.x, scale_to_dimensions.y, 0);
  this->scene_node->setScale(s);
  this->computed_layout_dimensions = dimensions;
}

void flexbox_node::layout_and_paint(int available_width, int available_height, const Ogre::Vector3 &origin) {
  this->layout(available_width, available_height);
  this->transform_layout_to_view_space(origin);
  this->scale_dimensions_to_view_space();

  const bool zero_dimension = (this->computed_layout_dimensions.x <= 0.0) || (this->computed_layout_dimensions.y <= 0.0);
  if (zero_dimension) {
    return;
  }

  if (this->canvas) {
    this->vertex_buffer->setVisible(true);
    if (!this->canvas->sdl_surface.get()) {
      this->canvas->paint(this->stylesheet, this->computed_layout_dimensions);
    }

    if (this->ogre_texture) {
      auto &tmgr = Ogre::TextureManager::getSingleton();
      tmgr.remove(this->ogre_texture);
    }

    const unsigned int texture_type = 0;
    this->ogre_texture              = this->canvas->to_texture();
    this->datablock->setTexture(texture_type, 0, this->ogre_texture);
  }

  for (auto child : this->children) {
    auto const &parent_position = Ogre::Vector3(this->computed_layout_position.x, this->computed_layout_position.y, 0);
    child->layout_and_paint(available_width, available_height, parent_position);
  }
}

void flexbox_node::layout_mark_dirty() {
  if (this->canvas) {
    // This will cause a call to paint() in this->render()
    this->canvas->clear_surfaces();
    this->canvas->free_surfaces();
  }
  if (this->children.size() == 0) {
    YGNodeMarkDirty(this->layout_node.get());
  } else {
    for (auto &child : this->children) {
      child->layout_mark_dirty();
    }
  }
}
}
