//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 28 April 2008 3:27:01 AM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_FUNCTIONS_H_239AF7ED_BF48_494E_86BE_433C02915A2B_
#define __IMAGE_FUNCTIONS_H_239AF7ED_BF48_494E_86BE_433C02915A2B_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/forward_decls.h"
#include "math/rect.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{

	enum filter_type
	{
		filter_type_invalid = 0,
		filter_type_box,
		filter_type_gaussian
	};
	
	/// Shrink the image to the nearest lower power of 2 boundary in width and height
	/// if not already a power of 2.
	IMAGE_ABI bool shrink_to_pow2(image_base_ptr, filter_type);
	
	/// Enlarge the image to the next highest power of 2 boundary in width and height
	/// if not already a power of 2.
	IMAGE_ABI bool enlarge_to_pow2(image_base_ptr, filter_type);

	/// Copy entire image to another
	IMAGE_ABI bool copy(image_base_ptr src, image_base_ptr dst);
		
	/// Copy from one canvas to another, this will convert the pixel format if necessary.
	IMAGE_ABI bool copy(canvas& src_canvas, canvas& dst_canvas, const math::recti& src_rect, const math::vector2i& dst_pos);
	
	/// Resize the image from one canvas to another, this will convert the pixel format if necessary.
	IMAGE_ABI bool resize(canvas& src_canvas, canvas& dst_canvas, const math::recti& src_rect, const math::recti& dst_rect, filter_type);
	
	/// gamma correct the image, this corrects all the canvas in the mip chain. Ideally
	/// images should have mip maps generated in linear space then re gamma'd to avoid 
	/// accuracy loss.
	IMAGE_ABI bool gamma_correct(image_base_ptr);
	
	/// build the mip chain for the image
	/// \param min_mip_width minimum width to create mip levels to
	/// \param min_mip_height minimum height to create mip levels to
	IMAGE_ABI bool build_mip_chain(image_base_ptr, int min_mip_width, int min_mip_height);


	// experimental
	IMAGE_ABI bool emboss(canvas& src_canvas, canvas& dst_canvas, const math::recti& src_rect, const math::recti& dst_rect);
	IMAGE_ABI bool edge_detect(canvas& src_canvas, canvas& dst_canvas, const math::recti& src_rect, const math::recti& dst_rect);

} // end namespace
} // end namespace

#endif // __IMAGE_FUNCTIONS_H_239AF7ED_BF48_494E_86BE_433C02915A2B_
