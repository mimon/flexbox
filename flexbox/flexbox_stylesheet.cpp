#include "flexbox_stylesheet.h"

namespace flexbox {

flexbox_stylesheet create_and_inherit(const flexbox_stylesheet &inherit_from) {
  flexbox_stylesheet inherited;
  inherited.font_family = inherit_from.font_family;
  inherited.font_size   = inherit_from.font_size;
  inherited.color       = inherit_from.color;
  return inherited;
}

void flexbox_stylesheet::set_padding(pixels top_right_bottom_left) {
  this->padding_top    = top_right_bottom_left;
  this->padding_right  = top_right_bottom_left;
  this->padding_bottom = top_right_bottom_left;
  this->padding_left   = top_right_bottom_left;
}

void flexbox_stylesheet::set_padding(pixels top_bottom, pixels right_left) {
  this->padding_top    = top_bottom;
  this->padding_bottom = top_bottom;
  this->padding_right  = right_left;
  this->padding_left   = right_left;
}

void flexbox_stylesheet::set_padding(pixels top, pixels right_left, pixels bottom) {
  this->padding_top    = top;
  this->padding_bottom = bottom;
  this->padding_right  = right_left;
  this->padding_left   = right_left;
}

void flexbox_stylesheet::set_padding(pixels top, pixels right, pixels bottom, pixels left) {
  this->padding_top    = top;
  this->padding_bottom = bottom;
  this->padding_right  = right;
  this->padding_left   = left;
}

void flexbox_stylesheet::set_margin(pixels top_right_bottom_left) {
  this->margin_top    = top_right_bottom_left;
  this->margin_right  = top_right_bottom_left;
  this->margin_bottom = top_right_bottom_left;
  this->margin_left   = top_right_bottom_left;
}

void flexbox_stylesheet::set_margin(pixels top_bottom, pixels right_left) {
  this->margin_top    = top_bottom;
  this->margin_bottom = top_bottom;
  this->margin_right  = right_left;
  this->margin_left   = right_left;
}

void flexbox_stylesheet::set_margin(pixels top, pixels right_left, pixels bottom) {
  this->margin_top    = top;
  this->margin_bottom = bottom;
  this->margin_right  = right_left;
  this->margin_left   = right_left;
}

void flexbox_stylesheet::set_margin(pixels top, pixels right, pixels bottom, pixels left) {
  this->margin_top    = top;
  this->margin_bottom = bottom;
  this->margin_right  = right;
  this->margin_left   = left;
}
}