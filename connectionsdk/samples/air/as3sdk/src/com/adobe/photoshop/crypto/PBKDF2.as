/*
* JavaScript implementation of Password-Based Key Derivation Function 2
* (PBKDF2) as defined in RFC 2898.
* Version 1.1 
* Copyright (c) 2007, Parvez Anandam
* parvez.anandam@cern.ch
* http://anandam.name/pbkdf2
*
* Distributed under the BSD license
*
* (Uses Paul Johnston's excellent SHA-1 JavaScript library sha1.js)
* Thanks to Felix Gartsman for pointing out a bug in version 1.0
*/
package com.adobe.photoshop.crypto
{
	import com.adobe.photoshop.crypto.SHA1;

	public class PBKDF2
	{
		/*
		* The four arguments to the constructor of the PBKDF2 object are 
		* the password, salt, number of iterations and number of bytes in
		* generated key. This follows the RFC 2898 definition: PBKDF2 (P, S, c, dkLen)
		*
		* The method deriveKey takes two parameters, both callback functions:
		* the first is used to provide status on the computation, the second
		* is called with the result of the computation (the generated key in hex).
		*
		* Example of use:
		*
		*    <script src="sha1.js"></script>
		*    <script src="pbkdf2.js"></script>
		*    <script>
		*    var mypbkdf2 = new PBKDF2("mypassword", "saltines", 1000, 16);
		*    var status_callback = function(percent_done) {
		*        document.getElementById("status").innerHTML = "Computed " + percent_done + "%"};
		*    var result_callback = function(key) {
		*        document.getElementById("status").innerHTML = "The derived key is: " + key};
		*    mypbkdf2.deriveKey(status_callback, result_callback);
		*    </script>
		*    <div id="status"></div>
		*
		*/
		
		// Remember the password and salt
		private var m_bpassword:Array;
		private var m_salt:String;
		
		// Total number of iterations
		private var m_total_iterations:Number;
		
		// Run iterations in chunks instead of all at once, so as to not block.
		// Define size of chunk here; adjust for slower or faster machines if necessary.
		private var m_iterations_in_chunk:Number;
		
		// Iteration counter
		private var m_iterations_done:Number;
		
		// Key length, as number of bytes
		private var m_key_length:Number;
		
		// The length (number of bytes) of the output of the pseudo-random function.
		// Since HMAC-SHA1 is the standard, and what is used here, it's 20 bytes.
		private var m_hash_length:Number;
		
		// Number of hash-sized blocks in the derived key (called 'l' in RFC2898)
		private var m_total_blocks:Number;
		
		// Start computation with the first block
		private var m_current_block:Number;
		
		// Used in the HMAC-SHA1 computations
		private var m_ipad:Array;
		private var m_opad:Array;
		
		// This is where the result of the iterations gets sotred
		private var m_buffer:Array;
		
		// The result
		private var m_key:String;
		
		// The function to call with the result
		private var m_result_func:Function;
		
		// The function to call with status after computing every chunk
		private var m_status_func:Function;
		
		private var m_hash:Array = new Array();
		
		public function PBKDF2(password:String, salt:String, num_iterations:Number, num_bytes:Number)
		{
			m_bpassword = SHA1.str2binb(password);
			m_salt = salt;
			m_total_iterations = num_iterations;
			m_iterations_in_chunk = 10;
			m_iterations_done = 0;
			m_key_length = num_bytes;
			m_hash_length = 20;
			m_total_blocks = Math.ceil(m_key_length/m_hash_length);
			m_current_block = 1;
			m_ipad = new Array(16);
			m_opad = new Array(16);
			m_buffer = [0x0,0x0,0x0,0x0,0x0];
			m_key = "";
			
			// Set up the HMAC-SHA1 computations			
			if (m_bpassword.length > 16) m_bpassword = SHA1.core_sha1(m_bpassword, password.length * SHA1.chrsz);
			for(var i:Number = 0; i < 16; ++i)
			{
				m_ipad[i] = m_bpassword[i] ^ 0x36363636;
				m_opad[i] = m_bpassword[i] ^ 0x5C5C5C5C;
			}
		}
		
		// Starts the computation
		public function deriveKey(status_callback:Function, result_callback:Function):void
		{
			m_status_func = status_callback;
			m_result_func = result_callback;
			do_PBKDF2_iterations();
		}
		
		
		// The workhorse
		private function do_PBKDF2_iterations():void
		{
			var iterations:Number = m_iterations_in_chunk;
			if (m_total_iterations - m_iterations_done < m_iterations_in_chunk)
				iterations = m_total_iterations - m_iterations_done;
			
			for(var i:Number=0; i<iterations; ++i)
			{
				// compute HMAC-SHA1 
				if (m_iterations_done == 0)
				{
					var salt_block:String = m_salt +
						String.fromCharCode(m_current_block >> 24 & 0xF) +
						String.fromCharCode(m_current_block >> 16 & 0xF) +
						String.fromCharCode(m_current_block >>  8 & 0xF) +
						String.fromCharCode(m_current_block       & 0xF);
					
					m_hash = SHA1.core_sha1(m_ipad.concat(SHA1.str2binb(salt_block)),
						512 + salt_block.length * 8);
					m_hash = SHA1.core_sha1(m_opad.concat(m_hash), 512 + 160);
				}
				else
				{
					m_hash = SHA1.core_sha1(m_ipad.concat(m_hash), 
						512 + m_hash.length * 32);
					m_hash = SHA1.core_sha1(m_opad.concat(m_hash), 512 + 160);
				}
				
				for(var j:Number=0; j<m_hash.length; ++j)
					m_buffer[j] ^= m_hash[j];
				
				m_iterations_done++;
			}
			
			// Call the status callback function
			m_status_func( (m_current_block - 1 + m_iterations_done/m_total_iterations) / m_total_blocks * 100);
			
			if (m_iterations_done < m_total_iterations)
			{
				do_PBKDF2_iterations();
			}
			else
			{
				if (m_current_block < m_total_blocks)
				{
					// Compute the next block (T_i in RFC 2898)
					
					m_key += SHA1.binb2hex(m_buffer);
					
					m_current_block++;
					m_buffer = [0x0,0x0,0x0,0x0,0x0];
					m_iterations_done = 0;
					
					do_PBKDF2_iterations();
				}
				else
				{
					// We've computed the final block T_l; we're done.
					
					var tmp:String = SHA1.binb2hex(m_buffer);
					m_key += tmp.substr(0, (m_key_length - (m_total_blocks - 1) * m_hash_length) * 2 );
					
					// Call the result callback function
					m_result_func(m_key);
				}
			}
		}
	}
}