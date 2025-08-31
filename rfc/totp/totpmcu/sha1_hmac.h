
#pragma once

#include <inttypes.h>

// https://github.com/Netthaw/TOTP-MCU

namespace totpmcu {
	// with HMAC-SHA1 support!
	class SHA1 {
		static uint32_t rol32(uint32_t number, uint8_t bits);
		static void hashBlock();
		static void addUncounted(uint8_t data);
		static void pad();

	public:
		static const int HASH_LENGTH = 20;
		static const int BLOCK_LENGTH = 64;

		static union _buffer {
			uint8_t b[BLOCK_LENGTH];
			uint32_t w[BLOCK_LENGTH / 4];
		} buffer;
		static union _state {
			uint8_t b[HASH_LENGTH];
			uint32_t w[HASH_LENGTH / 4];
		} state;

		static uint8_t bufferOffset;
		static uint32_t byteCount;
		static uint8_t keyBuffer[BLOCK_LENGTH];
		static uint8_t innerHash[HASH_LENGTH];

		static void init(void);
		static void initHmac(const uint8_t* secret, uint8_t secretLength);
		static uint8_t* result(void);
		static uint8_t* resultHmac(void);
		static void write(uint8_t);
		static void writeArray(uint8_t* buffer, uint8_t size);
	};
}