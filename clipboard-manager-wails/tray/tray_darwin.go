package tray

/*
#cgo CFLAGS: -x objective-c
#cgo LDFLAGS: -framework Cocoa

#import <Cocoa/Cocoa.h>

extern void createStatusItem(void);
*/
import "C"
import "sync"

var onClick func()
var once sync.Once

//export trayClicked
func trayClicked() {
	if onClick != nil {
		onClick()
	}
}

func setupTrayImpl(clickHandler func()) {
	onClick = clickHandler
	once.Do(func() {
		C.createStatusItem()
	})
}
