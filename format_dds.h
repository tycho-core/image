//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Wednesday, 30 April 2008 11:01:14 AM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __FORMAT_DDS_H_63A59559_F891_4B1B_A36D_0DA676FCED60_
#define __FORMAT_DDS_H_63A59559_F891_4B1B_A36D_0DA676FCED60_

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

	/// DDS format interface
	class IMAGE_ABI format_dds
	{
	public:
		/// initialise DDS format handler
		static void initialise();

		/// \returns true if the signature is a DDS file
		static bool identify(const char* signature, int signature_len);

		/// Load a DDS file from a memory buffer
		static image_base_ptr load(io::stream&);

		/// Save an image as a DDS file
		/// \warning currently always converts to 32bit rgba.
		static bool save(image_base_ptr, io::stream&);

		/// Save a mip level in an image as a DDS file
		/// \warning currently always converts to 32bit rgba.
		static bool save(image_base_ptr, int, io::stream&);

	private:
		static bool save_rgba(image_base_ptr, io::stream&);
	};

} // end namespace
} // end namespace

#endif // __FORMAT_DDS_H_63A59559_F891_4B1B_A36D_0DA676FCED60_
