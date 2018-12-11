#include "flexbox_stylesheet.h"
#include "test/catch.hpp"
#include "test/ogre_fixture.h"

using namespace flexbox;
TEST_CASE("Padding and Margin", "[flexbox_stylesheet]") {
  SECTION("padding shortcut") {
    flexbox_stylesheet sheet;

    sheet.set_padding(pixels(10));
    CHECK(sheet.padding_top == 10);
    CHECK(sheet.padding_right == 10);
    CHECK(sheet.padding_bottom == 10);
    CHECK(sheet.padding_left == 10);

    sheet.set_padding(pixels(20), pixels(30));
    CHECK(sheet.padding_top == 20);
    CHECK(sheet.padding_right == 30);
    CHECK(sheet.padding_bottom == 20);
    CHECK(sheet.padding_left == 30);

    sheet.set_padding(pixels(40), pixels(50), pixels(60));
    CHECK(sheet.padding_top == 40);
    CHECK(sheet.padding_right == 50);
    CHECK(sheet.padding_bottom == 60);
    CHECK(sheet.padding_left == 50);

    sheet.set_padding(pixels(70), pixels(80), pixels(90), pixels(100));
    CHECK(sheet.padding_top == 70);
    CHECK(sheet.padding_right == 80);
    CHECK(sheet.padding_bottom == 90);
    CHECK(sheet.padding_left == 100);
  }

  SECTION("margin shortcut") {
    flexbox_stylesheet sheet;

    sheet.set_margin(pixels(10));
    CHECK(sheet.margin_top == 10);
    CHECK(sheet.margin_right == 10);
    CHECK(sheet.margin_bottom == 10);
    CHECK(sheet.margin_left == 10);

    sheet.set_margin(pixels(20), pixels(30));
    CHECK(sheet.margin_top == 20);
    CHECK(sheet.margin_right == 30);
    CHECK(sheet.margin_bottom == 20);
    CHECK(sheet.margin_left == 30);

    sheet.set_margin(pixels(40), pixels(50), pixels(60));
    CHECK(sheet.margin_top == 40);
    CHECK(sheet.margin_right == 50);
    CHECK(sheet.margin_bottom == 60);
    CHECK(sheet.margin_left == 50);

    sheet.set_margin(pixels(70), pixels(80), pixels(90), pixels(100));
    CHECK(sheet.margin_top == 70);
    CHECK(sheet.margin_right == 80);
    CHECK(sheet.margin_bottom == 90);
    CHECK(sheet.margin_left == 100);
  }
}

TEST_CASE("Stylesheet construction", "[flexbox_stylesheet]") {
  SECTION("Inheriting from a stylesheet should copy a subset of stylesheet proeprties") {
    flexbox_stylesheet sheet;
    sheet.background_color = Ogre::ColourValue::Blue;
    sheet.color            = Ogre::ColourValue::Red;
    sheet.set_padding(pixels(20));
    sheet.font_family = "Futura";

    flexbox_stylesheet inherited_sheet = create_and_inherit(sheet);
    CHECK(inherited_sheet.padding_top == 0);
    CHECK(inherited_sheet.font_family == "Futura");
    CHECK(inherited_sheet.color == Ogre::ColourValue::Red);
    CHECK(inherited_sheet.background_color == Ogre::ColourValue::ZERO);
  }
}
