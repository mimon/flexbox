#include <iostream>
#include "flexbox_ui_state.h"
#include "test/catch.hpp"

using namespace flexbox;
TEST_CASE("Single node", "[ui_state]") {

  flexbox_ui_state node;

  SECTION("Set flag") {
    node.set(flexbox_ui_state_flag::pressed);
    node.set(flexbox_ui_state_flag::unpressed);

    CHECK(node.test(flexbox_ui_state_flag::pressed) == true);
    CHECK(node.test(flexbox_ui_state_flag::unpressed) == true);
    CHECK(node.test(flexbox_ui_state_flag::hover) == false);
  }

  SECTION("Set multiple flags at once") {
    node.set(flexbox_ui_state_flag::pressed);
    node.set(flexbox_ui_state_flag::unpressed);

    CHECK(node.test(flexbox_ui_state_flag::pressed) == true);
    CHECK(node.test(flexbox_ui_state_flag::unpressed) == true);
    CHECK(node.test(flexbox_ui_state_flag::hover) == false);
  }

  SECTION("Unset flag") {
    node.set(flexbox_ui_state_flag::pressed);
    node.set(flexbox_ui_state_flag::unpressed);

    CHECK(node.test(flexbox_ui_state_flag::pressed) == true);
    CHECK(node.test(flexbox_ui_state_flag::unpressed) == true);

    node.reset(flexbox_ui_state_flag::unpressed);

    CHECK(node.test(flexbox_ui_state_flag::unpressed) == false);
  }
}

TEST_CASE("Yielding individual states", "[ui_state]") {

  flexbox_ui_state node;
  flexbox_ui_state state;

  SECTION("Should not yield a 'click' state") {
    REQUIRE(node.to_ulong() == flexbox_ui_state_flag::none);

    node.set(flexbox_ui_state_flag::pressed);
    node.set(flexbox_ui_state_flag::unpressed);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::click) == false);
  }

  SECTION("Should yield a 'click' state") {
    REQUIRE(node.to_ulong() == flexbox_ui_state_flag::none);

    node.set(flexbox_ui_state_flag::pressed);
    node.set(flexbox_ui_state_flag::unpressed);
    node.set(flexbox_ui_state_flag::hover);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::click) == true);
  }

  SECTION("Should yield a 'unpressed' state") {
    REQUIRE(node.to_ulong() == flexbox_ui_state_flag::none);

    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::unpressed);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::unpressed) == true);
  }

  SECTION("Should yield a 'pressed' state") {
    REQUIRE(node.to_ulong() == flexbox_ui_state_flag::none);

    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::pressed);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::pressed) == true);
  }

  SECTION("Should yield a 'hover' state") {
    REQUIRE(node.to_ulong() == flexbox_ui_state_flag::none);

    node.set(flexbox_ui_state_flag::hover);
    state = node.transition();

    CHECK(state.test(flexbox_ui_state_flag::hover) == true);
    // The hover flag should be unset after every call to transition()
    CHECK(node.test(flexbox_ui_state_flag::hover) == false);
  }
}

TEST_CASE("Point and click scenarios") {
  flexbox_ui_state node;
  flexbox_ui_state state;

  SECTION("Click and drag") {
    // Pretend we click on this node
    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::pressed);
    // Step forward in time
    node.transition();
    // We still have our mouse button down, and now we unpress it ontop of this node
    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::unpressed);

    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::click) == true);

    // Calculating the state again should not yield any clicks
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::click) == false);
  }

  SECTION("Button down and then drag out of the node") {
    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::pressed);
    node.transition();

    // Calculate new state without setting any new flags,
    // i.e pretend that the mouse have gone out of the node's area
    state = node.transition();
    CHECK(state.to_ulong() == flexbox_ui_state_flag::none);
  }

  SECTION("Button down and hover") {
    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::pressed);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::pressed) == true);

    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::unpressed);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::click) == true);
  }

  SECTION("Button down and then button up") {
    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::pressed);
    node.transition();

    // Calculate new state a couple of times without setting any new flags,
    // i.e pretend that the mouse have gone out of the node's area
    node.transition();
    node.transition();
    node.transition();

    node.set(flexbox_ui_state_flag::hover);
    node.set(flexbox_ui_state_flag::unpressed);
    state = node.transition();
    CHECK(state.test(flexbox_ui_state_flag::click) == true);
  }
}