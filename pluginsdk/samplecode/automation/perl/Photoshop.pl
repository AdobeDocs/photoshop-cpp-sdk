# An attempt to use Perl to control Photoshop. 

# Thanks to those noted below we have flushed out some early issues. 
# You still can't tell if the methods fail but that is less likely to be an issue.
# There are still comments in the file of known problems.
# I still hope thatsome Perl expert(s) will review and give me a more robust code.
# Please send me your Perl scripts so I can include them in the next release of the SDK. 
# gapdevsup@adobe.com

# Thanks to Marco Nova for this interesting insight. Photoshop wants a variant passed by reference.
# Thanks to Mike Hollar for showing the $con->{phClassRGBColorMode}

use strict;
use Win32::OLE qw(in with);
use Win32::OLE::Const 'Photoshop';
use Win32::OLE::Variant;

print "DISCLAIMER: Please read this file before reporting errors!\n";

my $phKeySelection = 1718838636;  #'fsel';
my $phEnumPerceptual = 1348825699; #'Perc'; not sure why I need this, it should be in the type library.
my $phKeyForcedColors = 1181901635; #'FrcC' not sure why I need this, it should be in the type library.
my $phTypeForcedColors = $phKeyForcedColors; # not sure why I need this, it should be in the type library.
my $phEnumPrimaries = 1349675373; # 'Prim' not sure why I need this, it should be in the type library.

my $sourceImageFileName = "C:\\Projects\\Files\\Tiff\\Ducky.tif";
my $outputImageFileName = "C:\\Projects\\Files\\Gif\\Ducky.gif";

# You could use || Win32::OLE->new('Photoshop.Application');  to start Photoshop but perl doesn't wait
# until Photoshop is completely started before continuing so I will die for now, if you figure that out let me know

my $Photoshop =  Win32::OLE->GetActiveObject('Photoshop.Application') || Win32::OLE->new('Photoshop.Application') or die "Start Photoshop before running!\n";
my $con = Win32::OLE::Const->Load($Photoshop);
my $control = $Photoshop->MakeControlObject();

$Photoshop->{'Visible'} = 0;
$Photoshop->{'Visible'} = 1;         # if you want to see what's going on

#........Open the source image file...................
# You can do this but when the program exits the doc gets closed and saved automatically!!!
# my $Document = $Photoshop->Open($sourceImageFileName) or die "Can't open $sourceImageFileName\nPlease fix the path(s) and rerun.\n";

psOpenDocument($sourceImageFileName);

my $gaussianRadius = 12;
$gaussianRadius = psGaussianBlur($gaussianRadius);
print "Return Radius: $gaussianRadius\n";

my $width = -1;
my $height = -1;
($width, $height) = psCurrentDocumentSize();
print "Width: $width, Height: $height\n";

psConvertMode($con->{phClassRGBColorMode});

psSaveAsGIF($outputImageFileName);






#---------Photoshop Actions-----------------------

sub psOpenDocument {
	my $fileToOpen = $_[0];
 
	my $descriptor = $Photoshop->MakeDescriptor();
	my $errorString = "";
	my $hasKey = 0;

	$descriptor->PutPath($con->{phKeyNull}, $fileToOpen);

	my $result = $control->Play($con->{phEventOpen}, $descriptor, $con->{phDialogSilent}); # or die "Can't open $fileToOpen.\nPlease fix the path(s) and rerun.\n";

	my $playResult = CheckPlayResult($result);

	if ( $playResult == 0 ) {
		print "Could not open: $fileToOpen\n";
	}
}

sub psGaussianBlur {
	my $radius = $_[0];

    my $descriptor = $Photoshop->MakeDescriptor();
	
	$descriptor->PutDouble ($con->{phKeyRadius}, $radius);	

	my $result  = $control->Play($con->{phEventGaussianBlur}, $descriptor, $con->{phDialogDisplay});

	my $playResult = CheckPlayResult($result);

	my $hasKey = Variant(VT_I4|VT_BYREF, 0);

	my $returnRadius = Variant(VT_R8|VT_BYREF, $radius);
	if ( $playResult == 1 ) {
		$result->HasKey( $con->{phKeyRadius}, $hasKey );
		if ( $hasKey ) {
			$result->GetDouble( $con->{phKeyRadius}, $returnRadius );
		}
	}

	return ($returnRadius);
}

sub psConvertMode {
	my $colorMode = $_[0];

    my $descriptor = $Photoshop->MakeDescriptor();
	
	$descriptor->PutClass ($con->{phKeyTo}, $colorMode);	
	
	my $result  = $control->Play($con->{phEventConvertMode}, $descriptor, $con->{phDialogSilent});

	my $playResult = CheckPlayResult($result);

	if ( $playResult == 1 ) {
		print "Convert Mode: Success\n";
	}

	if ( $playResult == -1 ) {
		print "Convert Mode: Not needed\n";
	}
}

# This doesn't seem to work either. Go figure.
sub psSaveAsGIF {
	my $fileName = $_[0];

    my $descriptor = $Photoshop->MakeDescriptor();
    my $gifDescriptor = $Photoshop->MakeDescriptor();
	my $indexColorDescriptor = $Photoshop->MakeDescriptor();

	$indexColorDescriptor->PutEnumerated($con->{phKeyPalette}, $con->{phTypeColorPalette}, $phEnumPerceptual);
    $indexColorDescriptor->PutInteger($con->{phKeyColors}, 212);
	$indexColorDescriptor->PutEnumerated($phKeyForcedColors, $phTypeForcedColors, $phEnumPrimaries);
    $indexColorDescriptor->PutBoolean($con->{phKeyTransparency}, 1);
	
	$descriptor->PutObject($con->{phKeyTo}, $con->{phClassIndexedColorMode}, $indexColorDescriptor);

	$gifDescriptor->PutBoolean($con->{phKeyInterfaceIconFrameDimmed}, 0);

	$descriptor->PutObject($con->{phKeyAs}, $con->{phClassGIFFormat}, $gifDescriptor);

	$descriptor->PutPath($con->{phKeyIn}, $fileName);
	$descriptor->PutBoolean($con->{phKeyLowerCase}, 1);

	my $result = $control->Play($con->{phEventSave}, $descriptor, $con->{phDialogSilent});

	my $playResult = CheckPlayResult($result);

	if ( $playResult == 1 ) {
		print "Save As GIF: Success\n";
	}
}

# This routine and variations there of thanks to Marco Nova. 
# Photoshop wants a variant passed by reference.
# return 
#	-1 if $result is null, 
#	0 if an error message
#	1 if valid result no error message
sub CheckPlayResult
{
	my( $result ) = @_;

	if( $result == 0 ) {
    	return -1;
   	}

	my $hasKey = Variant(VT_I4|VT_BYREF,0);

	$result->HasKey( $con->{phKeyMessage}, $hasKey );

	if( $hasKey ) {
		my $errorString = Variant(VT_BSTR|VT_BYREF,'');
		$result->GetString($con->{phKeyMessage}, $errorString);
		print "Error: $errorString\n";
		return 0;
   	}

	return 1;
}


# This routine returns the current document width and height
# YES! And I was about to give up. 
# The problem is you need to pass by reference to GetActionProperty. 
# There is no way I know of to create a IActionDescriptor by reference. 
# You need something like: 
# my $result = Variant('IActionDescriptor'|VT_BYREF, 0); 
# which I don't know how to describe
# the solution is phEventGet shown below
sub psCurrentDocumentSize 
{
	my $w=Variant(VT_R8|VT_BYREF, -1);
	my $h=Variant(VT_R8|VT_BYREF, -1);
	my $hasKey = Variant(VT_I4|VT_BYREF, 0);

	my $descriptor=$Photoshop->MakeDescriptor();
	my $reference=$Photoshop->MakeReference();

	$reference->PutEnumerated($con->{phClassDocument}, $con->{phTypeOrdinal}, $con->{phEnumTarget});
	$descriptor->PutReference($con->{phKeyTarget}, $reference);

	my $result = $control->Play($con->{phEventGet}, $descriptor, $con->{phDialogSilent}); # or die "Can't open $fileToOpen.\nPlease fix the path(s) and rerun.\n";
	
	$result->HasKey( $con->{phKeyWidth}, $hasKey );

	if ($hasKey) {
		$result->GetDouble($con->{phKeyWidth}, $w);
	}

	$result->HasKey( $con->{phKeyHeight}, $hasKey );

	if ($hasKey) {
		$result->GetDouble($con->{phKeyHeight}, $h);
	}

	return ($w, $h);
}


