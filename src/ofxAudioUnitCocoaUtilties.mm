#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"

#if !(TARGET_OS_IPHONE)
#include <CoreAudioKit/CoreAudioKit.h>
#include <AudioUnit/AUCocoaUIView.h>

#pragma mark Objective-C

@interface ofxAudioUnitUIWindow : NSWindow
{
	NSView * _AUView;
}

- (id) initWithAudioUnit:(AudioUnit)unit forceGeneric:(BOOL)useGeneric;

@end

@implementation ofxAudioUnitUIWindow

// ----------------------------------------------------------
- (void) dealloc
// ----------------------------------------------------------
{
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSViewFrameDidChangeNotification
												  object:_AUView];
	[super dealloc];
}

// ----------------------------------------------------------
- (instancetype) initWithAudioUnit:(AudioUnit)unit forceGeneric:(BOOL)useGeneric
// ----------------------------------------------------------
{
	if(useGeneric) {
		return [self initWithGenericViewForUnit:unit];
	} else if([ofxAudioUnitUIWindow audioUnitHasCocoaView:unit]) {
		if(![self initWithCocoaViewForUnit:unit]) {
			return nil;
		}
	} else if([ofxAudioUnitUIWindow audioUnitHasCarbonView:unit]) {
		[self printUnsupportedCarbonMessage:unit];
	} else {
		return [self initWithGenericViewForUnit:unit];
	}
	
	return self;
}

// ----------------------------------------------------------
- (instancetype) initWithCocoaViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	// getting the size of the AU View info
	UInt32 dataSize;
	Boolean isWriteable;
	OSStatus result = AudioUnitGetPropertyInfo(unit,
											   kAudioUnitProperty_CocoaUI,
											   kAudioUnitScope_Global,
											   0,
											   &dataSize,
											   &isWriteable);
	
	UInt32 numberOfClasses = (dataSize - sizeof(CFURLRef)) / sizeof(CFStringRef);
	
	NSView * AUView = nil;
	
	if((result == noErr) && (numberOfClasses > 0)) {
		AudioUnitCocoaViewInfo * cocoaViewInfo = (AudioUnitCocoaViewInfo *)malloc(dataSize);
		OSStatus success = AudioUnitGetProperty(unit,
												kAudioUnitProperty_CocoaUI,
												kAudioUnitScope_Global,
												0,
												cocoaViewInfo,
												&dataSize);
		if(success == noErr && cocoaViewInfo) {
			CFURLRef cocoaViewBundlePath = cocoaViewInfo->mCocoaAUViewBundleLocation;
			CFStringRef factoryClassName = cocoaViewInfo->mCocoaAUViewClass[0];
			NSBundle * viewBundle = [NSBundle bundleWithURL:(NSURL *)cocoaViewBundlePath];
			
			if(viewBundle) {
				Class factoryClass = [viewBundle classNamed:(NSString *)factoryClassName];
				id<AUCocoaUIBase> factoryInstance = [[[factoryClass alloc] init] autorelease];
				AUView = [factoryInstance uiViewForAudioUnit:unit withSize:NSZeroSize];
			}
		}
		
		free(cocoaViewInfo);
	}
	
	if(AUView) {
		return [self initWithAudioUnitCocoaView:AUView];
	} else {
		NSLog(@"Failed to create AU view");
		return nil;
	}
}

// ----------------------------------------------------------
- (instancetype) initWithGenericViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	AUGenericView * AUView = [[[AUGenericView alloc] initWithAudioUnit:unit] autorelease];
	[AUView setShowsExpertParameters:YES];
	return [self initWithAudioUnitCocoaView:AUView];
}

// ----------------------------------------------------------
- (instancetype) initWithAudioUnitCocoaView:(NSView *)audioUnitView
// ----------------------------------------------------------
{
	_AUView = [audioUnitView retain];
	NSRect contentRect = NSMakeRect(0, 0, audioUnitView.frame.size.width, audioUnitView.frame.size.height);
	self = [super initWithContentRect:contentRect
<<<<<<< Updated upstream
							styleMask:(NSTitledWindowMask |
									   NSClosableWindowMask |
									   NSMiniaturizableWindowMask)
=======
							styleMask:(NSWindowStyleMaskTitled |
									   NSWindowStyleMaskClosable |
									   NSWindowStyleMaskMiniaturizable)
>>>>>>> Stashed changes
							  backing:NSBackingStoreBuffered
								defer:YES];
	if(self)
	{
		self.level = NSNormalWindowLevel;
		self.contentView = _AUView;
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(audioUnitChangedViewSize:)
													 name:NSViewFrameDidChangeNotification
												   object:_AUView];
	}
	return self;
}

// ----------------------------------------------------------
- (void) printUnsupportedCarbonMessage:(AudioUnit)unit
// ----------------------------------------------------------
{
	NSString * msg = @"This audio unit only has a Carbon-based UI. Carbon\
	support has been removed from ofxAudioUnit. Checkout the \"carbon\" tag\
	for the last commit which has Carbon support";
	
	NSLog(@"%@", msg);
}

// ----------------------------------------------------------
+ (BOOL) audioUnitHasCocoaView:(AudioUnit)unit
// ----------------------------------------------------------
{
	UInt32 dataSize;
	UInt32 numberOfClasses;
	Boolean isWriteable;
	
	OSStatus result = AudioUnitGetPropertyInfo(unit,
											   kAudioUnitProperty_CocoaUI,
											   kAudioUnitScope_Global,
											   0,
											   &dataSize,
											   &isWriteable);
	
	numberOfClasses = (dataSize - sizeof(CFURLRef)) / sizeof(CFStringRef);
	
	return (result == noErr) && (numberOfClasses > 0);
}

// ----------------------------------------------------------
+ (BOOL) audioUnitHasCarbonView:(AudioUnit)unit
// ----------------------------------------------------------
{
	UInt32 dataSize;
	Boolean isWriteable;
	OSStatus s = AudioUnitGetPropertyInfo(unit,
										  kAudioUnitProperty_GetUIComponentList,
										  kAudioUnitScope_Global,
										  0,
										  &dataSize,
										  &isWriteable);
	
	return (s == noErr) && (dataSize >= sizeof(ComponentDescription));
}

// ----------------------------------------------------------
- (void) audioUnitChangedViewSize:(NSNotification *)notification
// ----------------------------------------------------------
{
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSViewFrameDidChangeNotification
												  object:_AUView];
	
	NSRect newRect = self.frame;
	NSSize newSize = [self frameRectForContentRect:((NSView *)[notification object]).frame].size;
	newRect.origin.y -= newSize.height - newRect.size.height;
	newRect.size = newSize;
	[self setFrame:newRect display:YES];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(audioUnitChangedViewSize:)
												 name:NSViewFrameDidChangeNotification
											   object:_AUView];
}

@end

#pragma mark - C++

using namespace std;

// ----------------------------------------------------------
void ofxAudioUnit::showUI(const string &title, int x, int y, bool forceGeneric)
// ----------------------------------------------------------
{
	if(!_unit.get()) return;
	
	NSString * windowTitle = [[NSString stringWithUTF8String:title.c_str()] copy];
	
	if(!windowTitle) {
		windowTitle = @"Audio Unit UI";
	}
	
	AudioUnitRef au = _unit;
	
	dispatch_async(dispatch_get_main_queue(), ^{
		if(!au) return;
		
		ofxAudioUnitUIWindow * auWindow = [[ofxAudioUnitUIWindow alloc] initWithAudioUnit:*au forceGeneric:forceGeneric];
		
		if(auWindow) {
			CGFloat flippedY = [[NSScreen mainScreen] visibleFrame].size.height - y - auWindow.frame.size.height;
			[auWindow setFrameOrigin:NSMakePoint(x, flippedY)];
			[auWindow setTitle:windowTitle];
			[auWindow makeKeyAndOrderFront:nil];
		}
	});
}

#endif //TARGET_OS_IPHONE
