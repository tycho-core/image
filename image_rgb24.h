//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 10:56:52 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_RGB888_H_F671DDFE_B7AD_48A1_AF75_9BF735807E81_
#define __IMAGE_RGB888_H_F671DDFE_B7AD_48A1_AF75_9BF735807E81_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/image_rgba.h"
#include "core/memory.h"
#include "core/endian.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{

	/// 24 bit rgba image
    class IMAGE_ABI image_rgb24 : public image_rgba
    {
    public:
		/// constructor. takes a series of shifts and masks to access the underlying colour channels
		image_rgb24(int rshift = 16,  int rmask = 0xff, int gshift = 8,  int gmask = 0xff, 
					 int bshift = 0, int bmask = 0xff, int ashift = 0,  int amask = 0x00) :
			image_rgba(rshift, rmask, gshift, gmask, bshift, bmask, ashift, amask)
		{
			m_bytes_per_pixel = 3;
		}	
		
		image_rgb24(const pixel_layout& layout) :
			image_rgba(layout)
		{
			m_bytes_per_pixel = 3;
		}			
		
		virtual image_format get_image_format() const { return image_format_rgba24; }
		
    	virtual void put_pixel(core::rgba clr, int mip_level, int x, int y)
		{
			canvas c;
			if(!get_mip_level(mip_level, &c))
				return;
			core::uint8* p = c.get_pixels() + c.get_pitch() * y + x * m_bytes_per_pixel;
			int new_pixel = ((clr.r() & m_layout.rmask) << m_layout.rshift) |
							((clr.g() & m_layout.gmask) << m_layout.gshift) |
		                    ((clr.b() & m_layout.bmask) << m_layout.bshift) |
		                    ((255 & m_layout.amask) << m_layout.ashift);
			p[0] = (core::uint8)(new_pixel >> 16);
			p[1] = (core::uint8)(new_pixel >> 8);
			p[2] = (core::uint8)new_pixel;
		}

		virtual core::rgba get_pixel(int mip_level, int x, int y) const 
		{
			canvas c;
			// safe to cast away const, we're not going to modify it, be nicer not to tho.
			if(!const_cast<image_rgb24*>(this)->get_mip_level(mip_level, &c))
				return core::rgba(0,0,0,0);
			core::uint8* p = c.get_pixels() + c.get_pitch() * y + x * m_bytes_per_pixel;
			int pixel = (p[0] << 16) | (p[1] << 8) | p[2];
			int red   = m_layout.rmask ? (pixel >> m_layout.rshift) &  m_layout.rmask : 255;
			int green = m_layout.gmask ? (pixel >> m_layout.gshift) & m_layout.gmask : 255;
			int blue = m_layout.bmask ? (pixel >> m_layout.bshift) & m_layout.bmask : 255;
			int alpha = m_layout.amask ? (pixel >> m_layout.ashift) & m_layout.amask : 255;
			return core::rgba(red, green, blue, alpha);		
		}
    
    };

} // end namespace

} // end namespace

#endif // __IMAGE_RGB888_H_F671DDFE_B7AD_48A1_AF75_9BF735807E81_
