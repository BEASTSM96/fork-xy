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

//////////////////////////////////////////////////////////////////////////
/// Linux-specific data structures

struct xyMessageBoxData
{
	xyMessageBoxData( const char* pTitle, const char* pMessageContent, xyMessageButtons MessageButtons ) : m_pTitle( pTitle ), m_pMessageContent( pMessageContent ) { m_MessageButtons = MessageButtons; }

	// #TODO:
	~xyMessageBoxData() {}

	void DrawMessageBox();

	// Main Event loop
	bool WaitClose();

	const char* m_pTitle = "";
	const char* m_pMessageContent = "";

	float m_Width, m_Height = 150;

	xyMessageButtons m_MessageButtons = xyMessageButtons::Ok;

	// XCB Data

	xcb_window_t m_Window = NULL;
	xcb_screen_t* m_pScreen = nullptr;
	xcb_connection_t* m_pConnection = nullptr;
	int m_VisualID = 0;

	// X11 Data

	Display* m_pDisplay = nullptr;
};

struct xyPlatformImpl
{
	void xyCreateXCBMsgBox( std::string_view Title, std::string_view Message );

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

bool xyMessageBoxData::WaitClose()
{

}

void xyPlatformImpl::xyCreateXCBMsgBox( std::string_view Title, std::string_view Message )
{
	xyMessageBoxData MessageBox = { Title.data(), Message.data(), xyMessageButtons::Ok };

	// Open Xlib display.
	MessageBox.m_pDisplay = XOpenDisplay( 0 );

	// Get connection from xlib.
	MessageBox.m_pConnection = XGetXCBConnection( MessageBox.m_pDisplay );

	// Find first screen.
	MessageBox.m_pScreen = xcb_setup_roots_iterator( xcb_get_setup( MessageBox.m_pConnection ) ).data;

	MessageBox.m_Window = xcb_generate_id( MessageBox.m_pConnection );

	// xcb events -> https://xcb.freedesktop.org/tutorial/events/
	uint32_t EventMask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	uint32_t ValueList[] = { EventMask };

	int X = MessageBox.m_pScreen->width_in_pixels - m_Width;
	int Y = MessageBox.m_pScreen->height_in_pixels - m_Height;

	xcb_create_window( MessageBox.m_pConnection, XCB_COPY_FROM_PARENT, MessageBox.m_Window, MessageBox.m_pScreen->root, X, Y, m_Width, m_Height, 8, XCB_WINDOW_CLASS_INPUT_OUTPUT, MessageBox.m_VisualID, 0, ValueList );

	// Set title.
	xcb_change_property( MessageBox.m_pConnection, XCB_PROP_MODE_REPLACE, MessageBox.m_Window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen( MessageBox.m_pTitle ), MessageBox.m_pTitle );
	// Icon title.
	xcb_change_property( MessageBox.m_pConnection, XCB_PROP_MODE_REPLACE, MessageBox.m_Window, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8, strlen( MessageBox.m_pTitle ), MessageBox.m_pTitle );

	xcb_map_window( MessageBox.m_pConnection, MessageBox.m_Window );

	xcb_flush( MessageBox.m_pConnection );

	while( !MessageBox.WaitClose() )
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
}
#endif

#endif // XY_OS_LINUX
