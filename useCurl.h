#ifndef __USECURL_H__
#define __USECURL_H__
#include "curl/curl.h"

#include <iostream>
#include <vector>
using namespace std;

namespace utils {
	//void download(string urlHttp);
	string GetRedirectUrl(const string& url);
	string GetRedirectUrl2(const string& url, const int& cycleTime);//获取重定向后的url，注意这里有个循环次数
	void  getM3U8WithUrl(const string& url);//通过url获取m3u8文件
	vector<string> printM3U8File(const string& fileName,int &seqBeg); //打印m3u8文件中的内容,同时获取seq的起始值（这个可以用于判断序列号避免重复请求）
	string getUrlWithoutQuery(const string &url);//获取没有query参数和index.m3u8的url
	vector<string> getHLSUrl(const string &url,vector<string> &hlsAddr);//获取所有hls的链接
	void downloadHls(const vector<string>& hlsUrl, const vector<string>& fileName); //通过hls链接将视频下载下来,并将curl_fd作为返回值返回
	vector<string> getFileName(const vector<string>& hlsUrl);//获取hls的文件名
	void printNowInfo(CURL *curl_fd);//打印下载的时候的信息

	int checkRemoteRuleFileExist(const char* URL);//检查url文件是否存在
}

#pragma comment(lib,"libcurl.lib")
#endif // !__USECURL_H__
