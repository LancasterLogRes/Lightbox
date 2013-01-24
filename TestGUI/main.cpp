#include <App.h>
#include "TestGUIApp.h"
using namespace std;
using namespace Lightbox;

#if LIGHTBOX_ANDROID
extern "C" void android_main(struct android_app* _state)
{
	AppEngine e(_state);
	e.setApp(new TestGUIApp);
	e.exec();
}
#elif !defined(LIGHTBOX_CROSS)
int main()
{
	AppEngine e;
	e.setApp(new TestGUIApp);
	e.exec();
	return 0;
}
#endif
