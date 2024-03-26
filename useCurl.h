#ifndef __USECURL_H__
#define __USECURL_H__
#include "curl/curl.h"

#include <iostream>
#include <vector>
using namespace std;

namespace utils {
	//void download(string urlHttp);
	string GetRedirectUrl(const string& url);
	string GetRedirectUrl2(const string& url, const int& cycleTime);//��ȡ�ض�����url��ע�������и�ѭ������
	void  getM3U8WithUrl(const string& url);//ͨ��url��ȡm3u8�ļ�
	vector<string> printM3U8File(const string& fileName,int &seqBeg); //��ӡm3u8�ļ��е�����,ͬʱ��ȡseq����ʼֵ��������������ж����кű����ظ�����
	string getUrlWithoutQuery(const string &url);//��ȡû��query������index.m3u8��url
	vector<string> getHLSUrl(const string &url,vector<string> &hlsAddr);//��ȡ����hls������
	void downloadHls(const vector<string>& hlsUrl, const vector<string>& fileName); //ͨ��hls���ӽ���Ƶ��������,����curl_fd��Ϊ����ֵ����
	vector<string> getFileName(const vector<string>& hlsUrl);//��ȡhls���ļ���
	void printNowInfo(CURL *curl_fd);//��ӡ���ص�ʱ�����Ϣ

	int checkRemoteRuleFileExist(const char* URL);//���url�ļ��Ƿ����
}

#pragma comment(lib,"libcurl.lib")
#endif // !__USECURL_H__
