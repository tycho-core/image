//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 28 April 2008 3:27:02 AM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image_functions.h"
#include "math/utilities.h"
#include "canvas.h"
#include "image.h"
#include "image_rgb24.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace image
{
	using namespace tycho::math;
	
	template<class T, int Size>
	struct kernel
	{
		typedef T base_type;
		static const int kernel_size = Size;
		T value[kernel_size][kernel_size];
	};	
	typedef kernel<float, 2> kernel2f;
	typedef kernel<float, 3> kernel3f;
	
	template<class T>
	void apply_kernel(canvas& src, canvas& dst, const T& k)
	{
		float scale_x = (float)src.get_width() / dst.get_width();
		float scale_y = (float)src.get_height() / dst.get_height();
		
		// for each pixel in the destination image
		for(int y = 0; y < dst.get_height(); ++y)
		{
			for(int x = 0; x < dst.get_width(); ++x)
			{
				float src_center_x = (float)(x + 0.5f) * scale_x;
				float src_center_y = (float)(y + 0.5f) * scale_y;
				
				// convolve
				float sumr = 0;
				float sumg = 0;
				float sumb = 0;
				float suma = 0;
				int ks = (T::kernel_size-1)/2;
				for(int ky = 0, ty = -ks; ky < T::kernel_size; ++ky, ++ty)
				{
					for(int kx = 0, tx = -ks; kx < T::kernel_size; ++kx, ++tx)
					{
						int sx = static_cast<int>(math::floor(src_center_x + tx));
						int sy = static_cast<int>(math::floor(src_center_y + ty));
						
						if(sx >= 0 && sx < src.get_width() && 
						   sy >= 0 && sy < src.get_height())
						{
							const core::rgba& clr = src.get_pixel(sx, sy);
							const typename T::base_type& kv = k.value[kx][ky];
							sumr += kv * clr.r();
							sumg += kv * clr.g();
							sumb += kv * clr.b();
							suma += kv * clr.a();
						}
					}
				}
				dst.put_pixel(core::rgba((int)math::clamp(sumr, 0.0f, 255.0f),
										 (int)math::clamp(sumg, 0.0f, 255.0f),
										 (int)math::clamp(sumb, 0.0f, 255.0f),
										 (int)math::clamp(suma, 0.0f, 255.0f)), x, y);
			}
		}
	}

	/// Shrink the image to the nearest lower power of 2 boundary in width and height
	IMAGE_ABI bool shrink_to_pow2(image_base_ptr , filter_type)
	{
		return false;
	}
	
	/// Enlarge the image to the next highest power of 2 boundary in width and height
	/// if not already a power of 2.
	IMAGE_ABI bool enlarge_to_pow2(image_base_ptr, filter_type)
	{
		return false;
	}

	/// Copy entire image to another
	IMAGE_ABI bool copy(image_base_ptr src, image_base_ptr dst)
	{
		canvas src_c, dst_c;
		if(!src->get_mip_level(0, &src_c) ||
		   !dst->get_mip_level(0, &dst_c))
		{
			return false; 
		}
		
		return copy(src_c, dst_c, src->get_rect(0), vector2i(0,0));
	}	
	
	/// Copy from one canvas to another, this will convert the pixel format if necessary.
	IMAGE_ABI bool copy(canvas& src_canvas, canvas& dst_canvas, const math::recti& src_rect, const math::vector2i& dst_pos)
	{
		// allow 0 width and height as a no op
		if(src_rect.get_width() == 0 || src_rect.get_height() == 0)
			return true;
			
			
		int width = dst_canvas.get_width();
		int height = dst_canvas.get_height();
		
		// crop to fit to destination
		if(dst_pos.x() >= width ||
		   dst_pos.y() >= height)
		{
			return true;
		}
				
		if((dst_pos.x() + width) > dst_canvas.get_width())
			width = dst_canvas.get_width() - dst_pos.x();
		if((dst_pos.y() + height) > dst_canvas.get_height())
			height = dst_canvas.get_height() - dst_pos.y();
		 
		// super slow 
		vector2i tl = src_rect.get_top_left();
		for(int sy = tl.y(), dy = dst_pos.y(); sy < height; ++sy, ++dy)
		{
			for(int sx = tl.x(), dx = dst_pos.x(); sx < width; ++sx, ++dx)
			{
				core::rgba pixel = src_canvas.get_pixel(sx, sy);
				dst_canvas.put_pixel(pixel, dx, dy);
			}
		}
		return true;
	}
	
	/// Resize the image from one canvas to another, this will convert the pixel format if necessary.
	/// \todo implement src_rect + dst_rect support
	IMAGE_ABI bool resize(canvas& src_canvas, canvas& dst_canvas, const math::recti& /*src_rect*/, const math::recti& /*dst_rect*/, filter_type)
	{
		kernel2f box_filter;
		box_filter.value[0][0] = 0.25f;
		box_filter.value[0][1] = 0.25f;
		box_filter.value[1][0] = 0.25f;
		box_filter.value[1][1] = 0.25f;
		apply_kernel(src_canvas, dst_canvas, box_filter);		
		return true;
	}
	
	IMAGE_ABI bool emboss(canvas& src_canvas, canvas& dst_canvas, const math::recti& /*src_rect*/, const math::recti& /*dst_rect*/)
	{
		kernel3f emboss_filter;
		float* v = &emboss_filter.value[0][0];
		*v++ = -0.5f; *v++ = 0; *v++ = 0;
		*v++ = 0;     *v++ = 1; *v++ = 0;
		*v++ = 0;     *v++ = 0; *v++ = 0;		
		apply_kernel(src_canvas, dst_canvas, emboss_filter);
		return true;
	}

	IMAGE_ABI bool edge_detect(canvas& src_canvas, canvas& dst_canvas, const math::recti& /*src_rect*/, const math::recti& /*dst_rect*/)
	{
#if 0
		image_base_ptr tmp = image_base_ptr(new image_rgb24());
		tmp->resize_canvas(dst_canvas.get_width(), dst_canvas.get_height(), 1, false);
		canvas tmp_c;
		if(tmp->get_mip_level(0, &tmp_c))
		{
			kernel3f k0;
			float* v = &k0.value[0][0];
			*v++ =  1; *v++ =  2; *v++ =  1;
			*v++ =  0; *v++ =  0; *v++ =  0;
			*v++ = -1; *v++ = -2; *v++ = -1;		
			kernel3f k1;
			v = &k1.value[0][0];
			*v++ =  1; *v++ =  0; *v++ = -1;
			*v++ =  2; *v++ =  0; *v++ = -2;
			*v++ =  1; *v++ =  0; *v++ = -1;
			apply_kernel(src_canvas, tmp_c, k0);
			apply_kernel(tmp_c, dst_canvas, k1);
		}
		return true;
#else
		kernel3f k0;
		float* v = &k0.value[0][0];
		float c = -1.0f/8.0f;
		*v++ = c; *v++ = c; *v++ = c;
		*v++ = c; *v++ = 1; *v++ = c;
		*v++ = c; *v++ = c; *v++ = c;		
		apply_kernel(src_canvas, dst_canvas, k0);
		return true;
#endif			
	}
	
	/// gamma correct the image
	IMAGE_ABI bool gamma_correct(image_base_ptr)
	{
		return false;
	}
	
	/// build the mip chain for the image
	/// \todo implement support for min_mip_width and min_mip_height
	IMAGE_ABI bool build_mip_chain(image_base_ptr img, int /*min_mip_width*/, int /*min_mip_height*/)
	{
		canvas src_c;
		if(!img->get_mip_level(0, &src_c))
			return false;
			
		for(int m = 1; m < img->get_num_mips(); ++m)
		{
			canvas dst_c;
			if(!img->get_mip_level(m, &dst_c))
				return false;
			if(!image::resize(src_c, dst_c, img->get_rect(m-1), img->get_rect(m), filter_type_box))
				return false;
			src_c = dst_c;
		}
		
		return true;
	}


} // end namespace
} // end namespace
