

// Empty application without high dpi support.

#include "rfc/rfc.h"

class MyApplication : public KApplication
{
public:
	int Main(KString** argv, int argc)
	{
		return 0;
	}
};

START_RFC_APPLICATION(MyApplication, KDPIAwareness::UNAWARE_MODE);