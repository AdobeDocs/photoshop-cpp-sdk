function SubscribeToEvent( inEvent )
{
	var idNS = stringIDToTypeID( 'networkEventSubscribe' );
	var desc1 = new ActionDescriptor();
	desc1.putClass( stringIDToTypeID( 'eventIDAttr' ), stringIDToTypeID( inEvent ) );
	executeAction( idNS, desc1, DialogModes.NO );
}