
#pragma once

#include <inttypes.h>
#include <time.h>

// https://github.com/Netthaw/TOTP-MCU

namespace totpmcu {
	class TOTPGen {
		static uint8_t* _hmacKey;
		static uint8_t _keyLength;
		static uint8_t _timeZoneOffset;
		static uint32_t _timeStep;

		static uint32_t TimeStruct2Timestamp(struct tm time) noexcept;

	public:
		static void TOTP(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep) noexcept;
		static void setTimezone(uint8_t timezone) noexcept;
		static uint32_t getCodeFromTimestamp(uint32_t timeStamp) noexcept;
		static uint32_t getCodeFromTimeStruct(struct tm time) noexcept;
		static uint32_t getCodeFromSteps(uint32_t steps) noexcept;
	};
}