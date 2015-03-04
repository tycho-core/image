//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 9:56:20 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_H_D21CAE2C_2718_4A28_A787_41F24DD346BC_
#define __IMAGE_H_D21CAE2C_2718_4A28_A787_41F24DD346BC_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/forward_decls.h"
#include "image/types.h"
#include "math/rect.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{
	
	/// base of all image types
    class IMAGE_ABI image_base
    {
    public:
		/// virtual destructor
		virtual ~image_base() {}
		
		/// resize the image canvas
		/// \param width	New width
		/// \param height	New height
		/// \param preserve_contents If true existing contents are preserved on the new canvas, no rescaling of it occurs.
		virtual bool resize_canvas(int width, int height, int num_mip_levels, bool preserve_contents) = 0;

		/// creates a view over an external pixel buffer, for instance, this is useful to avoid creating a temporary surface
		/// when converting pixels into texture memory.
		virtual bool create_view(int width, int height, int num_mip_levels, core::uint8* pixels) = 0;
		
		/// set the contents of the image from a raw bunch of bytes, caller must be sure they use the correct format
		/// as no conversion takes place. It will be cropped if the target canvas is smaller than the source.
		///
		/// \param x origin of destination
		/// \param y origin of destination
		/// \param mip_level mip level to write to
		/// \param width width of source data
		/// \param height height of source data
		/// \param src source data, raw bytes
		/// \param src_len length of the source data buffer so bounds checks can be done internally.
		virtual bool raw_copy(int mip_level, int x, int y, int width, int height, const core::uint8* src, int src_len) = 0;
		
		/// \returns the width of the image at its lowest mip level
		virtual int get_width() const = 0;

		/// \returns the height of the image at its lowest mip level
		virtual int get_height() const = 0;
		
		/// \returns the stride of the image (width in bytes + any padding)
		virtual int get_stride() const = 0;
		
		/// \returns the number of mip levels in the image
		virtual int get_num_mips() const = 0;

		/// clear the image to a constant colour
		virtual void clear(core::rgba, int mip_level) = 0;
		
		/// \returns the image for the i'th mip level
		virtual bool get_mip_level(int i, canvas*) = 0;
		
		/// \returns the type of the image
		virtual image_format get_image_format() const = 0;								
		
		/// \returns pixel 
		virtual core::rgba get_pixel(int mip_level, int x, int y) const = 0;

		/// draw a pixel to the mip level
		virtual void put_pixel(core::rgba, int mip_level, int x, int y) = 0;
		
		/// \returns true if the image contains the specified channel
		virtual bool has_channel(colour_channel) const = 0;

		/// copy from one canvas to another
		virtual bool copy(int dstx, int dxty, int srcx, int srcy, int width, int height, canvas& src_canvas, canvas& dst_canvas) = 0;
		
		/// \returns A rect structure describing the mip level
		virtual math::recti get_rect(int mip_level) = 0;
    };

} // end namespace

} // end namespace

#endif // __IMAGE_H_D21CAE2C_2718_4A28_A787_41F24DD346BC_
