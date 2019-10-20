#include "app/osx_utils.h"
#import <AppKit/NSWindow.h>
extern unsigned long window_content_view_handle(const SDL_SysWMinfo &info) {
  NSWindow *window = info.info.cocoa.window;
  NSView *view     = [window contentView];
  return (unsigned long)view;
}
