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

#include "../xy.h"

#if defined( XY_OS_LINUX )

 //////////////////////////////////////////////////////////////////////////
 /// Linux-specific includes

#include <vector>
#include <unordered_map>
#include <chrono>
#include <xcb/xcb.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb_icccm.h> // install libxcb-icccm4-dev
#include <string>
#include <cstring>
#include <signal.h>

//////////////////////////////////////////////////////////////////////////
/// Linux-specific data structures

class xyMessageBoxData
{
public:
	xyMessageBoxData( const char* pTitle, const char* pMessageContent, xyMessageButtons MessageButtons ) : m_pTitle( pTitle ), m_pMessageContent( pMessageContent ) { m_MessageButtons = MessageButtons; }

	// #TODO:
	~xyMessageBoxData();

	// Main Event loop
	bool WaitClose();

	void CreateFontGC();
public:

	const char* m_pTitle = "";
	const char* m_pMessageContent = "";

	float m_Width = 463;
	float m_Height = 310;

	xyMessageButtons m_MessageButtons = xyMessageButtons::Ok;

	// XCB Data

	xcb_window_t m_Window = NULL;
	xcb_screen_t* m_pScreen = nullptr;
	xcb_connection_t* m_pConnection = nullptr;
	xcb_drawable_t m_PixelMap;

	xcb_gcontext_t m_ForegroundGC;
	xcb_gcontext_t m_FillGC;
	xcb_gcontext_t m_FontGC;

	int m_VisualID = 0;

	// X11 Data

	Display* m_pDisplay = nullptr;

private:

	void DrawMessageBox();

	void TestCookie( xcb_void_cookie_t Cookie );

};

struct xyPlatformImpl
{
	void xyCreateXCBMsgBox( std::string_view Title, std::string_view Message, xyMessageButtons = xyMessageButtons::Ok );

	std::vector< xyMessageBoxData > m_MessageBoxes;

}; // xyPlatformImpl

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

xyMessageBoxData::~xyMessageBoxData()
{
	xcb_free_gc( m_pConnection, m_FontGC );
	xcb_free_gc( m_pConnection, m_ForegroundGC );
	xcb_free_gc( m_pConnection, m_FillGC );

	xcb_free_pixmap( m_pConnection, m_PixelMap );

	xcb_destroy_window( m_pConnection, m_Window );

	xcb_disconnect( m_pConnection );

	m_pDisplay = nullptr;
	m_pScreen = nullptr;
}

void xyMessageBoxData::TestCookie( xcb_void_cookie_t Cookie )
{
	xcb_generic_error_t* pError = xcb_request_check( m_pConnection, Cookie );

	// #TODO: Print and maybe not raise.
	if( pError )
		raise( SIGTRAP );
}

void xyMessageBoxData::CreateFontGC()
{
	const char* pFontName = "fixed";
	xcb_void_cookie_t Cookie;

	xcb_font_t Font = xcb_generate_id( m_pConnection );
	Cookie = xcb_open_font_checked( m_pConnection, Font, strlen( pFontName ), pFontName );

	TestCookie( Cookie );

	uint32_t Mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
	// Here we want the text to be rendered on the same color as the fill gc, with white as the text color.
	uint32_t Values[] ={ m_pScreen->white_pixel, 0x343434, Font };

	Cookie = xcb_create_gc_checked( m_pConnection, m_FontGC, m_PixelMap, Mask, Values );

	TestCookie( Cookie );

	Cookie = xcb_close_font_checked( m_pConnection, Font );

	TestCookie( Cookie );
}

void xyMessageBoxData::DrawMessageBox()
{
	xcb_void_cookie_t Cookie;

	switch( m_MessageButtons )
	{
		case xyMessageButtons::Ok:
		{
			xcb_rectangle_t Rectangles[] ={ { m_Width / 2, m_Height / 2 - 50, 73, 30 } };
			Cookie                       = xcb_poly_fill_rectangle_checked( m_pConnection, m_PixelMap, m_ForegroundGC, 1, Rectangles );

			TestCookie( Cookie );

			Cookie = xcb_image_text_8_checked( m_pConnection, strlen( "Ok" ), m_PixelMap, m_FontGC, m_Width / 2, m_Height / 2 - 50, "Ok" );

			TestCookie( Cookie );
		} break;

		case xyMessageButtons::OkCancel:
		{
			// Ive got no idea what the hell this is.
			xcb_rectangle_t Rectangles[] ={ { m_Width / 2 - 50, m_Height / 2, 73, 30 }, { m_Width / 2 - 40, m_Height / 2 - 50, 73, 30 } };
			Cookie                       = xcb_poly_fill_rectangle_checked( m_pConnection, m_PixelMap, m_ForegroundGC, 2, Rectangles );

			TestCookie( Cookie );
		} break;

		default:
			break;
	}

	// Draw message content.
	Cookie = xcb_image_text_8_checked( m_pConnection, strlen( m_pMessageContent ), m_PixelMap, m_FontGC, m_Width / 2, m_Height / 2, m_pMessageContent );

	TestCookie( Cookie );
}

bool xyMessageBoxData::WaitClose()
{
	xcb_generic_event_t* pEvent;

	while( pEvent = xcb_wait_for_event( m_pConnection ) )
	{
		switch( pEvent->response_type & ~0x80 )
		{
			case XCB_CLIENT_MESSAGE:
			{
				if( ( *( xcb_client_message_event_t* )pEvent ).data.data32[ 0 ] == ( *pDeleteWindReply ).atom )
				{
					return true;
				}
			}
			case XCB_EXPOSE:
			{
				xcb_clear_area( m_pConnection, 1, m_Window, 0, 0, m_Width, m_Height );

				DrawMessageBox();

				xcb_flush( m_pConnection );
				return false;
			}
		}

		return false;
	}
}

void xyPlatformImpl::xyCreateXCBMsgBox( std::string_view Title, std::string_view Message, xyMessageButtons MessageButtons )
{
	xyMessageBoxData MessageBox ={ Title.data(), Message.data(), MessageButtons };

	// Open Xlib display.
	MessageBox.m_pDisplay = XOpenDisplay( 0 );

	// Get connection from xlib.
	MessageBox.m_pConnection = XGetXCBConnection( MessageBox.m_pDisplay );

	// Find first screen.
	MessageBox.m_pScreen = xcb_setup_roots_iterator( xcb_get_setup( MessageBox.m_pConnection ) ).data;

	uint32_t GCMask = 0;
	uint32_t GCValues[ 2 ];

	// Create black graphic context.
	MessageBox.m_ForegroundGC = xcb_generate_id( MessageBox.m_pConnection );
	MessageBox.m_FillGC       = xcb_generate_id( MessageBox.m_pConnection ); // #TODO: We might not need this.
	MessageBox.m_FontGC       = xcb_generate_id( MessageBox.m_pConnection ); // Will be used when rendering

	// Create foreground gc.
	GCMask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	GCValues[ 0 ] = 0x2c2c2c;
	GCValues[ 1 ] = 0;

	xcb_create_gc( MessageBox.m_pConnection, MessageBox.m_ForegroundGC, MessageBox.m_pScreen->root, GCMask, GCValues );

	// Create pixel/pixmap map.

	MessageBox.m_PixelMap = xcb_generate_id( MessageBox.m_pConnection );
	xcb_create_pixmap( MessageBox.m_pConnection, MessageBox.m_pScreen->root_depth, MessageBox.m_PixelMap, MessageBox.m_pScreen->root, 500, 500 );

	MessageBox.CreateFontGC();

	// Create fill gc.
	GCMask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
	GCValues[ 0 ] = 0x343434;
	GCValues[ 1 ] = 0x343434;

	xcb_create_gc( MessageBox.m_pConnection, MessageBox.m_FillGC, MessageBox.m_pScreen->root, GCMask, GCValues );

	// Generate window ID
	MessageBox.m_Window = xcb_generate_id( MessageBox.m_pConnection );

	// xcb events -> https://xcb.freedesktop.org/tutorial/events/
	uint32_t Mask = XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK;
	uint32_t EventMask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	uint32_t ValueList[] ={ MessageBox.m_PixelMap, EventMask };

	xcb_create_window( MessageBox.m_pConnection, XCB_COPY_FROM_PARENT, MessageBox.m_Window, MessageBox.m_pScreen->root, 0, 0, MessageBox.m_Width, MessageBox.m_Height, 8, XCB_WINDOW_CLASS_INPUT_OUTPUT, MessageBox.m_VisualID, Mask, ValueList );

	// Move window to center
	int WindowPos[] ={ MessageBox.m_pScreen->width_in_pixels - MessageBox.m_Width, MessageBox.m_pScreen->height_in_pixels - MessageBox.m_Height };
	xcb_configure_window( MessageBox.m_pConnection, MessageBox.m_Window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, WindowPos );

	// Set title.
	xcb_change_property( MessageBox.m_pConnection, XCB_PROP_MODE_REPLACE, MessageBox.m_Window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen( MessageBox.m_pTitle ), MessageBox.m_pTitle );
	// Icon title.
	xcb_change_property( MessageBox.m_pConnection, XCB_PROP_MODE_REPLACE, MessageBox.m_Window, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8, strlen( MessageBox.m_pTitle ), MessageBox.m_pTitle );

	xcb_intern_atom_cookie_t ProtocolsCookie = xcb_intern_atom( MessageBox.m_pConnection, 1, 12, "WM_PROTOCOLS" );
	xcb_intern_atom_reply_t* pProtcolsReply  = xcb_intern_atom_reply( MessageBox.m_pConnection, ProtocolsCookie, 0 );

	xcb_intern_atom_cookie_t CloseWindowCookie = xcb_intern_atom( MessageBox.m_pConnection, 0, 16, "WM_DELETE_WINDOW" );
	xcb_intern_atom_reply_t* pDeleteWindReply  = xcb_intern_atom_reply( MessageBox.m_pConnection, CloseWindowCookie, 0 );

	xcb_change_property( MessageBox.m_pConnection, XCB_PROP_MODE_REPLACE, MessageBox.m_Window, ( *pProtcolsReply ).atom, 4, 32, 1, &( *pDeleteWindReply ).atom );

	xcb_map_window( MessageBox.m_pConnection, MessageBox.m_Window );

	// Great hack...
	xcb_size_hints_t SizeHints;
	SizeHints.flags      = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE;
	SizeHints.min_width  = SizeHints.max_width  = MessageBox.m_Width;
	SizeHints.min_height = SizeHints.max_height = MessageBox.m_Height;

	xcb_icccm_set_wm_normal_hints( MessageBox.m_pConnection, MessageBox.m_Window, &SizeHints );

	xcb_flush( MessageBox.m_pConnection );

	// Fill rect with black. #TODO: Fill color corresponding to theme. Or even see if the theme color is a warm/cool color and set fill accordingly.

	xcb_rectangle_t Rectangles[] ={ { 0, 0, MessageBox.m_Width, MessageBox.m_Height } };

	xcb_poly_fill_rectangle( MessageBox.m_pConnection, MessageBox.m_PixelMap, MessageBox.m_FillGC, 1, Rectangles );

	while( !MessageBox.WaitClose() )
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

	//MessageBox = { };
}
#endif

#endif // XY_OS_LINUX
