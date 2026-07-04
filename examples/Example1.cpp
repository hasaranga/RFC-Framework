

// Empty application without high dpi support.

#include "rfc.h"

class MyApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc) noexcept
	{
		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::UNAWARE_MODE);