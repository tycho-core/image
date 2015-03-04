//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 11:42:40 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __CANVAS_H_BA217692_3333_44F0_9FFE_E59E18797199_
#define __CANVAS_H_BA217692_3333_44F0_9FFE_E59E18797199_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/types.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{
	class image_base;
	
	/// A canvas is the interface to the image data for a specific mip level, use \ref image_base::get_mip_level 
	/// to retrieve one for an image.
    class IMAGE_ABI canvas
    {
    public:
		/// constructor		
		canvas(image_base* owner, int width, int height, int mip_level, core::uint8* pixels, int byte_size, int pitch) :
			m_width(width),
			m_height(height),
			m_mip_level(mip_level),
            m_owner(owner),
			m_pixels(pixels),
			m_byte_size(byte_size),
			m_pitch(pitch)
		{}
		
		/// default constructor
		canvas() :
			m_width(0),
			m_height(0),
			m_mip_level(-1),
            m_owner(0),
			m_pixels(0),
			m_byte_size(0),
			m_pitch(0)
		{}
		
		/// \returns canvas width
		int get_width() const 
			{ return m_width; }
			
		/// \returns canvas height
		int get_height() const
			{ return m_height; }
			
		/// \returns row pitch in bytes
		int get_pitch() const;
		
		/// \returns raw pixels
		core::uint8* get_pixels()
			{ return m_pixels; }
		
		/// \returns raw pixels, const version
		const core::uint8* get_pixels() const 
			{ return m_pixels; }
			
		/// \returns the format of the canvas
		image_format get_format() const;
			
		/// draw a pixel to the canvas
		void put_pixel(core::rgba clr, int x, int y);
		
		/// lookup a pixel
		core::rgba get_pixel(int x, int y);
		
		/// \returns the size in bytes of the canvas
		int get_byte_size() const 
			{ return m_byte_size; }
		
    private:
		int m_width;			///< height of the canvas
		int m_height;			///< width of the canvas
		int m_mip_level;		///< mip level this canvas represents
		image_base*	m_owner;	///< image that owns this canvas
		core::uint8* m_pixels;		
		int	m_byte_size;
		int m_pitch;
    };

} // end namespace
} // end namespace

#endif // __CANVAS_H_BA217692_3333_44F0_9FFE_E59E18797199_
