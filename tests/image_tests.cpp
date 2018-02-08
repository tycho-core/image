//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 27 April 2008 11:42:40 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "core/memory/new.h"
#include "image/image.h"
#include "image/image_a8.h"
#include "image/image_rgba16.h"
#include "image/image_rgb24.h"
#include "image/image_rgba32.h" 
#include "image/image_functions.h"
#include "image/format_png.h"
#include "image/format_dds.h"
#include "core/core.h"
#include "core/globals.h"
#include "core/string.h"
#include "core/platform.h"
#include "core/colour/rgba.h"
#include "io/file.h"
#include "io/file_stream.h"
#include "io/memory_stream.h"
#include "io/interface.h"
#include "io/filesystem_device.h"
#include "test/global_test_fixture.h"
#include <stdio.h>

using namespace tycho::image;

//////////////////////////////////////////////////////////////////////////////
// TEST SETUP
//////////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE image
#include <boost/test/unit_test.hpp>

tycho::io::interface g_io_interface;

struct global_test_fixture : tycho::test::global_test_fixture
{

	global_test_fixture() 
	{
		using namespace tycho;

		std::string base_dir = core::current_working_directory();
		io::filesystem_device_ptr fs_device(new io::filesystem_device());
		g_io_interface.add_device(fs_device);
		g_io_interface.mount("/temp/", fs_device->make_mount_point(core::temp_dir().c_str()));
		g_io_interface.mount("/", fs_device->make_mount_point(base_dir.c_str()));
	}

	~global_test_fixture()
	{
	}
};

BOOST_GLOBAL_FIXTURE(global_test_fixture);

//////////////////////////////////////////////////////////////////////////////
// TEST CASES
//////////////////////////////////////////////////////////////////////////////

template<class T0, class T1>
void test_copy_aux()
{
	using namespace tycho;
	using namespace tycho::core;

	T0 image0;
	T1 image1;
	image0.resize_canvas(4, 4, 1, false);
	image1.resize_canvas(4, 4, 1, false);
	for(int y = 0; y < 4; ++y)
		for(int x = 0; x < 4; ++x)
			image0.put_pixel(rgba(y, x, y*x, y+x), 0, x, y);			
	image1.clear(rgba(0,0,0,0), 0);
	canvas c0, c1;
	BOOST_REQUIRE(image0.get_mip_level(0, &c0));
	BOOST_REQUIRE(image1.get_mip_level(0, &c1));
	image0.copy(0, 0, 0, 0, 4, 4, c0, c1);	
	for(int y = 0; y < 4; ++y)
	{
		for(int x = 0; x < 4; ++x)
		{
			rgba c0(y, x, y * x, y+x);
			rgba c1 = image1.get_pixel(0, x, y);
			if(!image0.has_channel(colour_channel_alpha))
				c0.a(255);
			else if(!image1.has_channel(colour_channel_alpha))
				c1.a(c0.a());	
			if(!image0.has_channel(colour_channel_red))
				c0.r(0);
			else if(!image1.has_channel(colour_channel_red))
				c1.r(c0.r());	
			if(!image0.has_channel(colour_channel_green))
				c0.g(0);
			else if(!image1.has_channel(colour_channel_green))
				c1.g(c0.g());	
			if(!image0.has_channel(colour_channel_blue))
				c0.b(0);
			else if(!image1.has_channel(colour_channel_blue))
				c1.b(c0.b());	
						
			BOOST_CHECK(c0 == c1);
		}
	}
}

template<class T0, class T1>
void test_copy_impl()
{
	test_copy_aux<T0, T1>();
	test_copy_aux<T1, T0>();	
}

template<class T>
void test_raw_copy()
{
	using namespace tycho;
	using namespace tycho::core;

	T image;
	T image1;
	image.resize_canvas(4, 4, 1, false);
	image1.resize_canvas(4, 4, 1, false);
	image.clear(rgba(27,17,07,01), 0);
	image1.clear(rgba(0,0,0,0), 0);
	canvas c;
	BOOST_REQUIRE(image.get_mip_level(0, &c));	
	image1.raw_copy(0, 0, 0, 4, 4, c.get_pixels(), c.get_byte_size());
	for(int y = 0; y < image1.get_width(); ++y)
	{
		for(int x = 0; x < image1.get_height(); ++x)
		{
			rgba c0(27,17,07,01); // gotta keep these lower than 5 bits to test the 16bit formats.
			rgba c1 = image1.get_pixel(0, x, y);
			if(!image.has_channel(colour_channel_alpha))
				c1.a(c0.a());
			if(!image.has_channel(colour_channel_red))
				c1.r(c0.r());
			if(!image.has_channel(colour_channel_green))
				c1.g(c0.g());
			if(!image.has_channel(colour_channel_blue))
				c1.b(c0.b());
			BOOST_CHECK(c0 == c1);
		}
	}
}

template<class T>
void test_rgba_format_impl(image_format format)
{
	using namespace tycho;
	using namespace tycho::core;

	T image;
	BOOST_CHECK(image.get_width() == 0);
	BOOST_CHECK(image.get_num_mips() == 0);
	BOOST_CHECK(image.get_height() == 0);	
	image.resize_canvas(4, 4, 1, false);
	BOOST_CHECK(image.get_width() == 4);
	BOOST_CHECK(image.get_height() == 4);
	BOOST_CHECK(image.get_num_mips() == 1);
	BOOST_CHECK(image.get_image_format() == format);
	image.clear(rgba(0,0,0,0), 0);
	rgba black(0,0,0,0);
	for(int y = 0; y < 4; ++y)
	{
		for(int x = 0; x < 4; ++x)
		{
			rgba c = image.get_pixel(0, 0, 0);
			if(!image.has_channel(colour_channel_alpha))
				c.a(0);
			if(!image.has_channel(colour_channel_red))
				c.r(0);
			if(!image.has_channel(colour_channel_green))
				c.g(0);
			if(!image.has_channel(colour_channel_blue))
				c.b(0);
			BOOST_CHECK(c == black);
		}
	}
	
	// put / get pixel
	for(int y = 0; y < image.get_width(); ++y)
	{
		for(int x = 0; x < image.get_height(); ++x)
		{
			rgba c0(x, y, 23, 4); // gotta keep these lower than 5 bits to test the 16bit formats.
			image.put_pixel(c0, 0, x, y);
			rgba c1 = image.get_pixel(0, x, y);
			if(!image.has_channel(colour_channel_alpha))
				c1.a(c0.a());
			if(!image.has_channel(colour_channel_red))
				c1.r(c0.r());
			if(!image.has_channel(colour_channel_green))
				c1.g(c0.g());
			if(!image.has_channel(colour_channel_blue))
				c1.b(c0.b());
			BOOST_CHECK(c0 == c1);
		}
	}
	
	test_raw_copy<T>();
}

BOOST_AUTO_TEST_CASE(test_png_load)
{
	using namespace tycho;
	using namespace tycho::core;

	// 32 bit color test
	{
		#include "png_32bit_clr_test.inc"
		io::memory_stream str((char*)png_32bit_clr_test, png_32bit_clr_testLen);
		image_base_ptr i = format_png::load(str);	
		BOOST_CHECK(i);
		BOOST_CHECK(i->get_width() == 2);
		BOOST_CHECK(i->get_height() == 2);
		BOOST_CHECK(i->get_num_mips() == 1);
		BOOST_CHECK(i->get_image_format() == image_format_rgba32); 
		canvas c;
		BOOST_CHECK(i->get_mip_level(0, &c));
		BOOST_CHECK(c.get_width() == 2);
		BOOST_CHECK(c.get_height() == 2);
		BOOST_CHECK(c.get_pixel(0, 0) == rgba(255,0,0,255)); 
			BOOST_CHECK(c.get_pixel(1, 0) == rgba(0,255,0,255));
		BOOST_CHECK(c.get_pixel(0, 1) == rgba(0,0,255,255)); 
			BOOST_CHECK(c.get_pixel(1, 1) == rgba(0,0,0,0));
	}

	// 24 bit color test
	{
		#include "png_24bit_clr_test.inc"
		io::memory_stream str((char*)png_24bit_clr_test, png_24bit_clr_testLen);
		image_base_ptr i = format_png::load(str);	
		BOOST_CHECK(i);
		BOOST_CHECK(i->get_width() == 2);
		BOOST_CHECK(i->get_height() == 2);
		BOOST_CHECK(i->get_num_mips() == 1);
		BOOST_CHECK(i->get_image_format() == image_format_rgba24); 
		canvas c;
		BOOST_CHECK(i->get_mip_level(0, &c));
		BOOST_CHECK(c.get_width() == 2);
		BOOST_CHECK(c.get_height() == 2);
		BOOST_CHECK(c.get_pixel(0, 0) == rgba(255,0,0,255)); 
			BOOST_CHECK(c.get_pixel(1, 0) == rgba(0,255,0,255));
		BOOST_CHECK(c.get_pixel(0, 1) == rgba(0,0,255,255)); 
			BOOST_CHECK(c.get_pixel(1, 1) == rgba(128,64,32,255));
	}
	
	// paletted image
	{
		#include "png_palette_test.inc"	
		io::memory_stream str((char*)png_palette_test, png_palette_testLen);
		image_base_ptr i = format_png::load(str);	
		BOOST_CHECK(i);
		BOOST_CHECK(i->get_width() == 32);
		BOOST_CHECK(i->get_height() == 32);
		BOOST_CHECK(i->get_num_mips() == 1);
		BOOST_CHECK(i->get_image_format() == image_format_rgba24); // no palette support yet so converts to rgb888
	}
	
	// 32 bit image
	{
		#include "png_32bit_test.inc"	
		io::memory_stream str((char*)png_32bit_test, png_32bit_testLen);
		image_base_ptr i = format_png::load(str);	
		BOOST_CHECK(i);
		BOOST_CHECK(i->get_width() == 32);
		BOOST_CHECK(i->get_height() == 32);
		BOOST_CHECK(i->get_num_mips() == 1);
		BOOST_CHECK(i->get_image_format() == image_format_rgba32);
	}

	// 24 bit image
	{
		#include "png_24bit_test.inc"	
		io::memory_stream str((char*)png_24bit_test, png_24bit_testLen);
		image_base_ptr i = format_png::load(str);	
		BOOST_CHECK(i);
		BOOST_CHECK(i->get_width() == 32);
		BOOST_CHECK(i->get_height() == 32);
		BOOST_CHECK(i->get_num_mips() == 1);
		BOOST_CHECK(i->get_image_format() == image_format_rgba24);
	}	
}

BOOST_AUTO_TEST_CASE(test_resize)
{
	using namespace tycho;
	
	#include "png_24bit_test.inc"	
	io::memory_stream str((char*)png_24bit_test, png_24bit_testLen);
	image_base_ptr i = format_png::load(str);	
	image_base_ptr ni = image_base_ptr(new image_rgb24());
	ni->resize_canvas(i->get_width() / 2, i->get_height() / 2, 1, false);
	ni->clear(core::rgba(0,0,0,0), 0);
	canvas src_c;
	canvas dst_c;
	BOOST_REQUIRE(i->get_mip_level(0, &src_c));
	BOOST_REQUIRE(ni->get_mip_level(0, &dst_c));
	image::resize(src_c, dst_c, 
	       math::recti(0, 0, i->get_width(), i->get_height()),
	       math::recti(0, 0, ni->get_width(), ni->get_height()), filter_type_box);
	       
	io::stream_ptr ostr = g_io_interface.open_stream("/temp/output.png", io::open_flag_create | io::open_flag_write);
	TYCHO_VERIFY(format_png::save(ni, *ostr.get()));
	//ostr.close();
}

namespace fool
{
	#include "daisy.inc"	
}

void play()
{
	using namespace tycho;

	io::memory_stream istr((char*)fool::daisy, fool::daisyLen);
	image_base_ptr i = format_png::load(istr);	
	image_base_ptr ni = image_base_ptr(new image_rgb24());
	ni->resize_canvas(i->get_width(), i->get_height(), 1, false);
	ni->clear(core::rgba(0,0,0,0), 0);
	canvas src_c;
	canvas dst_c;
	BOOST_REQUIRE(i->get_mip_level(0, &src_c));
	BOOST_REQUIRE(ni->get_mip_level(0, &dst_c));
	image::edge_detect(src_c, dst_c, 
	       math::recti(0, 0, i->get_width(), i->get_height()),
	       math::recti(0, 0, ni->get_width(), ni->get_height()));

	io::stream_ptr ostr = g_io_interface.open_stream("/temp/play.png", io::open_flag_write | io::open_flag_create);
	BOOST_REQUIRE(ostr);
	BOOST_CHECK(format_png::save(ni, *ostr.get()));
}

BOOST_AUTO_TEST_CASE(test_mipchain)
{
	using namespace tycho;

	io::memory_stream str((char*)fool::daisy, fool::daisyLen);
	image_base_ptr i = format_png::load(str);	
	image_base_ptr ni = image_base_ptr(new image_rgb24());
	ni->resize_canvas(i->get_width(), i->get_height(), 3, false);
	image::copy(i, ni);
	BOOST_REQUIRE(image::build_mip_chain(ni, 4, 4));
	io::stream_ptr ostr = g_io_interface.open_stream("/temp/mip_chain_test.dds", io::open_flag_create | io::open_flag_write);
    BOOST_REQUIRE(ostr);
 	BOOST_CHECK(format_dds::save(ni, *ostr.get()));

 	for(int i = 0; i < 3; ++i)
 	{
 		char name_buf[128];
 		core::snprintf(name_buf, 128, "/temp/mip_chain_test_%d.png", i);
		io::stream_ptr ostr = g_io_interface.open_stream(name_buf, io::open_flag_create | io::open_flag_write);
 		BOOST_CHECK(format_dds::save(ni, i, *ostr.get())); 		
 	}
}

BOOST_AUTO_TEST_CASE(test_rgba_format)
{
	test_rgba_format_impl<image_a8>(image_format_a8);
	test_rgba_format_impl<image_rgba16>(image_format_rgba16);
	test_rgba_format_impl<image_rgb24>(image_format_rgba24);
	test_rgba_format_impl<image_rgba32>(image_format_rgba32);
}

BOOST_AUTO_TEST_CASE(test_copy)
{
	test_copy_impl<image_rgb24, image_rgba32>();
	test_copy_impl<image_rgba16, image_rgba32>();
	test_copy_impl<image_rgba16, image_rgb24>();
	test_copy_impl<image_a8, image_rgba16>();
	test_copy_impl<image_a8, image_rgb24>();
	test_copy_impl<image_a8, image_rgba32>();
}
