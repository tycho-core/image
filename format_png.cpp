//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 11:16:35 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "format_png.h"
#include "core/debug/assert.h"
#include "core/memory.h"
#include "core/memory/allocator.h"
#include "core/console.h"
#include "image/libpng/png.h"
#include "image_rgb24.h"
#include "image_rgba32.h"

/// \todo Need to decide how to deal with libpng errors
//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace image
{
namespace detail
{
	png_voidp libpng_malloc(png_structp png_ptr, png_size_t size)
	{
		return core::allocator::malloc(size);
	}
	
	void libpng_free(png_structp png_ptr, png_voidp ptr)
	{
		core::allocator::free((void*)ptr);
	}

	
	void libpng_error(png_structp, png_const_charp msg)
	{
		core::console::write_ln("libpng : error : %s", msg);
	}

	void libpng_warning(png_structp, png_const_charp msg)
	{
		core::console::write_ln("libpng : warning : %s", msg);
	}
	
    void libpng_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {	
		const char* buf = (const char*)png_get_io_ptr(png_ptr);
		core::mem_cpy(data, buf, length);
		png_ptr->io_ptr =  (png_voidp)(buf + length);
    }
        
    void libpng_read_stream(png_structp png_ptr, png_bytep data, png_size_t length)
    {			
		io::stream* str = reinterpret_cast<io::stream*>(png_get_io_ptr(png_ptr));
		str->read((char*)data, length);
    }

    void libpng_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
    }
    
	
	void libpng_write_to_stream	(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		io::stream* str = reinterpret_cast<io::stream*>(png_ptr->io_ptr);
		TYCHO_ASSERT(str);
		if(str)
		{	
			str->write((char*)data, length);
		}
	}

	/// do nothing flush for writers that don't write to disk
    void libpng_null_flush(png_structp png_ptr)
    {
    }

	
} // end namespace

	/// initialise libpng
	void format_png::initialise()
	{
		
	}
	
	/// \returns true if the signature is a PNG file
	bool format_png::identify(const char* signature, int signature_len)
	{
		return png_sig_cmp((png_bytep)signature, 0, signature_len) == 0;
	}

	/// Load a PNG file from a stream
	image_base_ptr format_png::load(io::stream& stream)
	{
		struct libpng_read_ptrs
		{
			libpng_read_ptrs() : info(0), read(0) {}
			~libpng_read_ptrs() 
			{ 
				png_destroy_read_struct(read ? &read : (png_structpp)NULL,
				                        info ? &info : (png_infopp)NULL,
				                        end ? & end : (png_infopp)NULL);				                        
			}
			
			png_infop  info;
			png_infop  end;
			png_structp read;			
		} ptrs;			
			

		ptrs.read = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, 
									png_voidp_NULL,
									detail::libpng_error, 
									detail::libpng_warning, 
									png_voidp_NULL,
									detail::libpng_malloc,
									detail::libpng_free);		
		if(!ptrs.read)
			return image_base_ptr();
										
		png_set_read_fn(ptrs.read, (png_voidp)&stream, detail::libpng_read_stream);
				
		ptrs.info = png_create_info_struct(ptrs.read);
		if(!ptrs.info)
			return image_base_ptr();
		
		ptrs.end = png_create_info_struct(ptrs.read);
		if(!ptrs.end)
			return image_base_ptr();
		
		image_base_ptr result;
		png_read_png(ptrs.read, ptrs.info, PNG_TRANSFORM_IDENTITY, NULL);
		if(ptrs.info->width && ptrs.info->height)
		{
			switch(ptrs.info->color_type)
			{
				case PNG_COLOR_TYPE_GRAY : break;
				case PNG_COLOR_TYPE_PALETTE : {
					// expand to rgb
					png_colorp palette = ptrs.info->palette;
					if(!palette)
						return image_base_ptr();						
					result = image_base_ptr(new image_rgb24());
					result->resize_canvas(ptrs.info->width, ptrs.info->height, 1, false);
					for(int y = 0; y < (int)ptrs.info->height; ++y)
					{
						core::uint8* row = ptrs.info->row_pointers[y];
						for(int x = 0; x < (int)ptrs.info->width; ++x, ++row)
						{
							const png_color &c = palette[*row];
							result->put_pixel(core::rgba(c.red, c.green, c.blue), 0, x, y);
						}
					}
				} break;
				
				case PNG_COLOR_TYPE_RGB : {
					if(ptrs.info->bit_depth == 8)
					{
						result = image_base_ptr(new image_rgb24());
						result->resize_canvas(ptrs.info->width, ptrs.info->height, 1, false);
						// just copy all rows
						for(int y = 0; y < (int)ptrs.info->height; ++y)
						{
							result->raw_copy(0, 0, y, ptrs.info->width, 1, (const core::uint8*)ptrs.info->row_pointers[y], 4 * ptrs.info->width);
						}
					}
				} break;
				
				case PNG_COLOR_TYPE_RGB_ALPHA : {
					if(ptrs.info->bit_depth == 8)
					{
						result = image_base_ptr(new image_rgba32());
						result->resize_canvas(ptrs.info->width, ptrs.info->height, 1, false);
						// just copy all rows
						for(int y = 0; y < (int)ptrs.info->height; ++y)
						{
							result->raw_copy(0, 0, y, ptrs.info->width, 1, (const core::uint8*)ptrs.info->row_pointers[y], 4 * ptrs.info->width);
						}
					}
				} break;
				
				case PNG_COLOR_TYPE_GRAY_ALPHA : break;
			}
		}
		
		return result;
	}
	
	/// Save the image in PNG format
	bool format_png::save(image_base_ptr img, io::stream& str)
	{
		if(!img)
			return false;
			
		switch(img->get_image_format())
		{
			case image_format_rgba16 :
			case image_format_rgba24 :
			case image_format_rgba32 : return format_png::save_rgba(img, str);
			default : TYCHO_NOT_IMPLEMENTED;
		}
		
		return false;
	}

	bool format_png::save_rgba(image_base_ptr img, io::stream& str)
	{	
		if(!img || !img->get_width() || !img->get_height())
			return false;
			
		struct libpng_write_ptrs
		{
			libpng_write_ptrs() : info(0), write(0) {}
			~libpng_write_ptrs() 
			{ 
				png_destroy_write_struct(write ? &write : (png_structpp)NULL,
				                         info ? &info : (png_infopp)NULL);				                        
			}
			
			png_infop  info;
			png_structp write;
			
		} ptrs;
	
		ptrs.write = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, 
											   png_voidp_NULL,
											   detail::libpng_error, 
											   detail::libpng_warning, 
											   png_voidp_NULL,
											   detail::libpng_malloc,
											   detail::libpng_free);		
		if(!ptrs.write)
			return false;

		png_set_write_fn(ptrs.write, (png_voidp)&str, detail::libpng_write_to_stream, detail::libpng_null_flush);			
		
		ptrs.info = png_create_info_struct(ptrs.write);		
		if(!ptrs.info)
		   return false;
		   
		//TODO : we cheat here and just save everything as rgba 32 bit, should just use the whatever the source image has
		canvas src_c;
		if(!img->get_mip_level(0, &src_c))
			return false;		
		
		// convert to canonical form, rgba 8 bits per pixel, slow and waste of memory but simple for now.
		std::vector<core::rgba> tmp_buf;
		int width = src_c.get_width();
		int height = src_c.get_height();
		tmp_buf.resize(width * height);
		core::rgba* dst_ptr = &tmp_buf[0];
		for(int y = 0; y < height; ++y)
		{
			for(int x = 0; x < width; ++x)
			{
				*dst_ptr = src_c.get_pixel(x, y);
				if(!img->has_channel(colour_channel_alpha))
					dst_ptr->a(255);
				++dst_ptr;
			}
		}

		// setup row pointers
		std::vector<png_bytep> row_pointers;
		row_pointers.resize(height);
		for(int i = 0; i < height; ++i)
			row_pointers[i] = reinterpret_cast<png_bytep>(&tmp_buf[i * width]);		
				
		// fill out the PNG header
        png_set_IHDR(ptrs.write,
                     ptrs.info,
                     width,
                     height,
                     8, // bit_depth
                     PNG_COLOR_TYPE_RGB_ALPHA,
                     false,
                     PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_DEFAULT);                         			
		png_set_rows(ptrs.write, ptrs.info, &row_pointers[0]);
		png_write_png(ptrs.write, ptrs.info, PNG_TRANSFORM_IDENTITY, NULL);		
				
		// all done				
		return true;
	}

} // end namespace
} // end namespace
