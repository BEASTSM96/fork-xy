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

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <vector>


//////////////////////////////////////////////////////////////////////////
/// Pre-processor defines

#define XY_UI_MODE_DESKTOP  0x01
#define XY_UI_MODE_PHONE    0x02
#define XY_UI_MODE_WATCH    0x04
#define XY_UI_MODE_TV       0x08
#define XY_UI_MODE_VR       0x10
#define XY_UI_MODE_CAR      0x20
#define XY_UI_MODE_HEADLESS 0x40

#if defined( _WIN32 )
/// Windows

#define XY_OS_WINDOWS
#define XY_UI_MODES ( XY_UI_MODE_DESKTOP )

#elif defined( __APPLE__ ) // _WIN32
/// Apple

#include <TargetConditionals.h>

#if TARGET_OS_OSX
	#define XY_OS_MACOS
	#define XY_UI_MODES ( XY_UI_MODE_DESKTOP )
#elif TARGET_OS_IOS // TARGET_OS_OSX
	#define XY_OS_IOS
	#define XY_UI_MODES ( XY_UI_MODE_PHONE )
#elif TARGET_OS_WATCH // TARGET_OS_IOS
	#define XY_OS_WATCHOS
	#define XY_UI_MODES ( XY_UI_MODE_WATCH )
#elif TARGET_OS_TV // TARGET_OS_WATCH
	#define XY_OS_TVOS
	#define XY_UI_MODES ( XY_UI_MODE_TV )
#endif // TARGET_OS_TV

#elif defined( __ANDROID__ ) // __APPLE__
/// Android

#define XY_OS_ANDROID
// Since there is no way to detect at compile time what UI mode we are targeting, we have to define all possible environments
// List of UI modes can be found at https://developer.android.google.cn/guide/topics/resources/providing-resources.html#UiModeQualifier
// (Appliance corresponds to Headless)
#define XY_UI_MODES ( XY_UI_MODE_PHONE | XY_UI_MODE_WATCH | XY_UI_MODE_TV | XY_UI_MODE_VR | XY_UI_MODE_CAR | XY_UI_MODE_HEADLESS )

#elif defined( __linux__ ) // __ANDROID__
/// Linux

#define XY_OS_LINUX
#define XY_UI_MODES ( XY_UI_MODE_DESKTOP | XY_UI_MODE_HEADLESS )

#endif // __linux__


//////////////////////////////////////////////////////////////////////////
/// Enumerators

enum class xyTheme
{
	Light,
	Dark,

}; // xyTheme


//////////////////////////////////////////////////////////////////////////
/// Data structures

struct xyPlatformImpl;

struct xyContext
{
	std::span< char* >                CommandLineArgs;
	std::unique_ptr< xyPlatformImpl > pPlatformImpl;
	uint32_t                          UIMode = 0x0;

}; // xyContext

struct xyRect
{
	int32_t Left   = 0;
	int32_t Top    = 0;
	int32_t Right  = 0;
	int32_t Bottom = 0;

}; // xyRect

struct xyDevice
{
	std::string Name;

}; // xyDevice

struct xyDisplayAdapter
{
	std::string Name;
	xyRect      FullRect;
	xyRect      WorkRect;

}; // xyDisplayAdapter

struct xyLanguage
{
	std::string LocaleName;

}; // xyLanguage

struct xyBatteryState
{
	operator bool( void ) const { return Valid; }

	uint8_t CapacityPercentage = 100;
	bool    Charging           = false;
	bool    Valid              = false;

}; // xyPowerStatus


//////////////////////////////////////////////////////////////////////////
/// Functions

/**
 * Obtains the internal xy context where all the global data is stored
 *
 * @return A reference to the context data.
 */
extern xyContext& xyGetContext( void );

/**
 * Convert a unicode string to UTF-8.
 *
 * @return A UTF-8 string.
 */
extern std::string xyUTF( std::wstring_view String );

/**
 * Convert a UTF-8 to Unicode.
 *
 * @return A Unicode string.
 */
extern std::wstring xyUnicode( std::string_view String );

/**
 * Prompts a system message box containing a user-defined message and an 'OK' button.
 * The current thread is blocked until the message box is closed.
 *
 * @param Title The title of the message box window.
 * @param Message The content of the message text box.
 */
extern void xyMessageBox( std::string_view Title, std::string_view Message );

/**
 * Obtains information about the current device.
 *
 * @return The device data.
 */
extern xyDevice xyGetDevice( void );

/**
 * Obtains the preferred theme of this device.
 *
 * @return The theme enumerator.
 */
extern xyTheme xyGetPreferredTheme( void );

/**
 * Obtains the system language.
 *
 * @return The language code.
 */
extern xyLanguage xyGetLanguage( void );

/**
 * Obtains the state of the battery power source on this device.
 *
 * Note: The battery state has a Valid field that is set to true if battery information was successfully obtained.
 * If it is false, that either means that the device is only using electric power, or that an error occurred.
 * Either way, the battery state provides a boolean operator that allows you to write code like this:
 * 
 * if( xyBatteryState State = xyGetBatteryState() )
 * {
 *     // Successfully obtained the battery state!
 * }
 *
 * @return The battery state.
 */
extern xyBatteryState xyGetBatteryState( void );

/**
 * Obtains the display adapters connected to the device.
 *
 * @return A vector of display adapters.
 */
extern std::vector< xyDisplayAdapter > xyGetDisplayAdapters( void );

//////////////////////////////////////////////////////////////////////////
/*

██╗███╗   ███╗██████╗ ██╗     ███████╗███╗   ███╗███████╗███╗   ██╗████████╗ █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
██║████╗ ████║██╔══██╗██║     ██╔════╝████╗ ████║██╔════╝████╗  ██║╚══██╔══╝██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██║██╔████╔██║██████╔╝██║     █████╗  ██╔████╔██║█████╗  ██╔██╗ ██║   ██║   ███████║   ██║   ██║██║   ██║██╔██╗ ██║
██║██║╚██╔╝██║██╔═══╝ ██║     ██╔══╝  ██║╚██╔╝██║██╔══╝  ██║╚██╗██║   ██║   ██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
██║██║ ╚═╝ ██║██║     ███████╗███████╗██║ ╚═╝ ██║███████╗██║ ╚████║   ██║   ██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
╚═╝╚═╝     ╚═╝╚═╝     ╚══════╝╚══════╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
*/
#if defined( XY_IMPLEMENT )

//////////////////////////////////////////////////////////////////////////
/// Includes

#include "xy-platforms/xy-android.h"
#include "xy-platforms/xy-desktop.h"
#include "xy-platforms/xy-ios.h"
#include "xy-platforms/xy-macos.h"
#include "xy-platforms/xy-tvos.h"
#include "xy-platforms/xy-watchos.h"
#include "xy-platforms/xy-windows.h"

#if defined( XY_OS_WINDOWS )
#include <windows.h>
#include <lmcons.h>
#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS
#include <android/configuration.h>
#include <android/native_activity.h>
#include <uchar.h>
#include <unistd.h>
#elif defined( XY_OS_IOS ) // XY_OS_ANDROID
#include <UIKit/UIKit.h>
#endif // XY_OS_IOS


//////////////////////////////////////////////////////////////////////////
/// Functions

xyContext& xyGetContext( void )
{
	static xyContext Context;

	return Context;

} // xyGetContext

//////////////////////////////////////////////////////////////////////////

std::string xyUTF( std::wstring_view String )
{
	std::string    UTFString;
	size_t         Size;
	const wchar_t* pSrc = String.data();
	mbstate_t      State{};

#if defined( XY_OS_WINDOWS )
	if( wcsrtombs_s( &Size, nullptr, 0, &pSrc, String.size(), &State ) == 0 )
	{
		UTFString.resize( Size );
		wcsrtombs_s( &Size, &UTFString[ 0 ], Size, &pSrc, String.size(), &State );
	}
#else // XY_OS_WINDOWS
	if( ( Size = wcsrtombs( nullptr, &pSrc, String.size(), &State ) ) > 0 )
	{
		UTFString.resize( Size );
		wcsrtombs( &UTFString[ 0 ], &pSrc, String.size(), &State );
	}
#endif // !XY_OS_WINDOWS

	return UTFString;

} // xyUTF

//////////////////////////////////////////////////////////////////////////

std::wstring xyUnicode( std::string_view String )
{
	std::wstring Result;
	size_t       Size;
	const char*  pSrc = String.data();
	mbstate_t    MultiByteState{ };

#if defined( XY_OS_WINDOWS )
	if( mbsrtowcs_s( &Size, nullptr, 0, &pSrc, String.size(), &MultiByteState ) == 0 )
	{
		Result.resize( Size );
		mbsrtowcs_s( &Size, &Result[ 0 ], Size, &pSrc, String.size(), &MultiByteState );
	}
#else // XY_OS_WINDOWS
	if( ( Size = mbsrtowcs( nullptr, &pSrc, String.size(), &MultiByteState ) ) > 0 )
	{
		Result.resize( Size );
		mbsrtowcs( &Result[ 0 ], &pSrc, String.size(), &MultiByteState );
	}
#endif // !XY_OS_WINDOWS

	return Result;

} // xyUnicode

//////////////////////////////////////////////////////////////////////////

void xyMessageBox( std::string_view Title, std::string_view Message )
{

#if defined( XY_OS_WINDOWS )

	// The ANSI version of MessageBox doesn't support UTF-8 strings so we convert it into Unicode and use the Unicode version instead
	const std::wstring UnicodeTitle   = xyUnicode( Title );
	const std::wstring UnicodeMessage = xyUnicode( Message );

	if( !UnicodeMessage.empty() ) MessageBoxW( NULL, UnicodeMessage.data(), UnicodeTitle.data(), MB_OK | MB_ICONINFORMATION | MB_TASKMODAL );
	else                          MessageBoxA( NULL, Message       .data(), Title       .data(), MB_OK | MB_ICONINFORMATION | MB_TASKMODAL );

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

	NSAlert*  pAlert           = [ [ NSAlert alloc ] init ];
	NSString* pMessageText     = [ NSString stringWithUTF8String:Title.data() ];
	NSString* pInformativeText = [ NSString stringWithUTF8String:Message.data() ];

	[ pAlert addButtonWithTitle:@"OK" ];
	[ pAlert setMessageText:pMessageText ];
	[ pAlert setInformativeText:pInformativeText ];
	[ pAlert setAlertStyle:NSAlertStyleInformational ];
	[ pAlert runModal ];

	[ pInformativeText release ];
	[ pMessageText release ];
	[ pAlert release ];

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

	jobject Alert = xyRunOnJavaThread( []( std::string Title, std::string Message )
	{
		xyContext& rContext = xyGetContext();
		JNIEnv*    pJNI     = rContext.pPlatformImpl->pNativeActivity->env;

		// Easy way to tidy up all our local references when we're done
		pJNI->PushLocalFrame( 16 );

		// Obtain all necessary classes and method IDs
		jclass    ClassBuilder           = pJNI->FindClass( "android/app/AlertDialog$Builder" );
		jmethodID CtorBuilder            = pJNI->GetMethodID( ClassBuilder, "<init>", "(Landroid/content/Context;)V" );
		jmethodID MethodSetTitle         = pJNI->GetMethodID( ClassBuilder, "setTitle", "(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;" );
		jmethodID MethodSetMessage       = pJNI->GetMethodID( ClassBuilder, "setMessage", "(Ljava/lang/CharSequence;)Landroid/app/AlertDialog$Builder;" );
		jmethodID MethodSetNeutralButton = pJNI->GetMethodID( ClassBuilder, "setNeutralButton", "(Ljava/lang/CharSequence;Landroid/content/DialogInterface$OnClickListener;)Landroid/app/AlertDialog$Builder;" );
		jmethodID MethodSetCancelable    = pJNI->GetMethodID( ClassBuilder, "setCancelable", "(Z)Landroid/app/AlertDialog$Builder;" );
		jmethodID MethodShow             = pJNI->GetMethodID( ClassBuilder, "show","()Landroid/app/AlertDialog;" );

		// Create the alert dialog
		jobject Builder = pJNI->NewObject( ClassBuilder, CtorBuilder, rContext.pPlatformImpl->pNativeActivity->clazz );
		pJNI->CallObjectMethod( Builder, MethodSetTitle, pJNI->NewStringUTF( Title.data() ) );
		pJNI->CallObjectMethod( Builder, MethodSetMessage, pJNI->NewStringUTF( Message.data() ) );
		pJNI->CallObjectMethod( Builder, MethodSetNeutralButton, pJNI->NewStringUTF( "OK" ), nullptr );
		pJNI->CallObjectMethod( Builder, MethodSetCancelable, false );
		jobject Alert = pJNI->CallObjectMethod( Builder, MethodShow );
		Alert         = pJNI->NewGlobalRef( Alert );

		// Clean up local references
		pJNI->PopLocalFrame( nullptr );

		return Alert;

	}, std::string( Title ), std::string( Message ) );

	xyContext& rContext = xyGetContext();
	JNIEnv*    pJNI;
	rContext.pPlatformImpl->pNativeActivity->vm->AttachCurrentThread( &pJNI, nullptr );

	jclass    ClassAlertDialog = pJNI->GetObjectClass( Alert );
	jmethodID MethodIsShowing  = pJNI->GetMethodID( ClassAlertDialog, "isShowing", "()Z" );

	// Sleep until dialog is closed
	while( pJNI->CallBooleanMethod( Alert, MethodIsShowing ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

	pJNI->DeleteGlobalRef( Alert );

	rContext.pPlatformImpl->pNativeActivity->vm->DetachCurrentThread();

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID

	__block bool Presented = true;

	dispatch_async_and_wait( dispatch_get_main_queue(), ^
	{
		UIApplication*     pApplication     = [ UIApplication sharedApplication ];
		UIScene*           pScene           = [ [ [ pApplication connectedScenes ] allObjects ] firstObject ];
		UIViewController*  pViewController  = [ [ [ ( UIWindowScene* )pScene windows ] firstObject ] rootViewController ];
		NSString*          pTitle           = [ NSString stringWithUTF8String:Title.data() ];
		NSString*          pMessage         = [ NSString stringWithUTF8String:Message.data() ];
		UIAlertController* pAlertController = [ UIAlertController alertControllerWithTitle:pTitle message:pMessage preferredStyle:UIAlertControllerStyleAlert ];
		UIAlertAction*     pActionOK        = [ UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^( UIAlertAction* pAction ) { Presented = false; } ];

		[ pAlertController addAction:pActionOK ];
		[ pViewController presentViewController:pAlertController animated:NO completion:^{ } ];
	} );

	// Sleep until dialog is closed
	while( Presented )
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

#endif // XY_OS_IOS

} // xyMessageBox

//////////////////////////////////////////////////////////////////////////

xyDevice xyGetDevice( void )
{

#if defined( XY_OS_WINDOWS )

	CHAR  Buffer[ CNLEN + 1 ];
	DWORD Size = static_cast< DWORD >( std::size( Buffer ) );
	if( GetComputerNameA( Buffer, &Size ) )
	{
		return { .Name={ Buffer, Size } };
	}

	return { };

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

	NSString* pName = [ [ NSHost currentHost ] name ];

	return { .Name=[ pName UTF8String ] };

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

	xyContext& rContext = xyGetContext();
	JNIEnv*    pJNI;
	rContext.pPlatformImpl->pNativeActivity->vm->AttachCurrentThread( &pJNI, nullptr );

	jclass      BuildClass           = pJNI->FindClass( "android/os/Build" );
	jfieldID    ManufacturerField    = pJNI->GetStaticFieldID( BuildClass, "MANUFACTURER", "Ljava/lang/String;" );
	jfieldID    ModelField           = pJNI->GetStaticFieldID( BuildClass, "MODEL", "Ljava/lang/String;" );
	jstring     ManufacturerName     = static_cast< jstring >( pJNI->GetStaticObjectField( BuildClass, ManufacturerField ) );
	jstring     ModelName            = static_cast< jstring >( pJNI->GetStaticObjectField( BuildClass, ModelField ) );
	const char* pManufacturerNameUTF = pJNI->GetStringUTFChars( ManufacturerName, nullptr );
	const char* pModelNameUTF        = pJNI->GetStringUTFChars( ModelName, nullptr );
	std::string DeviceName           = std::string( pManufacturerNameUTF ) + ' ' + pModelNameUTF;

	pJNI->ReleaseStringUTFChars( ModelName, pModelNameUTF );
	pJNI->ReleaseStringUTFChars( ManufacturerName, pManufacturerNameUTF );
	rContext.pPlatformImpl->pNativeActivity->vm->DetachCurrentThread();

	return { .Name=std::move( DeviceName ) };

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID

	NSString* pDeviceName = [ [ UIDevice currentDevice ] name ];

	return { .Name=[ pDeviceName UTF8String ] };

#endif // XY_OS_IOS

} // xyGetDevice

//////////////////////////////////////////////////////////////////////////

xyTheme xyGetPreferredTheme( void )
{
	// Default to light theme
	xyTheme Theme = xyTheme::Light;

#if defined( XY_OS_WINDOWS )

	DWORD AppsUseLightTheme;
	DWORD DataSize = sizeof( AppsUseLightTheme );
	if( RegGetValueA( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", "AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &AppsUseLightTheme, &DataSize ) == ERROR_SUCCESS )
		Theme = AppsUseLightTheme ? xyTheme::Light : xyTheme::Dark;

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

	NSString* pStyle = [ [ NSUserDefaults standardUserDefaults ] stringForKey:@"AppleInterfaceStyle" ];
	if( [ pStyle isEqualToString:@"Dark" ] )
		Theme = xyTheme::Dark;

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

	xyContext& rContext = xyGetContext();

	switch( AConfiguration_getUiModeNight( rContext.pPlatformImpl->pConfiguration ) )
	{
		case ACONFIGURATION_UI_MODE_NIGHT_NO:  { Theme = xyTheme::Light; } break;
		case ACONFIGURATION_UI_MODE_NIGHT_YES: { Theme = xyTheme::Dark;  } break;

		default: break;
	}

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID

	UITraitCollection* pTraitCollection = [ UITraitCollection currentTraitCollection ];

	switch( [ pTraitCollection userInterfaceStyle ] )
	{
		case UIUserInterfaceStyleLight: { Theme = xyTheme::Light; } break;
		case UIUserInterfaceStyleDark:  { Theme = xyTheme::Dark;  } break;

		default: break;
	}

#endif // XY_OS_IOS

	return Theme;

} // xyGetPreferredTheme

//////////////////////////////////////////////////////////////////////////

xyLanguage xyGetLanguage( void )
{

#if defined( XY_OS_WINDOWS )

	xyLanguage Language;
	WCHAR      Buffer[ LOCALE_NAME_MAX_LENGTH ];
	GetUserDefaultLocaleName( Buffer, static_cast< int >( std::size( Buffer ) ) );

	return { .LocaleName=xyUTF( Buffer ) };

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

	NSString* pLanguageCode = [ [ NSLocale currentLocale ] languageCode ];

	return { .LocaleName=[ pLanguageCode UTF8String ] };

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

	xyContext& rContext = xyGetContext();
	char       LanguageCode[ 2 ];
	AConfiguration_getLanguage( rContext.pPlatformImpl->pConfiguration, LanguageCode );

	return { .LocaleName=std::string( LanguageCode, 2 ) };

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID

	NSString* pLanguage = [ [ NSLocale preferredLanguages ] firstObject ];

	return { .LocaleName=[ pLanguage UTF8String ] };

#endif // XY_OS_IOS

} // xyGetLanguage

//////////////////////////////////////////////////////////////////////////

xyBatteryState xyGetBatteryState( void )
{
	xyBatteryState BatteryState;

#if defined( XY_OS_WINDOWS )

	SYSTEM_POWER_STATUS SystemPowerStatus;
	if( GetSystemPowerStatus( &SystemPowerStatus ) && SystemPowerStatus.BatteryFlag ^ 128 )
	{
		BatteryState.CapacityPercentage = SystemPowerStatus.BatteryLifePercent;
		BatteryState.Charging           = SystemPowerStatus.BatteryFlag & 8;
		BatteryState.Valid              = true;
	}

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

	CFMutableDictionaryRef Service = IOServiceMatching( "IOPMPowerSource" );
	if( io_registry_entry_t Entry = IOServiceGetMatchingService( kIOMasterPortDefault, Service ) )
	{
		CFMutableDictionaryRef Properties = nullptr;
		if( IORegistryEntryCreateCFProperties( Entry, &Properties, nullptr, 0 ) == kIOReturnSuccess )
		{
			NSDictionary* pRawProperties = ( NSDictionary* )Properties;
			bool          IsCharging     = [ [ pRawProperties objectForKey:@"IsCharging" ]      boolValue ];
			double        Capacity       = [ [ pRawProperties objectForKey:@"CurrentCapacity" ] doubleValue ];
			double        MaxCapacity    = [ [ pRawProperties objectForKey:@"MaxCapacity" ]     doubleValue ];

			BatteryState.CapacityPercentage = static_cast< uint8_t >( 100.0 * Capacity / MaxCapacity );
			BatteryState.Charging           = IsCharging;
			BatteryState.Valid              = true;
		}

		IOObjectRelease( Entry );
	}

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

	xyContext& rContext = xyGetContext();
	JNIEnv*    pEnv;
	rContext.pPlatformImpl->pNativeActivity->vm->AttachCurrentThread( &pEnv, nullptr );

	jobject   Activity            = rContext.pPlatformImpl->pNativeActivity->clazz;
	jclass    ActivityClass       = pEnv->GetObjectClass( Activity );
	jstring   BatteryService      = ( jstring )pEnv->GetStaticObjectField( ActivityClass, pEnv->GetStaticFieldID( ActivityClass, "BATTERY_SERVICE", "Ljava/lang/String;" ) );
	jobject   BatteryManager      = pEnv->CallObjectMethod( Activity, pEnv->GetMethodID( ActivityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;" ), BatteryService );
	jclass    BatteryManagerClass = pEnv->GetObjectClass( BatteryManager );
	jint      PropertyCapacity    = pEnv->GetStaticIntField( BatteryManagerClass, pEnv->GetStaticFieldID( BatteryManagerClass, "BATTERY_PROPERTY_CAPACITY", "I" ) );
	jint      PropertyStatus      = pEnv->GetStaticIntField( BatteryManagerClass, pEnv->GetStaticFieldID( BatteryManagerClass, "BATTERY_PROPERTY_STATUS",   "I" ) );
	jint      StatusCharging      = pEnv->GetStaticIntField( BatteryManagerClass, pEnv->GetStaticFieldID( BatteryManagerClass, "BATTERY_STATUS_CHARGING",   "I" ) );
	jmethodID GetIntProperty      = pEnv->GetMethodID( BatteryManagerClass, "getIntProperty", "(I)I" );
	jint      Capacity            = pEnv->CallIntMethod( BatteryManager, GetIntProperty, PropertyCapacity );
	jint      Status              = pEnv->CallIntMethod( BatteryManager, GetIntProperty, PropertyStatus );

	BatteryState.CapacityPercentage = static_cast< uint8_t >( Capacity );
	BatteryState.Charging           = Status == StatusCharging;
	BatteryState.Valid              = true;

	rContext.pPlatformImpl->pNativeActivity->vm->DetachCurrentThread();

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID
	
	UIDevice* pDevice = [ UIDevice currentDevice ];
	BatteryState.CapacityPercentage = static_cast< uint8_t >( 100.0 * [ pDevice batteryLevel ] );
	BatteryState.Charging           = [ pDevice batteryState ] == UIDeviceBatteryStateCharging;
	BatteryState.Valid              = true;

#endif // XY_OS_IOS

	return BatteryState;

} // xyGetBatteryState

//////////////////////////////////////////////////////////////////////////

std::vector< xyDisplayAdapter > xyGetDisplayAdapters( void )
{
	std::vector< xyDisplayAdapter > DisplayAdapters;

#if defined( XY_OS_WINDOWS )

	auto EnumProc = []( HMONITOR MonitorHandle, HDC /*DeviceContextHandle*/, LPRECT /*pRect*/, LPARAM UserData ) -> BOOL
	{
		auto& rMonitors = *reinterpret_cast< std::vector< xyDisplayAdapter >* >( UserData );

		MONITORINFOEXA Info = { sizeof( MONITORINFOEXA ) };
		if( GetMonitorInfoA( MonitorHandle, &Info ) )
		{
			xyDisplayAdapter Adapter = { .Name     = Info.szDevice,
			                             .FullRect = { .Left=Info.rcMonitor.left, .Top=Info.rcMonitor.top, .Right=Info.rcMonitor.right, .Bottom=Info.rcMonitor.bottom },
			                             .WorkRect = { .Left=Info.rcWork   .left, .Top=Info.rcWork   .top, .Right=Info.rcWork   .right, .Bottom=Info.rcWork   .bottom } };

			DISPLAY_DEVICEA DisplayDevice = { .cb=sizeof( DISPLAY_DEVICEA ) };
			if( EnumDisplayDevicesA( Adapter.Name.c_str(), 0, &DisplayDevice, 0 ) )
				Adapter.Name = DisplayDevice.DeviceString;

			rMonitors.emplace_back( std::move( Adapter ) );
		}

		// Always continue
		return TRUE;
	};

	EnumDisplayMonitors( NULL, NULL, EnumProc, reinterpret_cast< LPARAM >( &DisplayAdapters ) );

#elif defined( XY_OS_MACOS ) // XY_OS_WINDOWS

	for( NSScreen* pScreen in [ NSScreen screens ] )
	{
		xyDisplayAdapter Adapter = { .Name     = [ [ pScreen localizedName ] UTF8String ],
		                             .FullRect = { .Left=NSMinX( pScreen.frame ),        .Top=NSMinY( pScreen.frame ),        .Right=NSMaxX( pScreen.frame ),        .Bottom=NSMaxY( pScreen.frame ) },
		                             .WorkRect = { .Left=NSMinX( pScreen.visibleFrame ), .Top=NSMinY( pScreen.visibleFrame ), .Right=NSMaxX( pScreen.visibleFrame ), .Bottom=NSMaxY( pScreen.visibleFrame ) } };

		DisplayAdapters.emplace_back( std::move( Adapter ) );
	}

#elif defined( XY_OS_ANDROID ) // XY_OS_MACOS

	xyContext& rContext = xyGetContext();
	JNIEnv*    pJNI;
	rContext.pPlatformImpl->pNativeActivity->vm->AttachCurrentThread( &pJNI, nullptr );

	jobject      Activity       = rContext.pPlatformImpl->pNativeActivity->clazz;
	jclass       ActivityClass  = pJNI->GetObjectClass( Activity );
	jstring      DisplayService = ( jstring )pJNI->GetStaticObjectField( ActivityClass, pJNI->GetStaticFieldID( ActivityClass, "DISPLAY_SERVICE", "Ljava/lang/String;" ) );
	jobject      DisplayManager = pJNI->CallObjectMethod( Activity, pJNI->GetMethodID( ActivityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;" ), DisplayService );
	jobjectArray Displays       = ( jobjectArray )pJNI->CallObjectMethod( DisplayManager, pJNI->GetMethodID( pJNI->GetObjectClass( DisplayManager ), "getDisplays", "()[Landroid/view/Display;" ) );
	jsize        DisplayCount   = pJNI->GetArrayLength( Displays );

	for( jsize i = 0; i < DisplayCount; ++i )
	{
		jobject     Display      = pJNI->GetObjectArrayElement( Displays, i );
		jclass      DisplayClass = pJNI->GetObjectClass( Display );
		jstring     Name         = ( jstring )pJNI->CallObjectMethod( Display, pJNI->GetMethodID( DisplayClass, "getName", "()Ljava/lang/String;" ) );
		const char* pNameUTF     = pJNI->GetStringUTFChars( Name, nullptr );
		jclass      RectClass    = pJNI->FindClass( "android/graphics/Rect" );
		jobject     Bounds       = pJNI->AllocObject( RectClass );

		xyDisplayAdapter Adapter = { .Name=pNameUTF };

		// NOTE: "getRectSize" is deprecated as of SDK v30.
		// The documentation suggests using WindowMetric#getBounds(), but there seems to be no way of obtaining the bounds of a specific Display object.
		pJNI->CallVoidMethod( Display, pJNI->GetMethodID( DisplayClass, "getRectSize", "(Landroid/graphics/Rect;)V" ), Bounds );
		Adapter.FullRect.Left   = pJNI->GetIntField( Bounds, pJNI->GetFieldID( RectClass, "left",   "I" ) );
		Adapter.FullRect.Top    = pJNI->GetIntField( Bounds, pJNI->GetFieldID( RectClass, "top",    "I" ) );
		Adapter.FullRect.Right  = pJNI->GetIntField( Bounds, pJNI->GetFieldID( RectClass, "right",  "I" ) );
		Adapter.FullRect.Bottom = pJNI->GetIntField( Bounds, pJNI->GetFieldID( RectClass, "bottom", "I" ) );

		if( jobject DisplayCutout = pJNI->CallObjectMethod( Display, pJNI->GetMethodID( DisplayClass, "getCutout", "()Landroid/view/DisplayCutout;" ) ) )
		{
			jclass DisplayCutoutClass = pJNI->GetObjectClass( DisplayCutout );

			Adapter.WorkRect.Left   = pJNI->CallIntMethod( DisplayCutout, pJNI->GetMethodID( DisplayCutoutClass, "getSafeInsetLeft",   "()I" ) );
			Adapter.WorkRect.Top    = pJNI->CallIntMethod( DisplayCutout, pJNI->GetMethodID( DisplayCutoutClass, "getSafeInsetTop",    "()I" ) );
			Adapter.WorkRect.Right  = pJNI->CallIntMethod( DisplayCutout, pJNI->GetMethodID( DisplayCutoutClass, "getSafeInsetRight",  "()I" ) );
			Adapter.WorkRect.Bottom = pJNI->CallIntMethod( DisplayCutout, pJNI->GetMethodID( DisplayCutoutClass, "getSafeInsetBottom", "()I" ) );
		}
		else
		{
			// TODO: There are other ways to obtain the safe area
			Adapter.WorkRect = Adapter.FullRect;
		}

		DisplayAdapters.emplace_back( std::move( Adapter ) );

		pJNI->ReleaseStringUTFChars( Name, pNameUTF );
	}

	rContext.pPlatformImpl->pNativeActivity->vm->DetachCurrentThread();

#elif defined( XY_OS_IOS ) // XY_OS_ANDROID

	for( UIScreen* pScreen in [ UIScreen screens ] )
	{
		NSString*        pScreenName = [ pScreen debugDescription ];
		CGRect           Bounds      = [ pScreen bounds ];
		xyDisplayAdapter MainDisplay = { .Name     = [ pScreenName UTF8String ],
		                                 .FullRect = { .Left=CGRectGetMinX( Bounds ), .Top=CGRectGetMinY( Bounds ), .Right=CGRectGetMaxX( Bounds ), .Bottom=CGRectGetMaxY( Bounds ) } };

		// TODO: Obtain the safe area

		DisplayAdapters.emplace_back( std::move( MainDisplay ) );
	}

#endif // XY_OS_IOS

	return DisplayAdapters;

} // xyGetDisplayAdapters


#endif // XY_IMPLEMENT
