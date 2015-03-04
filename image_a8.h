//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 10:57:16 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_A8_H_559805C3_5835_4AA9_BA72_E2BB5AF6BAFF_
#define __IMAGE_A8_H_559805C3_5835_4AA9_BA72_E2BB5AF6BAFF_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/image_rgba.h"
#include "core/memory.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{ 
namespace image
{

	/// 8 bit alpha image
    class IMAGE_ABI image_a8  : public image_rgba
    {
    public:
		/// constructor. takes a series of shifts and masks to access the underlying colour channels
		image_a8(int rshift = 0,  int rmask = 0, int gshift = 0,  int gmask = 0, 
					 int bshift = 0, int bmask = 0, int ashift = 0,  int amask = 0xff) :
			image_rgba(rshift, rmask, gshift, gmask, bshift, bmask, ashift, amask)
		{
			m_bytes_per_pixel = 1;
		}				

		/// construct from a pixel layout structure
		image_a8(const pixel_layout& pl) :
			image_rgba(pl)
		{
			m_bytes_per_pixel = 4;
		}
		
		virtual image_format get_image_format() const { return image_format_a8; }
		
    	virtual void put_pixel(core::rgba clr, int mip_level, int x, int y)
		{
			canvas c;
			if(!get_mip_level(mip_level, &c))
				return;
			core::uint8 *pixel = (core::uint8*)(c.get_pixels() + c.get_pitch() * y  + x * m_bytes_per_pixel);
			*pixel = clr.a();
		}

		virtual core::rgba get_pixel(int mip_level, int x, int y) const 
		{
			// safe to cast away const, we're not going to modify it, be nicer not to tho.
			canvas c;
			if(!const_cast<image_a8*>(this)->get_mip_level(mip_level, &c))
				return core::rgba(0,0,0,0);
			core::uint8 alpha = *(core::uint8*)(c.get_pixels() + c.get_pitch() * y  + x * m_bytes_per_pixel);
			return core::rgba(0, 0, 0, alpha);		
		}
    
    };

} // end namespace

} // end namespace

#endif // __IMAGE_A8_H_559805C3_5835_4AA9_BA72_E2BB5AF6BAFF_
