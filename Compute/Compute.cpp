#include <Common/Global.h>
#include "Compute.h"
using namespace std;
using namespace lb;

thread_local ComputeRegistrar* ComputeRegistrar::s_this = nullptr;

