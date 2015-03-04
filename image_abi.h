//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Wednesday, 5 March 2008 1:29:49 AM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IMAGE_ABI_H_80E464F4_809C_4F46_8586_1901CE2E6C48_
#define __IMAGE_ABI_H_80E464F4_809C_4F46_8586_1901CE2E6C48_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////

#if TYCHO_PC

#ifdef TYCHO_IMAGE_EXPORTS
#define IMAGE_ABI __declspec(dllexport)
#define PNG_BUILD_DLL
#else
#define IMAGE_ABI __declspec(dllimport)
#endif 

// class '' needs to have dll-interface to be used by clients of class ''
#pragma warning(disable : 4251) 

#else // TYCHO_PC

#define IMAGE_ABI

#endif // TYCHO_PC

#include "core/types.h"

#endif // __IMAGE_ABI_H_80E464F4_809C_4F46_8586_1901CE2E6C48_
