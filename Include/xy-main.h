/*
 * Copyright (c) 2021 Sebastian Kylander https://gaztin.com/
 *
 * This software is provided 'as-is', without any express or implied warranty. In no event will
 * the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the
 *    original software. If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as
 *    being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

//////////////////////////////////////////////////////////////////////////
/// Includes

#include "xy.h"


//////////////////////////////////////////////////////////////////////////
/// Global functions

/*
 * Main entry function
 * Define this in your application's main source file!
 */
extern int xyMain( void );


//////////////////////////////////////////////////////////////////////////
/// Platform-specific implementations

#if defined( XY_OS_WINDOWS )

#include <windows.h>
#include <clocale>

int main( int ArgC, char** ppArgV )
{
	xyContext& rContext      = xyGetContext();
	rContext.CommandLineArgs = std::span< char* >( ppArgV, ArgC );
	rContext.pPlatformImpl   = std::make_unique< xyPlatformImpl >();
	rContext.UIMode          = XY_UI_MODE_DESKTOP;

	// Store the handle to the application instance
	rContext.pPlatformImpl->ApplicationInstanceHandle = GetModuleHandle( NULL );

	std::setlocale( LC_ALL, "en_US.utf8" );

	return xyMain();

} // main

INT WINAPI WinMain( _In_ HINSTANCE Instance, _In_opt_ HINSTANCE /*PrevInstance*/, _In_ LPSTR /*CmdLine*/, _In_ int /*ShowCmd*/ )
{
	xyContext& rContext      = xyGetContext();
	rContext.CommandLineArgs = std::span< char* >( __argv, __argc );
	rContext.pPlatformImpl   = std::make_unique< xyPlatformImpl >();
	rContext.UIMode          = XY_UI_MODE_DESKTOP;

	// Store the handle to the application instance
	rContext.pPlatformImpl->ApplicationInstanceHandle = Instance;

	std::setlocale( LC_ALL, "en_US.utf8" );

	return xyMain();

} // WinMain

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

#include <locale.h>

int main( int ArgC, char** ppArgV )
{
	xyContext& rContext      = xyGetContext();
	rContext.CommandLineArgs = std::span< char* >( ppArgV, ArgC );
	rContext.UIMode          = XY_UI_MODE_DESKTOP;

	setlocale( LC_CTYPE, "UTF-8" );

	return xyMain();

} // main

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

#include <android/native_activity.h>
#include <fcntl.h>
#include <unistd.h>

[[maybe_unused]] JNIEXPORT void ANativeActivity_onCreate( ANativeActivity* pActivity, void* /*pSavedState*/, size_t /*SavedStateSize*/ )
{
	xyContext& rContext    = xyGetContext();
	rContext.pPlatformImpl = std::make_unique< xyPlatformImpl >();

	// Store the activity data
	rContext.pPlatformImpl->pNativeActivity = pActivity;

	// Obtain the configuration
	rContext.pPlatformImpl->pConfiguration = AConfiguration_new();
	AConfiguration_fromAssetManager( rContext.pPlatformImpl->pConfiguration, rContext.pPlatformImpl->pNativeActivity->assetManager );

	// Obtain the UI mode
	switch( AConfiguration_getUiModeType( rContext.pPlatformImpl->pConfiguration ) )
	{
		case ACONFIGURATION_UI_MODE_TYPE_CAR:        { rContext.UIMode = XY_UI_MODE_CAR;      } break;
		case ACONFIGURATION_UI_MODE_TYPE_TELEVISION: { rContext.UIMode = XY_UI_MODE_TV;       } break;
		case ACONFIGURATION_UI_MODE_TYPE_APPLIANCE:  { rContext.UIMode = XY_UI_MODE_HEADLESS; } break;
		case ACONFIGURATION_UI_MODE_TYPE_WATCH:      { rContext.UIMode = XY_UI_MODE_WATCH;    } break;
		case ACONFIGURATION_UI_MODE_TYPE_VR_HEADSET: { rContext.UIMode = XY_UI_MODE_VR;       } break;
		default:                                     { rContext.UIMode = XY_UI_MODE_PHONE;    } break; // Default to phone UI
	}

	// Obtain the looper for the main thread
	ALooper* pMainLooper = ALooper_forThread();
	ALooper_acquire( pMainLooper );

	// Listen for data on the main thread
	pipe2( rContext.pPlatformImpl->JavaThreadPipe, O_NONBLOCK | O_CLOEXEC );
	ALooper_addFd( pMainLooper, rContext.pPlatformImpl->JavaThreadPipe[ 0 ], 0, ALOOPER_EVENT_INPUT, []( int Read, int /*Events*/, void* /*pData*/ ) -> int
	{
		xyRunnable* pRunnable;
		if( read( Read, &pRunnable, sizeof( pRunnable ) ) == sizeof( pRunnable ) )
		{
			pRunnable->Execute();
		}

		// Keep listening
		return 1;

	}, nullptr );

	std::thread AppThread( &xyMain );
	AppThread.detach();

} // ANativeActivity_onCreate

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID

#include <thread>

#include <UIKit/UIKit.h>

@interface xyViewController : UIViewController
@end // xyViewController

@interface xyAppDelegate : NSObject< UIApplicationDelegate >
@end // xyAppDelegate

@implementation xyViewController
@end // xyViewController

@implementation xyAppDelegate

-( BOOL )application:( UIApplication* )pApplication didFinishLaunchingWithOptions:( NSDictionary* )pLaunchOptions
{
	UIScene*          pScene          = [ [ [ pApplication connectedScenes ] allObjects ] firstObject ];
	UIWindow*         pWindow         = [ [ UIWindow alloc ] initWithWindowScene:( UIWindowScene* )pScene ];
	xyViewController* pViewController = [ [ xyViewController alloc ] init ];
	pWindow.rootViewController        = pViewController;
	[ pWindow makeKeyAndVisible ];

	std::thread Thread( &xyMain );
	Thread.detach();

} // didFinishLaunchingWithOptions

@end // xyAppDelegate

int main( int ArgC, char** ppArgV )
{
	xyContext& rContext      = xyGetContext();
	rContext.CommandLineArgs = std::span< char* >( ppArgV, ArgC );
	rContext.UIMode          = XY_UI_MODE_PHONE;

	setlocale( LC_CTYPE, "UTF-8" );

	@autoreleasepool
	{
		return UIApplicationMain( ArgC, ppArgV, nil, NSStringFromClass( [ xyAppDelegate class ] ) );
	}

} // main

#elif defined( XY_OS_LINUX ) // XY_OS_IOS

#include <locale.h>

int main( int ArgC, char** ppArgV )
{
	xyContext& rContext      = xyGetContext();
	rContext.CommandLineArgs = std::span< char* >( ppArgV, ArgC );
	rContext.UIMode          = XY_UI_MODE_DESKTOP; // We assume Linux is running with a GUI, we might want to create a function to test if we are.

	std::setlocale( LC_ALL, "en_US.utf8" );

	return xyMain();
	
} // main

#else // XY_OS_LINUX

int main( int ArgC, char** ppArgV )
{
	xyContext& rContext      = xyGetContext();
	rContext.CommandLineArgs = std::span< char* >( ppArgV, ArgC );
	rContext.UIMode          = XY_UI_MODE_HEADLESS; // We don't know the UI mode. Might as well assume the worst.

	return xyMain();

} // main

#endif // !XY_OS_WINDOWS && !XY_OS_ANDROID && !XY_OS_IOS && !XY_OS_LINUX
