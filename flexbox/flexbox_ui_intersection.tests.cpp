#include <iostream>
#include "flexbox_ui_intersection.h"
#include "test/catch.hpp"

using namespace flexbox;
TEST_CASE("One rectangle", "[flexbox_ui_intersection]") {

  flexbox_ui_intersection<float>                clickables;
  flexbox_ui_intersection<float>::result_vector result;
  Ogre::Vector2                                 position, dimensions;
  float                                         obj = 1.0;
  position                                          = { 8, 8 };
  dimensions                                        = { 4, 4 };

  clickables.insert(position, dimensions, obj);

  SECTION("Inside the rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(10, 10));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == obj);
  }

  SECTION("On the border the rectangle should not intersect") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(8, 8));
    REQUIRE(result.size() == 0);
  }

  SECTION("Inside the rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(8.1, 8.1));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == obj);
  }

  SECTION("Outside the rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(13, 10));
    REQUIRE(result.size() == 0);
  }

  SECTION("Outside the rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(10, 13));
    REQUIRE(result.size() == 0);
  }
}

TEST_CASE("Two rectangles", "[flexbox_ui_intersection]") {

  flexbox_ui_intersection<float>                clickables;
  flexbox_ui_intersection<float>::result_vector result;
  Ogre::Vector2                                 position, position2, dimensions, dimensions2;
  float                                         background = 1.0;
  float                                         foreground = 1.0;

  position    = { 8, 8 };
  dimensions  = { 4, 4 };
  position2   = { 9, 9 };
  dimensions2 = { 4, 4 };

  clickables.insert(position, dimensions, background);
  // The following will appear in foreground
  clickables.insert(position2, dimensions2, foreground);

  SECTION("Click on foreground rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(10, 10));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == foreground);
  }

  SECTION("Click on background rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(8.5, 8.5));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == background);
  }

  SECTION("Get both rectangles in the result when they overlap") {
    bool multiple_results = true;
    result                = clickables.get_intersecting_intersectables(Ogre::Vector2(10, 10), multiple_results);
    REQUIRE(result.size() == 2);
    CHECK(result[0] == foreground);
    CHECK(result[1] == background);
  }

  SECTION("Outside the rectangles") {
    bool multiple_results = true;
    result                = clickables.get_intersecting_intersectables(Ogre::Vector2(10, 14), multiple_results);
    REQUIRE(result.size() == 0);
  }
}

TEST_CASE("Insertion/Removal") {

  flexbox_ui_intersection<float>                clickables;
  flexbox_ui_intersection<float>::result_vector result;
  Ogre::Vector2                                 position, position2, dimensions, dimensions2;
  float                                         background = 1.0;
  float                                         foreground = 1.0;

  position    = { 8, 8 };
  dimensions  = { 4, 4 };
  position2   = { 9, 9 };
  dimensions2 = { 4, 4 };

  clickables.insert(position, dimensions, background);
  // The following will appear in foreground
  clickables.insert(position2, dimensions2, foreground);

  SECTION("Click on foreground rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(10, 10));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == foreground);
  }

  SECTION("Click on background rectangle") {
    result = clickables.get_intersecting_intersectables(Ogre::Vector2(8.5, 8.5));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == background);
  }
}
