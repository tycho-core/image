//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 11:43:29 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TYPES_H_516EFAAD_A74B_4137_A39F_61A86EEC6D00_
#define __TYPES_H_516EFAAD_A74B_4137_A39F_61A86EEC6D00_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "image/image_abi.h"
#include "core/colour/rgba.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace image
{

	/// poor mans type info for image formats.
	enum image_format
	{
		image_format_rgba32,
		image_format_rgba24,
		image_format_rgba16,
		image_format_a8,
		image_format_dxtn,
		image_format_gc_dxt5, ///< gamecube has no dxt5 so we represent it as 2 dxt1, with the alpha channel stored in the second.
		image_format_srgb
	};

	enum colour_channel
	{
		colour_channel_red,
		colour_channel_green,
		colour_channel_blue,
		colour_channel_alpha
	};
	
} // end namespace
} // end namespace

#endif // __TYPES_H_516EFAAD_A74B_4137_A39F_61A86EEC6D00_
