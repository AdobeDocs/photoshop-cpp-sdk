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
//		LayerFormatUI.r
//
//	Description:
//		Dialog for the LayerFormat Mac project.
//
//-------------------------------------------------------------------------------

/* the list box control that looks a lot like a combo box :-) */
resource 'MENU' (16050) {
	16050,
	textMenuProc,
	allEnabled,
	enabled,
	"Resources",
	{	/* array: 4 elements */
		/* [1] */
		"temp", noIcon, noKey, noMark, plain,
	}
};

resource 'CNTL' (16050, purgeable) {
	{30, 10, 190, 160},
	0,
	visible,
	0,
	16050,
	1008,
	0,
	""
};

resource 'DITL' (16050, "LayerFormat UI", purgeable) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{8, 270, 28, 338},
		Button {
			enabled,
			"Remove"
		},
		/* [2] */
		{40, 270, 60, 338},
		Button {
			enabled,
			"Skip"
		},
		/* [3] */
		{8, 10, 24, 190},
		StaticText {
			disabled,
			"Resource Identifier"
		},
		/* [4] */
		{30, 10, 190, 160},
		Control {
			enabled,
			16050
		},
		/* [5] */
		{90, 170, 106, 260},
		StaticText {
			disabled,
			"Type:"
		},
		/* [6] */
		{90, 270, 106, 338},
		StaticText {
			disabled,
			"run time text"
		},
		/* [7] */
		{110, 170, 126, 260},
		StaticText {
			disabled,
			"ID:"
		},
		/* [8] */
		{110, 270, 126, 338},
		StaticText {
			disabled,
			"run time id"
		},
		/* [9] */
		{130, 170, 146, 260},
		StaticText {
			disabled,
			"Name:"
		},
		/* [10] */
		{130, 270, 146, 338},
		StaticText {
			disabled,
			"run time name"
		},
		/* [11]170*/
		{150, 170, 166, 260},
		StaticText {
			disabled,
			"Size:"
		},
		/* [12] */
		{150, 270, 166, 338},
		StaticText {
			disabled,
			"run time size"
		},
		/* [13] */
		{170, 170, 186, 338},
		CheckBox {
			enabled,
			"Keep Resource"
		}
	}
};

resource 'dlgx' (16050) {
	versionZero {
		kDialogFlagsHandleMovableModal + kDialogFlagsUseThemeControls + kDialogFlagsUseThemeBackground
	}
};

resource 'DLOG' (16050, "LayerFormat", purgeable) {
	{100, 150, 300, 550},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	16050,
	"Layer Format Resource Remover",
	centerParentWindowScreen
};

// end LayerFormatUI.r