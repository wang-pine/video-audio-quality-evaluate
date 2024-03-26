// CMakeProject1.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#ifndef __CMAKEPROJECT1_H__
#define __CMAKEPROJECT1_H__

#include <iostream>
#include <fstream>
#include <string>
#include "ffmHead.h"
#include "useCurl.h"

using namespace std;

static string fileURL;

namespace utils {
	int getOS();
	int test();
	///void download(string urlHttp);
}

namespace video {
	void getInfoOfVideo(const string& videoName);
}

#endif // !__CMAKEPROJECT1_H__

