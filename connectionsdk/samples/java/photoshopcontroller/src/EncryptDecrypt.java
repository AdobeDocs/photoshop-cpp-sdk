/**************************************************************************
	ADOBE SYSTEMS INCORPORATED
	 Copyright 2010 Adobe Systems Incorporated
	 All Rights Reserved.

	NOTICE:  Adobe permits you to use, modify, and distribute this file
	in accordance with the terms of the Adobe license agreement accompanying
	it.  If you have received this file from a source other than Adobe, then
	your use, modification, or distribution of it requires the prior written
	permission of Adobe.
**************************************************************************/

/**  Filename: EncryptDecrypt.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/


/** this file is the same as in the AndroidExample except this line */


import javax.crypto.Cipher;
import java.io.IOException;
import java.security.Key;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.BadPaddingException; 
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.spec.DESedeKeySpec;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import java.security.InvalidAlgorithmParameterException;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidKeyException;
import java.io.UnsupportedEncodingException;

/** Messages are encrypted both sending and receiving from Photoshop.
 * This utility class does that work for us.
 * The salt value must match the values used in Photoshop DO NOT CHANGE
 * Enter the same password that the user did in the Connections dialog
 * box in Photoshop
 */
public class EncryptDecrypt {
    private Cipher mECipher;
    private Cipher mDCipher;

    private final byte [] SALT = { 'A', 'd', 'o', 'b', 'e', ' ', 'P', 'h', 'o', 't', 'o', 's', 'h', 'o', 'p' };
    private final int ITERACTIONCOUNT = 1000;
    private final int KEY_LENGTH = 24;

    EncryptDecrypt(String passPhrase) throws InvalidAlgorithmParameterException,
					     NoSuchPaddingException, 
					     NoSuchAlgorithmException, 
					     InvalidKeyException {

       	DESedeKeySpec keySpec = new DESedeKeySpec(PBKDF2.deriveKey( passPhrase.getBytes(), SALT, ITERACTIONCOUNT, KEY_LENGTH ));
       	Key key = new SecretKeySpec(keySpec.getKey(), "DESede");
        mECipher = Cipher.getInstance("DESede/CBC/PKCS5Padding");
        mDCipher = Cipher.getInstance("DESede/CBC/PKCS5Padding");
        IvParameterSpec iv = new IvParameterSpec(new byte[8]);
        mECipher.init(Cipher.ENCRYPT_MODE, key, iv);
        mDCipher.init(Cipher.DECRYPT_MODE, key, iv);
	
    }

    public byte [] encrypt(byte [] inBytes) throws BadPaddingException, 
						   IllegalBlockSizeException,
						   UnsupportedEncodingException {
    	
    	byte[] encryptedBytes = mECipher.doFinal(inBytes);
    	return encryptedBytes;
        
    } 


    public byte [] decrypt(byte [] strBytes) throws BadPaddingException, 
						    IllegalBlockSizeException,
						    UnsupportedEncodingException, 
						    IOException {

    	byte[] utf8 = mDCipher.doFinal(strBytes);
    	return utf8;
    
    }  	

} /* end EncryptDecrypt.java */

