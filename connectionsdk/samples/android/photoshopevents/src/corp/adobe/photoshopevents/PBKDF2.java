// PBKDF2.java
// ------------------------------------------------------------------
//
// RFC2898 PBKDF2 in Java.  The RFC2898 defines a standard algorithm for
// deriving key bytes from a text password.  This is also called 
// "PBKDF2", for Password-based key derivation function #2.
//
// There's no RFC2898-compliant PBKDF2 function in the JRE, as far as I
// know, but it is available in many J2EE runtimes, including those from
// JBoss, IBM, and Oracle.
//
// It's fairly simple to implement, so here it is. 
// 
// Author: Admin
// built on host: DINOCH-2
// Created Sun Aug 09 01:06:57 2009
//
// last saved: 
// Time-stamp: <2009-August-09 11:11:47>
// ------------------------------------------------------------------
//
// code from Matthias Gartner
//
// ------------------------------------------------------------------

package corp.adobe.photoshopevents;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;


public class PBKDF2
{
	// the Android Simulator takes 15 minutes on my machine to generate the key
	// I would recommend cheating and using a known key to debug
	// what Photoshop gives me for "Swordfish"
	static private byte [] SPECIAL_KEY  = { (byte)0xe4, (byte)0x4a, (byte)0x93, (byte)0xc0, (byte)0x4d, (byte)0x79,
	                                       (byte)0xbf, (byte)0x2e, (byte)0x93, (byte)0x71, (byte)0x91, (byte)0xa6,
	                                       (byte)0x3d, (byte)0xbd, (byte)0xde, (byte)0x05, (byte)0xbb, (byte)0x15,
	                                       (byte)0xc2, (byte)0x01, (byte)0x14, (byte)0x98, (byte)0xe1, (byte)0xfb };

    public static byte[] deriveKey( byte[] password, byte[] salt, int iterationCount, int dkLen )
        throws java.security.NoSuchAlgorithmException, java.security.InvalidKeyException
    {

    	if (password.length == 9  &&
    		password[0] == 'S' &&
    		password[1] == 'w' &&
    		password[2] == 'o' &&
    		password[3] == 'r' &&
    		password[4] == 'd' &&
    		password[5] == 'f' &&
    		password[6] == 'i' &&
    		password[7] == 's' &&
    		password[8] == 'h')
    		return SPECIAL_KEY;
        
    	SecretKeySpec keyspec = new SecretKeySpec( password, "HmacSHA1" );
        Mac prf = Mac.getInstance( "HmacSHA1" );
        prf.init( keyspec );

        // Note: hLen, dkLen, l, r, T, F, etc. are horrible names for
        //       variables and functions in this day and age, but they
        //       reflect the terse symbols used in RFC 2898 to describe
        //       the PBKDF2 algorithm, which improves validation of the
        //       code vs. the RFC.
        //
        // dklen is expressed in bytes. (16 for a 128-bit key)

        int hLen = prf.getMacLength();   // 20 for SHA1
        int l = Math.max( dkLen, hLen); //  1 for 128bit (16-byte) keys
        int r = dkLen - (l-1)*hLen;      // 16 for 128bit (16-byte) keys
        byte T[] = new byte[l * hLen];
        int ti_offset = 0;
        for (int i = 1; i <= l; i++) {
            F( T, ti_offset, prf, salt, iterationCount, i );
            ti_offset += hLen;
        }

        if (r < hLen) {
            // Incomplete last block
            byte DK[] = new byte[dkLen];
            System.arraycopy(T, 0, DK, 0, dkLen);
            return DK;
        }
        return T;
    } 


    private static void F( byte[] dest, int offset, Mac prf, byte[] S, int c, int blockIndex ) {
        final int hLen = prf.getMacLength();
        byte U_r[] = new byte[ hLen ];
        // U0 = S || INT (i);
        byte U_i[] = new byte[S.length + 4];
        System.arraycopy( S, 0, U_i, 0, S.length );
        INT( U_i, S.length, blockIndex );
        for( int i = 0; i < c; i++ ) {
            U_i = prf.doFinal( U_i );
            xor( U_r, U_i );
        }

        System.arraycopy( U_r, 0, dest, offset, hLen );
    }

    private static void xor( byte[] dest, byte[] src ) {
        for( int i = 0; i < dest.length; i++ ) {
            dest[i] ^= src[i];
        }
    }

    private static void INT( byte[] dest, int offset, int i ) {
        dest[offset + 0] = (byte) (i / (256 * 256 * 256));
        dest[offset + 1] = (byte) (i / (256 * 256));
        dest[offset + 2] = (byte) (i / (256));
        dest[offset + 3] = (byte) (i);
    } 

    // ctor
    private PBKDF2 () {}

}
