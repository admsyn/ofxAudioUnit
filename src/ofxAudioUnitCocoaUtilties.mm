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
- (id) initWithAudioUnit:(AudioUnit)unit forceGeneric:(BOOL)useGeneric
// ----------------------------------------------------------
{
	if(useGeneric) {
		[self initWithGenericViewForUnit:unit];
	} else if([ofxAudioUnitUIWindow audioUnitHasCocoaView:unit]) {
		[self initWithCocoaViewForUnit:unit];
	} else if([ofxAudioUnitUIWindow audioUnitHasCarbonView:unit]) {
		[self printUnsupportedCarbonMessage:unit];
	} else {
		[self initWithGenericViewForUnit:unit];
	}
	
	return self;
}

// ----------------------------------------------------------
- (void) initWithCocoaViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	UInt32  dataSize;
	Boolean isWriteable;
	AudioUnitCocoaViewInfo * cocoaViewInfo = NULL;
	UInt32  numberOfClasses;
	CFURLRef cocoaViewBundlePath = NULL;
	CFStringRef factoryClassName = NULL;
	NSView * AUView = nil;
	
	// getting the size of the AU View info
	OSStatus result = AudioUnitGetPropertyInfo(unit,
											   kAudioUnitProperty_CocoaUI,
											   kAudioUnitScope_Global,
											   0,
											   &dataSize,
											   &isWriteable);
	
	numberOfClasses = (dataSize - sizeof(CFURLRef)) / sizeof(CFStringRef);
	
	// getting the location / name of the necessary view factory bits
	if((result == noErr) && (numberOfClasses > 0))
	{
		cocoaViewInfo = (AudioUnitCocoaViewInfo *)malloc(dataSize);
		if(AudioUnitGetProperty(unit,
								kAudioUnitProperty_CocoaUI,
								kAudioUnitScope_Global,
								0,
								cocoaViewInfo,
								&dataSize) == noErr)
		{
			cocoaViewBundlePath = cocoaViewInfo->mCocoaAUViewBundleLocation;
			factoryClassName    = cocoaViewInfo->mCocoaAUViewClass[0];
		}
		else if(cocoaViewInfo != NULL)
		{
			free(cocoaViewInfo);
			cocoaViewInfo = NULL;
		}
	}
	
	// if we have everything we need, create the custom Cocoa view
	if(cocoaViewBundlePath && factoryClassName)
	{
		NSBundle * viewBundle = [NSBundle bundleWithURL:(NSURL *)cocoaViewBundlePath];
		if(viewBundle)
		{
			Class factoryClass = [viewBundle classNamed:(NSString *)factoryClassName];
			id<AUCocoaUIBase> factoryInstance = [[[factoryClass alloc] init] autorelease];
			AUView = [factoryInstance uiViewForAudioUnit:unit 
												withSize:NSMakeSize(0, 0)];
			// cleanup
			CFRelease(cocoaViewBundlePath);
			if(cocoaViewInfo)
			{
				for(int i = 0; i < numberOfClasses; i++)
					CFRelease(cocoaViewInfo->mCocoaAUViewClass[i]);
				free(cocoaViewInfo);
			}
		}
	}
	
	[self initWithAudioUnitCocoaView:AUView];
}

// ----------------------------------------------------------
- (void) initWithGenericViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	AUGenericView * AUView = [[[AUGenericView alloc] initWithAudioUnit:unit] autorelease];
	[AUView setShowsExpertParameters:YES];
	[self initWithAudioUnitCocoaView:AUView];
}

// ----------------------------------------------------------
- (void) initWithAudioUnitCocoaView:(NSView *)audioUnitView
// ----------------------------------------------------------
{
	_AUView = audioUnitView;
	NSRect contentRect = NSMakeRect(0, 0, audioUnitView.frame.size.width, audioUnitView.frame.size.height);
	self = [super initWithContentRect:contentRect
							styleMask:(NSTitledWindowMask |
									   NSClosableWindowMask |
									   NSMiniaturizableWindowMask)
							  backing:NSBackingStoreBuffered
								defer:NO];
	if(self)
	{
		self.level = NSNormalWindowLevel;
		self.contentView = audioUnitView;
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(audioUnitChangedViewSize:)
													 name:NSViewFrameDidChangeNotification
												   object:_AUView];
	}
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
	
	NSString * windowTitle = [NSString stringWithUTF8String:title.c_str()];
	
	if(!windowTitle) {
		windowTitle = @"Audio Unit UI";
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		@autoreleasepool {
			ofxAudioUnitUIWindow * auWindow = [[ofxAudioUnitUIWindow alloc] initWithAudioUnit:*_unit.get()
																				 forceGeneric:forceGeneric];
			
			CGFloat flippedY = [[NSScreen mainScreen] visibleFrame].size.height - y - auWindow.frame.size.height;
			
			[auWindow setFrameOrigin:NSMakePoint(x, flippedY)];
			[auWindow setTitle:windowTitle];
			[auWindow makeKeyAndOrderFront:nil];
		}
	});
}

#endif //TARGET_OS_IPHONE
