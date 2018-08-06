//
// Created by root on 8/2/18.
//

#ifndef TRACER_CIPHER_H
#define TRACER_CIPHER_H

#include <stdint.h>

#include <stddef.h>

/*
 * A message cipher has two functions :
 *
 * 	- encrypt : computes the encrypted version of a decrypted message;
 * 	- decrypt : computes the decrypted version of an encrypted message;
 */


struct cipher_t {

	//Encrypt the message;
	size_t (*const encrypt)(struct cipher_t *cipher, const void *src, size_t src_size, void *dst, size_t dst_size);

	//Decrypt the message;
	size_t (*const decrypt)(struct cipher_t *cipher, const void *src, size_t src_size, void *dst, size_t dst_size);

	//Destructor;
	void (*const destructor)(struct cipher_t *);

};


inline size_t cipher_encrypt(struct cipher_t *cipher, const void *src, size_t src_size, void *dst, size_t dst_size) {

	(*(cipher->encrypt))(cipher, src, src_size, dst, dst_size);

}

inline size_t cipher_decrypt(struct cipher_t *cipher, const void *src, size_t src_size, void *dst, size_t dst_size) {

	(*(cipher->decrypt))(cipher, src, src_size, dst, dst_size);

}

enum framer_decoding_status_t framer_destruct(struct cipher_t *cipher) {

	(*(cipher->destructor))(cipher);
}

#endif //TRACER_CIPHER_H
