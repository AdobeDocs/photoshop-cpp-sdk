function MakeSwatch( inColor, inName )
{
	var id80 = charIDToTypeID( 'Mk  ' );
	var desc17 = new ActionDescriptor();
	var id81 = charIDToTypeID( 'null' );
	var ref9 = new ActionReference();
	var id82 = charIDToTypeID( 'Clrs' );
	ref9.putClass( id82 );
	desc17.putReference( id81, ref9 );
	var id83 = charIDToTypeID( 'Usng' );
	var desc18 = new ActionDescriptor();
	var id84 = charIDToTypeID( 'Nm  ' );
	desc18.putString( id84, inName );
	var id85 = charIDToTypeID( 'Clr ' );
	var desc19 = new ActionDescriptor();
	var id86 = charIDToTypeID( 'Rd  ' );
	desc19.putDouble( id86, inColor.rgb.red );
	var id87 = charIDToTypeID( 'Grn ' );
	desc19.putDouble( id87, inColor.rgb.green );
	var id88 = charIDToTypeID( 'Bl  ' );
	desc19.putDouble( id88, inColor.rgb.blue );
	var id89 = charIDToTypeID( 'RGBC' );
	desc18.putObject( id85, id89, desc19 );
	var id90 = charIDToTypeID( 'Clrs' );
	desc17.putObject( id83, id90, desc18 );
	executeAction( id80, desc17, DialogModes.NO );
}