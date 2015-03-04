//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 10:50:40 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image_rgba.h"
#include "core/memory.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace image
{

	image_rgba::pixel_layout image_rgba::pixel_layout_rgba8888 = { 24, 0xff, 16, 0xff, 8, 0xff, 0, 0xff } ;
	image_rgba::pixel_layout image_rgba::pixel_layout_bgra8888 = { 8, 0xff, 16, 0xff, 24, 0xff, 0, 0xff } ;
	image_rgba::pixel_layout image_rgba::pixel_layout_bgrx8888 = { 8, 0xff, 16, 0xff, 24, 0xff, 0, 0 } ;
	image_rgba::pixel_layout image_rgba::pixel_layout_rgb888   = { 16, 0xff, 8, 0xff, 0, 0xff, 0, 0 };
	image_rgba::pixel_layout image_rgba::pixel_layout_bgr888   = { 0, 0xff, 8, 0xff, 16, 0xff, 0, 0 };
	image_rgba::pixel_layout image_rgba::pixel_layout_argb4444 = { 8, 0xf, 4, 0xf, 0, 0xf, 12, 0xf};
	image_rgba::pixel_layout image_rgba::pixel_layout_argb1555   = { 10, 0x1f, 5, 0x1f, 0, 0x1f, 15, 0x1};
	image_rgba::pixel_layout image_rgba::pixel_layout_rgb565     = { 11, 0x1f, 5, 0x1f, 0, 0x1f, 0, 0};
	image_rgba::pixel_layout image_rgba::pixel_layout_bgr565     = {  0, 0x1f, 5, 0x1f, 11, 0x1f, 0, 0};
	image_rgba::pixel_layout image_rgba::pixel_layout_bgrx5551   = {  0, 0x1f, 5, 0x1f, 11, 0x1f, 0, 0};
	image_rgba::pixel_layout image_rgba::pixel_layout_a8   = {  0, 0, 0, 0, 0, 0, 0, 0xff};
	
	/// constructor. takes a series of shifts and masks to access the underlying colour channels
	image_rgba::image_rgba(int rshift, int rmask, int gshift, int gmask, int bshift, int bmask, int ashift, int amask) :
		m_width(0),
		m_height(0),
		m_bytes_per_pixel(0),
		m_num_mip_levels(0),
		m_pixels(0),
		m_pixels_owned(false)
	{
		core::mem_zero(m_mip_offsets);
		m_layout.rshift = rshift;
		m_layout.rmask = rmask;
		m_layout.gshift = gshift;
		m_layout.gmask = gmask;
		m_layout.bshift = bshift;
		m_layout.bmask = bmask;
		m_layout.ashift = ashift;
		m_layout.amask = amask;
	}

	/// construct from a pixel layout structure
	image_rgba::image_rgba(const pixel_layout& pl) :
		m_layout(pl),
		m_width(0),
		m_height(0),
		m_bytes_per_pixel(0),
		m_num_mip_levels(0),
		m_pixels(0),
		m_pixels_owned(false)
	{
	}
	
	/// destructor
	image_rgba::~image_rgba()
	{
		if(m_pixels_owned)
			core::safe_delete_array(m_pixels);
		else
			m_pixels = 0;
	}
	
	
	int image_rgba::setup_mip_info(int width, int height, int num_mip_levels)
	{
		int total_size = 0;
		{
			mip_info& mip = m_mip_offsets[0];
			mip.offset = 0;
			mip.w = width;
			mip.h = height;
			total_size += width * height * m_bytes_per_pixel;
			int w = width >> 1, h = height >> 1;
			int m = 1;
			for(; w > 4 && h > 4 && m < num_mip_levels; w >>= 1, h >>= 1, ++m)
			{
				mip_info& mip = m_mip_offsets[m];
				mip.offset = total_size;
				mip.w = w;
				mip.h = h;				
				total_size += w * h * m_bytes_per_pixel;
			}
		}
		return total_size;
	}
	
	bool image_rgba::resize_canvas(int width, int height, int num_mip_levels, bool preserve_contents)
	{ 
		if(num_mip_levels >= MaxMips)
			return false;
		
		// calculate total size including mip chain and setup mip offsets	
		int total_size = setup_mip_info(width, height, num_mip_levels);
		core::uint8* new_pixels = new core::uint8[total_size];
		if(preserve_contents && m_pixels)
		{
			//copy(0, 0, 0, 0, m_width, m_height, m_canvas, new_canvas);
		}
		if(m_pixels_owned)
			delete[] m_pixels;
		m_pixels = new_pixels;
		m_pixels_owned = true;
		m_width = width;
		m_height = height;
		m_num_mip_levels = num_mip_levels;
		return true;
	}

	bool image_rgba::create_view(int width, int height, int num_mip_levels, core::uint8* pixels)
	{
		if(num_mip_levels >= MaxMips)
			return false;
		if(m_pixels_owned)
			delete[] m_pixels;
		setup_mip_info(width, height, num_mip_levels);
		m_pixels = pixels;
		m_pixels_owned = false;
		m_width = width;
		m_height = height;
		m_num_mip_levels = num_mip_levels;
		return true;
	}
	
	bool image_rgba::raw_copy(int mip_level, int x, int y, int width, int height, const core::uint8* src, int src_len)
	{
		// validate parameters
		if(x < 0 || y < 0 || !src || !src_len || mip_level >= m_num_mip_levels)
			return false;
		
		// crop to target canvas size
		canvas c;
		if(!get_mip_level(mip_level, &c))
			return false;
		if(x > c.get_width() || y > c.get_height())
			return false;		
		if(x + width > c.get_width())
			width = c.get_width() - x;
		if(y + height > c.get_height())
			height = c.get_height() - y;
			
		// copy a line at a time into place		
		int dst_stride = c.get_width() * m_bytes_per_pixel;
		core::uint8* dst_ptr = c.get_pixels() + dst_stride * y + x * m_bytes_per_pixel;
		int src_stride = width * m_bytes_per_pixel;
		const core::uint8* src_ptr = src;
		for(int y = 0; y < height; ++y)
		{
			core::mem_cpy(dst_ptr, src_ptr, src_stride);
			dst_ptr += dst_stride;
			src_ptr += src_stride;
		}
			
		return true;			
	}
	
	void image_rgba::clear(core::rgba clr, int mip_level)
	{
		for(int y = 0; y < get_height(); ++y)
		{
			for(int x = 0; x < get_width(); ++x)
			{
				put_pixel(clr, mip_level, x, y);
			}
		}
	}

	bool image_rgba::has_channel(colour_channel c) const
	{
		switch(c)
		{
			case colour_channel_red	: return m_layout.rmask != 0;
			case colour_channel_green : return m_layout.gmask != 0;
			case colour_channel_blue  : return m_layout.bmask != 0;
			case colour_channel_alpha : return m_layout.amask != 0;
		}
		return false;
	}


	/// copy from one rgba surface to another
	bool image_rgba::copy(int dstx, int dsty, int srcx, int srcy, int width, int height, canvas& src_canvas, canvas& dst_canvas)
	{
		// validate parameters
		if(!is_rgba_format(src_canvas.get_format()) ||
		   !is_rgba_format(dst_canvas.get_format()))
		{
			return false;
		}
		
		// allow 0 width and height as a no op
		if(width == 0 || height == 0)
			return true;
			
		// crop to fit to destination
		if(dstx >= dst_canvas.get_width() ||
		   dsty >= dst_canvas.get_height())
		{
			return true;
		}		
		if((dstx + width) > dst_canvas.get_width())
			width = dst_canvas.get_width() - dstx;
		if((dsty + height) > dst_canvas.get_height())
			height = dst_canvas.get_height() - dsty;
		 
		// super slow 
		for(int sy = srcy, dy = dsty; sy < (srcy + height); ++sy, ++dy)
		{
			for(int sx = srcx, dx = dstx; sx < (srcx + width); ++sx, ++dx)
			{
				core::rgba pixel = src_canvas.get_pixel(sx, sy);
				dst_canvas.put_pixel(pixel, dx, dy);
			}
		}
		return true;
	}
	
	bool image_rgba::get_mip_level(int i, canvas* out_canvas)
	{
		if(i >= m_num_mip_levels || !out_canvas)
			return false;
			
		const mip_info& mip = m_mip_offsets[i];
		*out_canvas = canvas(this, mip.w, mip.h, i, &m_pixels[mip.offset], m_bytes_per_pixel * mip.w * mip.h, m_bytes_per_pixel * mip.w);
		return true;
	}

	int image_rgba::get_stride() const
	{
		return get_width() * m_bytes_per_pixel;
	}
	
	
	math::recti image_rgba::get_rect(int mip_level)
	{
		TYCHO_ASSERT(mip_level < m_num_mip_levels);
		return math::recti(0, 0, m_mip_offsets[mip_level].w, m_mip_offsets[mip_level].h);
	}
	
	/// \returns true if the passed format is a rgba format
	bool image_rgba::is_rgba_format(image_format fmt)
	{
		switch(fmt)
		{
			case image_format_rgba32 :
			case image_format_rgba24 :
			case image_format_rgba16 :
			case image_format_a8 : return true;
                
            default: TYCHO_NOT_IMPLEMENTED; break;
		}		
		return false;
	}
	
} // end namespace
} // end namespace
