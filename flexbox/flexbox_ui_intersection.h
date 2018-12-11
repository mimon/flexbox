#pragma once
#include <list>
#include "OgreVector2.h"

namespace flexbox {
/// Add positions and dimensions of objects and
/// test if they intersects with a given input.
template <typename UserType>
class flexbox_ui_intersection {
  public:
  struct intersectable {
    const UserType object;
    Ogre::Vector2  position, dimensions;
  };

  typedef std::vector<UserType>    result_vector;
  typedef std::list<intersectable> list;

  void insert(const Ogre::Vector2 &position, const Ogre::Vector2 &dimensions, const UserType &object) {
    auto it = this->intersectables_by_zindex.begin();
    this->intersectables_by_zindex.insert(it, { object, position, dimensions });
  }

  void insert(std::size_t z_index, const Ogre::Vector2 &position, const Ogre::Vector2 &dimensions, UserType *object) {
    auto it = this->intersectables_by_zindex.begin();
    this->intersectables_by_zindex.insert(it + z_index, { object, position, dimensions });
  }

  // http://math.stackexchange.com/questions/190111/how-to-check-if-a-point-is-inside-a-rectangle/190373#190373
  result_vector get_intersecting_intersectables(const Ogre::Vector2 &ray_origin, bool multiple_results = false) {
    result_vector results;

    const Ogre::Vector2 &m = ray_origin;
    Ogre::Vector2        a, b, d, am, ab, ad;
    Ogre::Real           am_dot_ab, am_dot_ad, ab_dot_ab, ad_dot_ad;
    auto                 it = this->intersectables_by_zindex.begin();
    for (; it != this->intersectables_by_zindex.end(); ++it) {
      intersectable &current   = *it;
      a                        = current.position;
      b                        = Ogre::Vector2(a.x + current.dimensions.x, a.y);
      d                        = Ogre::Vector2(a.x, a.y + current.dimensions.y);
      am                       = m - a;
      ab                       = b - a;
      ad                       = d - a;
      am_dot_ab                = am.dotProduct(ab);
      am_dot_ad                = am.dotProduct(ad);
      ab_dot_ab                = ab.dotProduct(ab);
      ad_dot_ad                = ad.dotProduct(ad);
      bool       exp1          = am_dot_ab > 0 && am_dot_ab < ab_dot_ab;
      bool       exp2          = am_dot_ad > 0 && am_dot_ad < ad_dot_ad;
      const bool it_intersects = exp1 && exp2;
      if (it_intersects) {
        results.push_back(it->object);
        if (!multiple_results) {
          return results;
        }
      }
    }

    return results;
  }

  list intersectables_by_zindex;
};
}