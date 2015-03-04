//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Wednesday, 30 April 2008 11:01:16 AM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/format_dds.h"
#include "image/image.h"
#include "image/image_rgba32.h"
#include "image/canvas.h"
#include "core/colour/rgba.h"
#include "core/debug/assert.h"
#include "core/memory.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace image
{
namespace dds
{
	
	/// Indicates a complex surface structure is being described.  A complex surface structure 
	/// results in the creation of more than one surface.  The additional surfaces are attached 
	/// to the root surface.  The complex structure can only be destroyed by destroying the root.
	static const int DDSCAPS_COMPLEX = 0x00000008l;

	/// Indicates that this surface can be used as a 3D texture.  It does not
	/// indicate whether or not the surface is being used for that purpose.
	static const int DDSCAPS_TEXTURE = 0x00001000l;

	/// Indicates surface is one level of a mip-map. This surface will
	/// be attached to other DDSCAPS_MIPMAP surfaces to form the mip-map.
	/// This can be done explicitly, by creating a number of surfaces and
	/// attaching them with AddAttachedSurface or by implicitly by CreateSurface.
	/// If this bit is set then DDSCAPS_TEXTURE must also be set.
	static const int DDSCAPS_MIPMAP = 0x00400000l;


	static const int DDPF_ALPHAPIXELS  = 0x00000001;

    // Unused currently
    //static const int DDPF_FOURCC = 0x00000004;

	static const int DDPF_RGB = 0x00000040;
	
	static const int DDSD_CAPS = 0x00000001;
	static const int DDSD_HEIGHT = 0x00000002; 
	static const int DDSD_WIDTH = 0x00000004;
	static const int DDSD_PITCH = 0x00000008;
	static const int DDSD_PIXELFORMAT = 0x00001000;
	static const int DDSD_MIPMAPCOUNT = 0x00020000;

    // Unused currently
    //static const int DDSD_LINEARSIZE = 0x00080000;
    //static const int DDSD_DEPTH = 0x00800000;
	

	struct dds_caps2
	{
		core::uint32 caps;         ///< capabilities of surface wanted
		core::uint32 caps2;
		core::uint32 reserved[2];
	};

	struct dd_color_key
	{
		core::uint32 dwColorSpaceLowValue;   ///< low boundary of color space that is to be treated as Color Key, inclusive
		core::uint32 dwColorSpaceHighValue;  ///< high boundary of color space that is to be treated as Color Key, inclusive
	};

	 struct dd_pixel_format
	{
		core::uint32 dwSize;                 ///< size of structure
		core::uint32 dwFlags;                ///< pixel format flags
		core::uint32 dwFourCC;               ///< (FOURCC code)
		core::uint32 dwRGBBitCount;          ///< how many bits per pixel
		core::uint32 dwRBitMask;             ///< mask for red bit
		core::uint32 dwGBitMask;             ///< mask for green bits
		core::uint32 dwBBitMask;             ///< mask for green bits
		core::uint32 dwRGBAlphaBitMask;      ///< mask for alpha channel
	};

	struct dd_surface_desc2
	{
		core::uint32  dwSize;                 ///< size of the DDSURFACEDESC structure
		core::uint32  dwFlags;                ///< determines what fields are valid
		core::uint32  dwHeight;               ///< height of surface to be created
		core::uint32  dwWidth;                ///< width of input surface
		union
		{
			core::int32  lPitch;                 ///< distance to start of next line (return value only)
			core::uint32 dwLinearSize;           ///< Formless late-allocated optimized surface size
		};
		core::uint32    dwDepth;                ///< the depth if this is a volume texture 
		core::uint32    dwMipMapCount;          ///< number of mip-map levels requested
		core::uint32    dwReservered1[11];
		dd_pixel_format	pixel_format;
		dds_caps2	    caps;
		core::uint32    reserved2;
	};
		
	
	static const core::int8 magic[4] = { 'D', 'D', 'S', ' ' };

} // end namespace

	using namespace dds;
	
	/// initialise DDS format handler
	void format_dds::initialise()
	{

	}

	/// \returns true if the signature is a DDS file
	/// \todo implement format_dds::identify
	bool format_dds::identify(const char* /*signature*/, int /*signature_len*/)
	{
		return false;
	}

	/// Load a DDS file from a memory buffer
	image_base_ptr format_dds::load(io::stream&)
	{
		return image_base_ptr();
	}

	// Save an image as a DDS file
	bool format_dds::save(image_base_ptr img, io::stream& str)
	{
		if(!img || !img->get_width() || !img->get_height() || str.fail())
			return false;
		
		// write out the image header
		const int img_width = img->get_width();
		const int img_height = img->get_height();
		const int img_num_mips = img->get_num_mips();
		dd_surface_desc2 desc;
		core::mem_zero(desc);
		desc.dwSize = sizeof(dd_surface_desc2);
		desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_MIPMAPCOUNT | DDSD_PITCH;
		desc.dwHeight = img_height;
		desc.dwWidth = img_width;
		desc.lPitch = img_width * 4;
		desc.dwDepth = 0;
		desc.dwMipMapCount = img_num_mips;
		desc.pixel_format.dwSize = sizeof(desc.pixel_format);
		desc.pixel_format.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		desc.pixel_format.dwRGBBitCount = 32;
		desc.pixel_format.dwRBitMask = 0x00ff0000;
		desc.pixel_format.dwGBitMask = 0x0000ff00;
		desc.pixel_format.dwBBitMask = 0x000000ff;
		desc.pixel_format.dwRGBAlphaBitMask = 0xff000000;
		desc.caps.caps = DDSCAPS_TEXTURE;;
		if(img_num_mips > 1)
			desc.caps.caps |= (DDSCAPS_MIPMAP | DDSCAPS_COMPLEX);
		
		str.write((char*)(&dds::magic[0]), 4);
		str.write((char*)(&desc), sizeof(dd_surface_desc2));

		// loop over mip levels writing them out in bgra format
		image_rgba32 line_buf(image_rgba::pixel_layout_bgra8888);
		line_buf.resize_canvas(img_width, 1, 1, false);
		canvas dst_c;
		if(!line_buf.get_mip_level(0, &dst_c))
			return false;
		for(int i = 0; i < img_num_mips; ++i)
		{
			canvas src_c;
			if(!img->get_mip_level(i, &src_c))
				return false;
			int width = src_c.get_width();
			int height = src_c.get_height();
			for(int y = 0; y < height; ++y)
			{
				line_buf.copy(0, 0, 0, y, width, 1, src_c, dst_c);				
				str.write((const char*)dst_c.get_pixels(), width * 4);
			}			
		}
		
		return true;
	}

	/// Save a mip level in an image as a DDS file
	bool format_dds::save(image_base_ptr img, int mip_level, io::stream& str)
	{
		if(!img || !img->get_width() || !img->get_height())
			return false;

		canvas src_c;
		if(!img->get_mip_level(mip_level, &src_c))
			return false;
				
		// write out the image header
		const int img_width = src_c.get_width();
		const int img_height = src_c.get_height();
		dd_surface_desc2 desc;
		core::mem_zero(desc);
		desc.dwSize = sizeof(dd_surface_desc2);
		desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH;
		desc.dwHeight = img_height;
		desc.dwWidth = img_width;
		desc.lPitch = img_width * 4;
		desc.pixel_format.dwSize = sizeof(desc.pixel_format);
		desc.pixel_format.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		desc.pixel_format.dwRGBBitCount = 32;
		desc.pixel_format.dwRBitMask = 0x00ff0000;
		desc.pixel_format.dwGBitMask = 0x0000ff00;
		desc.pixel_format.dwBBitMask = 0x000000ff;
		desc.pixel_format.dwRGBAlphaBitMask = 0xff000000;
		desc.caps.caps = DDSCAPS_TEXTURE;;
		
		str.write((char*)(&dds::magic[0]), 4);
		str.write((char*)(&desc), sizeof(dd_surface_desc2));

		// loop over mip levels writing them out in bgra format
		image_rgba32 line_buf(8, 0xff, 16, 0xff, 24, 0xff, 0, 0xff);
		line_buf.resize_canvas(img_width, 1, 1, false);
		canvas dst_c;
		if(!line_buf.get_mip_level(0, &dst_c))
			return false;
		int width = src_c.get_width();
		int height = src_c.get_height();
		for(int y = 0; y < height; ++y)
		{
			line_buf.copy(0, 0, 0, y, width, 1, src_c, dst_c);				
			str.write((const char*)dst_c.get_pixels(), width * 4);
		}			
		
		return true;		
	}

	bool format_dds::save_rgba(image_base_ptr, io::stream&)
	{
		return false;
	}


} // end namespace
} // end namespace
