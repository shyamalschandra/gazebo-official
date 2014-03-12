///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  OPCODE - Optimized Collision Detection
 *  Copyright (C) 2001 Pierre Terdiman
 *  Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  Main file for Opcode.dll.
 *  \file  	Opcode.h
 *  \author  	Pierre Terdiman
 *  \date  	March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPCODE_H__
#define __OPCODE_H__

// stdarg.h must be included before Opcode headers as it later
// may not compile being not able to find std::va_list
#include <stdarg.h>

#include <gazebo/util/system.hh>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Things to help us compile on non-windows platforms

#if defined(__APPLE__) || defined(__MACOSX__)
#if __APPLE_CC__ < 1495
#define sqrtf sqrt
#define sinf sin
#define cosf cos
#define acosf acos
#define asinf asin
#endif
#endif

#ifndef _MSC_VER
#ifndef __int64
#define __int64 long long int
#endif
#ifndef __stdcall /* this is defined in MinGW and CygWin, so avoid the warning */
#define __stdcall /* */
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compilation messages
#ifdef _MSC_VER
  #if defined(OPCODE_EXPORTS)
    // #pragma message("Compiling OPCODE")
  #elif !defined(OPCODE_EXPORTS)
    // #pragma message("Using OPCODE")
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Automatic linking
    #ifndef BAN_OPCODE_AUTOLINK
    	#ifdef _DEBUG
    		//#pragma comment(lib, "Opcode_D.lib")
    	#else
    		//#pragma comment(lib, "Opcode.lib")
    	#endif
    #endif
  #endif
#endif

//////////////////////////////////////////////////
// Preprocessor
// #ifndef ICE_NO_DLL
//   #ifdef OPCODE_EXPORTS
//     #define OPCODE_API// __declspec(dllexport)
//   #else
//     #define OPCODE_API// __declspec(dllimport)
//   #endif
// #else
//   #define OPCODE_API
// #endif

// Preprocessor__
#define OPCODE_API GAZEBO_VISIBLE

  #include "OPC_Settings.h"
  #include "OPC_IceHook.h"

  namespace Opcode
  {
    // Bulk-of-the-work
    #include "OPC_Common.h"
    #include "OPC_MeshInterface.h"
    // Builders
    #include "OPC_TreeBuilders.h"
    // Trees
    #include "OPC_AABBTree.h"
    #include "OPC_OptimizedTree.h"
    // Models
    #include "OPC_BaseModel.h"
    #include "OPC_Model.h"
    #include "OPC_HybridModel.h"
    // Colliders
    #include "OPC_Collider.h"
    #include "OPC_VolumeCollider.h"
    #include "OPC_TreeCollider.h"
    #include "OPC_RayCollider.h"
    #include "OPC_SphereCollider.h"
    #include "OPC_OBBCollider.h"
    #include "OPC_AABBCollider.h"
    #include "OPC_LSSCollider.h"
    #include "OPC_PlanesCollider.h"
    // Usages
    #include "OPC_Picking.h"


    FUNCTION OPCODE_API bool InitOpcode();
    FUNCTION OPCODE_API bool CloseOpcode();
  }

#endif // __OPCODE_H__
