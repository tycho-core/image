//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 11:16:35 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __format_png_H_3D140981_CBA0_478D_ABD8_65F27E81447E_
#define __format_png_H_3D140981_CBA0_478D_ABD8_65F27E81447E_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "image/forward_decls.h"
#include "io/stream.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{

	/// PNG format interface
    class IMAGE_ABI format_png
    {
    public:
		/// initialise libpng
		static void initialise();
		
		/// \returns true if the signature is a PNG file
		static bool identify(const char* signature, int signature_len);

		/// Load a PNG file from a stream
		static image_base_ptr load(io::stream&);
		
		/// Save an image as a PNG file
		/// \warning currently always converts to 32bit rgba.
		static bool save(image_base_ptr, io::stream&);
		
	private:
		static bool save_rgba(image_base_ptr, io::stream&);
    };

} // end namespace

} // end namespace

#endif // __format_png_H_3D140981_CBA0_478D_ABD8_65F27E81447E_
