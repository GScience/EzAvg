#include "eaApplication.h"
#include <functional>
using namespace std;

int main(int argc, char* argv[])
{
	eaApplication app;

	vector<string> args;
	for (int i = 1; i < argc; ++i)
		args.push_back(argv[i]);

#ifndef _DEBUG
	try
	{
		app.Run(args);
	}
	catch (exception e)
	{
		return -1;
	}
#else
	app.Run(args);
#endif
}