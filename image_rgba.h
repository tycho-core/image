//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 10:50:39 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_RGBA_H_23731CA0_6318_474B_AC2B_7D70BA47502F_
#define __IMAGE_RGBA_H_23731CA0_6318_474B_AC2B_7D70BA47502F_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/canvas.h"
#include "image/image.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{
	/// base of all rgba based formats, this takes a set of shifts and masks to access
	/// each component, if a component doesn't exist then it's mask will be 0.
    class IMAGE_ABI image_rgba : public image_base
    {
    public:
		struct pixel_layout
		{
			int rshift, rmask;
			int gshift, gmask;
			int bshift, bmask;
			int ashift, amask;				
		};
		
		static pixel_layout pixel_layout_rgba8888;
		static pixel_layout pixel_layout_rgb888;
		static pixel_layout pixel_layout_argb4444;
		static pixel_layout pixel_layout_argb1555;
		static pixel_layout pixel_layout_rgb565;
		static pixel_layout pixel_layout_bgr565;
		static pixel_layout pixel_layout_bgrx5551;
		static pixel_layout pixel_layout_bgra8888;
		static pixel_layout pixel_layout_bgrx8888;
		static pixel_layout pixel_layout_bgr888;
		static pixel_layout pixel_layout_a8;
		
    public:
		/// constructor. takes a series of shifts and masks to access the underlying colour channels
		image_rgba(int rshift, int rmask, int gshift, int gmask, int bshift, int bmask, int ashift, int amask);

		/// constructor
		image_rgba(const pixel_layout&);
		
		/// destructor
		~image_rgba();
		
		/// \name image_base interface
		//@{		
		virtual bool create_view(int width, int height, int num_mip_levels, core::uint8* pixels);
		virtual bool resize_canvas(int width, int height, int num_mip_levels, bool preserve_contents);
		virtual bool raw_copy(int mip_level, int x, int y, int width, int height, const core::uint8* src, int src_len);
		virtual int  get_width() const				{ return m_width; }			
		virtual int  get_height() const				{ return m_height; }
		virtual int  get_num_mips() const		{ return m_num_mip_levels; }			
		virtual void clear(core::rgba, int mip_level);
		virtual bool has_channel(colour_channel) const;
		virtual bool copy(int dstx, int dxty, int srcx, int srcy, int width, int height, canvas& src_canvas, canvas& dst_canvas);
		virtual bool get_mip_level(int i, canvas*);
		virtual int  get_stride() const;
		virtual math::recti get_rect(int mip_level);
		//@}
		
		/// \returns The pixel layout for this image
		const pixel_layout& get_pixel_layout() const
			{ return m_layout; }
		
		/// \returns true if the passed format is a rgba format
		static bool is_rgba_format(image_format);
		
    private:
		/// non-copyable, use clone method instead
		void operator=(const image_rgba&);
		int setup_mip_info(int width, int height, int num_mips);
		
    protected:
		static const int MaxMips = 10;
		struct mip_info
		{
			int w, h, offset;
		};
    
		pixel_layout	m_layout;
		int				m_width;			///< width of image
		int				m_height;			///< height of image
		int				m_bytes_per_pixel;	///< number of bytes per pixel
		int				m_num_mip_levels;
		core::uint8*	m_pixels;			///< raw pixels
		bool			m_pixels_owned;		///< true if we own the pixel memory s should delete it
		mip_info		m_mip_offsets[MaxMips]; ///< offsets to mip maps in pixel buffer
    };

} // end namespace

} // end namespace

#endif // __IMAGE_RGBA_H_23731CA0_6318_474B_AC2B_7D70BA47502F_
