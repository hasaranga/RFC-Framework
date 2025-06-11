

// Empty application without high dpi support.

#include "rfc/rfc.h"

class MyApplication : public KApplication
{
public:
	int main(wchar_t** argv, int argc)
	{
		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::UNAWARE_MODE);