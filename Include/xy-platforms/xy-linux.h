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
#include <xcb/xcb.h>

//////////////////////////////////////////////////////////////////////////
/// Linux-specific data structures

struct xyPlatformImpl
{
	int xyCreateXCBMsgBox( std::string_view Title, std::string_view Message );

	xyXCBData m_XCBData = xyXCBData();

}; // xyPlatformImpl

struct xyXCBData
{
	xyXCBData();
	~xyXCBData();

	xcb_screen_t* FindFirstScreen();

	xcb_connection_t* m_pConnection;

	std::unordered_map< const char*, xcb_window_t /* uint32_t */ > m_Windows;
};

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

xyXCBData::xyXCBData()
{
	// Open a connection to the X server.
	m_pConnection = xcb_connect( NULL, NULL );
}

xyXCBData::~xyXCBData()
{
	xcb_disconnect( m_pConnection );
}

xcb_screen_t* xyXCBData::FindFirstScreen()
{
	const xcb_setup_t* pSetup = xcb_get_setup( pConnection );
	xcb_screen_iterator_t It = xcb_setup_roots_iterator( pSetup );

	return It.data;
}

int xyPlatformImpl::xyCreateXCBMsgBox( std::string_view Title, std::string_view Message )
{
	// TODO: Right now we always create the window on the first screen, when we have GUI added we may want it to be on the same screen as the window.

	// Find first screen.
	std::unique_ptr< xcb_screen_t > pFirstScreen;
	pFirstScreen = std::make_unique< xcb_screen_t >( m_XCBData.FindFirstScreen() );

	// Create the window.
	m_XCBData.m_Windows.insert( { Title.data(), xcb_generate_id( m_XCBData.m_pConnection ) } );

	xcb_create_window( m_XCBData.m_pConnection,
				   XCB_COPY_FROM_PARENT,
				   m_XCBData.m_Windows[ Title.data() ],
				   pFirstScreen->root,
				   0, 0,
				   150, 150,
				   8,
				   XCB_WINDOW_CLASS_INPUT_OUTPUT
				   pFirstScreen->root_visual,
				   0, NULL );

	// Map window to the screen.
	xcb_map_window( m_XCBData.m_pConnection, m_XCBData.m_Windows[ Title.data() ] );
}
#endif

#endif // XY_OS_LINUX
