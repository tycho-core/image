//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 10:56:59 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_RGBA32_H_0E2D5C02_FCE7_4176_9275_3ADE919462E6_
#define __IMAGE_RGBA32_H_0E2D5C02_FCE7_4176_9275_3ADE919462E6_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/image_rgba.h"
#include "core/endian.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{

    class IMAGE_ABI image_rgba32 : public image_rgba
    {
    public:
		/// constructor. takes a series of shifts and masks to access the underlying colour channels
		image_rgba32(int rshift = 24,  int rmask = 0xff, int gshift = 16,  int gmask = 0xff, 
					 int bshift = 8, int bmask = 0xff, int ashift = 0,  int amask = 0xff) :
			image_rgba(rshift, rmask, gshift, gmask, bshift, bmask, ashift, amask)
		{
			m_bytes_per_pixel = 4;
		}				
		
		/// construct from a pixel layout structure
		image_rgba32(const pixel_layout& pl) :
			image_rgba(pl)
		{
			m_bytes_per_pixel = 4;
		}

		virtual image_format get_image_format() const { return image_format_rgba32; }    
		
    	virtual void put_pixel(core::rgba clr, int mip_level, int x, int y)
		{
			canvas c;
			if(!get_mip_level(mip_level, &c))
				return;
			core::uint8* p = c.get_pixels() + c.get_pitch() * y + x * m_bytes_per_pixel;
			int new_pixel = ((clr.r() & m_layout.rmask) << m_layout.rshift) |
							((clr.g() & m_layout.gmask) << m_layout.gshift) |
		                    ((clr.b() & m_layout.bmask) << m_layout.bshift) |
		                    ((clr.a() & m_layout.amask) << m_layout.ashift);
			p[0] = (core::uint8)(new_pixel >> 24);
			p[1] = (core::uint8)(new_pixel >> 16);
			p[2] = (core::uint8)(new_pixel >> 8);
			p[3] = (core::uint8)(new_pixel);
		}
		
		virtual core::rgba get_pixel(int mip_level, int x, int y) const 
		{
			// safe to cast away const, we're not going to modify it, be nicer not to tho.
			canvas c;
			if(!const_cast<image_rgba32*>(this)->get_mip_level(mip_level, &c))
				return core::rgba(0,0,0,0);
			core::uint8* p = c.get_pixels() + c.get_pitch() * y + x * m_bytes_per_pixel;
			int pixel = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
			int red   = m_layout.rmask ? (pixel >> m_layout.rshift) & m_layout.rmask : 255;
			int green = m_layout.gmask ? (pixel >> m_layout.gshift) & m_layout.gmask : 255;
			int blue = m_layout.bmask ? (pixel >> m_layout.bshift) & m_layout.bmask : 255;
			int alpha = m_layout.amask ? (pixel >> m_layout.ashift) & m_layout.amask : 255;
			return core::rgba(red, green, blue, alpha);		
		}										
    };

} // end namespace

} // end namespace

#endif // __IMAGE_RGBA32_H_0E2D5C02_FCE7_4176_9275_3ADE919462E6_
