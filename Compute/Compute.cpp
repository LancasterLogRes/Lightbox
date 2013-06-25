#include <Common/Global.h>
#include "Compute.h"
using namespace std;
using namespace lb;

thread_local ComputeRegistrar* lb::ComputeRegistrar::s_this = nullptr;

