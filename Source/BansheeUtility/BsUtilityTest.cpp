//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Testing/BsFileSystemTestSuite.h"
#include "Testing/BsConsoleTestOutput.h"

using namespace bs;

int main()
{
	SPtr<TestSuite> tests = FileSystemTestSuite::create<FileSystemTestSuite>();
	ConsoleTestOutput testOutput;
	tests->run(testOutput);

	return 0;
}
