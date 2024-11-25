function DeleteSwatch( i )
{
	var idDlt = charIDToTypeID( 'Dlt ' );
	var desc57 = new ActionDescriptor();
	var idnull = charIDToTypeID( 'null' );
	var ref15 = new ActionReference();
	var idClrs = charIDToTypeID( 'Clrs' );
	ref15.putIndex( idClrs, i );
	desc57.putReference( idnull, ref15 );
	executeAction( idDlt, desc57, DialogModes.NO );
}