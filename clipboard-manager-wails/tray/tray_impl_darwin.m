#include <Cocoa/Cocoa.h>

// Forward declaration of Go export
extern void trayClicked();

@interface CMTrayHandler : NSObject
@end

@implementation CMTrayHandler
- (void)handleClick:(id)sender {
    trayClicked();
}
@end

static CMTrayHandler *cm_tray_handler;
static NSStatusItem *cm_status_item;

void createStatusItem(void) {
    cm_tray_handler = [[CMTrayHandler alloc] init];
    
    // Create Item in System Status Bar
    cm_status_item = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    
    // Set Icon (Using simple text for now, can perform image loading later)
    cm_status_item.button.title = @"📋";
    
    // Set Action
    [cm_status_item.button setTarget:cm_tray_handler];
    [cm_status_item.button setAction:@selector(handleClick:)];
}
