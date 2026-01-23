import SwiftUI

@main
struct ClipboardManagerApp: App {
    @StateObject private var manager = ClipboardManager()
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    
    var body: some Scene {
        Settings {
            EmptyView()
        }
    }
    
    init() {
        // We initialize the manager here and pass it to the appDelegate
        // In a real app we might use a shared singleton or environment
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    var statusItem: NSStatusItem!
    var popover: NSPopover!
    var manager = ClipboardManager()
    
    func applicationDidFinishLaunching(_ notification: Notification) {
        // Create the popover
        let popover = NSPopover()
        popover.contentSize = NSSize(width: 300, height: 450)
        popover.behavior = .transient
        popover.contentViewController = NSHostingController(rootView: MainView(manager: manager))
        self.popover = popover
        
        // Create the status item
        statusItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
        
        if let button = statusItem.button {
            button.title = "📋"
            button.action = #selector(togglePopover(_:))
        }
    }
    
    @objc func togglePopover(_ sender: AnyObject?) {
        if let button = statusItem.button {
            if popover.isShown {
                popover.performClose(sender)
            } else {
                popover.show(relativeTo: button.bounds, of: button, preferredEdge: .minY)
            }
        }
    }
}
