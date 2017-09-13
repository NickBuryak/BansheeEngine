//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Utility/BsUUID.h"
#include "Platform/BsPlatform.h"
#include <chrono>

using namespace std::chrono;

namespace bs
{
	String UUIDGenerator::generateRandom()
	{
		return PlatformUtility::generateUUID();
	}
};