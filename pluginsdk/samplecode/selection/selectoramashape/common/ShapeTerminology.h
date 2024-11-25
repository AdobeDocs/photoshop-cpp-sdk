// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		Shape.r
//
//	Description:
//		This file contains the resource definitions for the
//		Import module Shape, which returns a path, layer,
//		or selection with an interesting shape.
//
//	Use:
//		This module specifically works the path return
//		functionality of Photoshop.  The Paths are stored
//		by derezing a work path saved in Photoshop then
//		storing them in a "Path" resource, which is a
//		binary data resource of a "Path" as defined in the
//		"Path layout" section of the Photosop File Format.
//
//-------------------------------------------------------------------------------

#ifndef __ShapeTerminology_h__	// Have we defined this?
#define __ShapeTerminology_h__	// Only define once.

//-------------------------------------------------------------------------------
//	Definitions -- Scripting keys.
//-------------------------------------------------------------------------------

#define shapeTriangle		'shP0'
#define shapeSquare			'shP1'
#define shapeCircle			'shP2'
#define shapeStar			'shP3'
#define shapeTreble			'shP4'
#define	shapeRibbon			'shP5'
#define shapeNote			'shP6'
#define typeMyShape			'tshP'
#define keyMyShape			keyShape
#define keyMyCreate			'kcrE'
#define typeMyCreate		'tcrE'
#define typeMyPISel			'tpiS'
#define	createSelection		'crE0'
#define createMaskpath		'crE1'
#define createLayer			'crE2'

//-------------------------------------------------------------------------------
//	Definitions -- Resource types.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

#endif // __ShapeTerminology_h__
