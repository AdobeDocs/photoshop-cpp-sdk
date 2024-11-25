package com.adobe.photoshop.tools
{
	/**
	 * This class abstracts the Tool concept from Photoshop. You can use the definitions of Tools in this class to send to Photoshop, 
	 *  to change the currently selected tool, for example. (See MessageDispatcher, which provides a function to do this.)
	 **/ 
	public class Tool
	{
		public static const TOOLS_ADD_ANCHOR_POINT:String = "addKnotTool";
		public static const TOOLS_ART_HISTORY_BRUSH:String = "artBrushTool";		
		public static const TOOLS_BACKGROUND_ERASER:String = "backgroundEraserTool";
		public static const TOOLS_BLUR:String = "blurTool";
		public static const TOOLS_BRUSH:String = "paintbrushTool";
		public static const TOOLS_BURN:String = "burnInTool";
		public static const TOOLS_CLONE_STAMP:String = "cloneStampTool";
		public static const TOOLS_COLOR_REPLACEMENT:String = "colorReplacementBrushTool";
		public static const TOOLS_COLOR_SAMPLER:String = "colorSamplerTool";
		public static const TOOLS_CONVERT_ANCHOR_POINT:String = "convertKnotTool";
		public static const TOOLS_CUSTOM_SHAPE:String = "customShapeTool";
		public static const TOOLS_CROP:String ="cropTool";
		public static const TOOLS_DELETE_ANCHOR_POINT:String = "deleteKnotTool";
		public static const TOOLS_DIRECT_SELECTION:String = "directSelectTool";
		public static const TOOLS_DODGE:String = "dodgeTool";
		public static const TOOLS_ELLIPSE:String = "ellipseTool";
		public static const TOOLS_ERASER:String = "eraserTool";
		public static const TOOLS_EYEDROPPER:String = "eyedropperTool";
		public static const TOOLS_FREEFORM_PEN:String = "freeformPenTool";
		public static const TOOLS_GRADIENT:String = "gradientTool";
		public static const TOOLS_HAND:String = "handTool";
		public static const TOOLS_HEALING_BRUSH:String = "magicStampTool";
		public static const TOOLS_HISTORY_BRUSH:String = "historyBrushTool";
		public static const TOOLS_HORIZONTAL_TYPE:String = "typeCreateOrEditTool";
		public static const TOOLS_HORIZONTAL_TYPE_MASK:String = "typeCreateMaskTool";
		public static const TOOLS_LASSO:String = "lassoTool";
		public static const TOOLS_LASSO_MAGNETIC:String = "magneticLassoTool";
		public static const TOOLS_LINE:String = "lineTool";
		public static const TOOLS_MAGIC_ERASER:String = "magicEraserTool";
		public static const TOOLS_MAGIC_WAND:String = "magicWandTool";
		public static const TOOLS_MARQUEE_ELLIP:String = "marqueeEllipTool";
		public static const TOOLS_MARQUEE_RECT:String = "marqueeRectTool";
		public static const TOOLS_MARQUEE_SINGLE_COLUMN:String = "marqueeSingleColumnTool";
		public static const TOOLS_MARQUEE_SINGLE_ROW:String = "marqueeSingleRowTool";
		public static const TOOLS_MIXER_BRUSH:String = "wetBrushTool";
		public static const TOOLS_MOVE:String = "moveTool";
		public static const TOOLS_NOTE:String = "textAnnotTool";
		public static const TOOLS_PAINT_BUCKET:String = "bucketTool";
		public static const TOOLS_PATCH:String = "patchSelection";
		public static const TOOLS_PATH_SELECTION:String = "pathComponentSelectTool";
		public static const TOOLS_PATTERN_STAMP:String = "patternStampTool";
		public static const TOOLS_PEN:String = "penTool";
		public static const TOOLS_PENCIL:String = "pencilTool";
		public static const TOOLS_POLY_SELECTION:String = "polySelTool";
		public static const TOOLS_POLYGON:String = "polygonTool";
		public static const TOOLS_QUICK_SELECT:String = "quickSelectTool";
		public static const TOOLS_RECTANGLE:String = "rectangleTool";
		public static const TOOLS_RED_EYE:String = "redEyeTool";
		public static const TOOLS_ROTATE_VIEW:String = "rotateTool";
		public static const TOOLS_ROUNDED_RECTANGLE:String = "roundedRectangleTool";
		public static const TOOLS_RULER:String = "rulerTool"; 
		public static const TOOLS_SHARPEN:String = "sharpenTool";
		public static const TOOLS_SLICE:String = "sliceTool";
		public static const TOOLS_SLICE_SELECT:String = "sliceSelectTool"; 
		public static const TOOLS_SMUDGE:String = "smudgeTool";
		public static const TOOLS_SPONGE:String = "saturationTool";
		public static const TOOLS_SPOT_HEALING_BRUSH:String = "spotHealingBrushTool";
		public static const TOOLS_VERITCAL_TYPE:String = "typeVerticalCreateOrEditTool";
		public static const TOOLS_VERTICAL_TYPE_MASK:String = "typeVerticalCreateMaskTool";		
		public static const TOOLS_ZOOM:String = "zoomTool";
		
		public function Tool()
		{
		}
	}
}