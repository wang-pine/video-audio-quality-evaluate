#pragma once
#ifndef __TEST2__
#define __TEST2__

#include "../CMakeProject1.h"
#include "../ffmHead.h"
#include "../unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
#include "libavutil/dict.h"
#include <thread>
#include <time.h>
#include <mutex>


//线程不阻塞版本
//主线程负责向命令行打印参数
//辅助线程在后台负责下载等等事情
using namespace std;

static CURL* curl_fd = nullptr;//curl句柄无法多线程共享




class Info
{
public:
	Info(double& size, double& speed) :size(size), speed(speed) {};
	void setSpeed(const double& speed);
	double getSpeed() const;
	void setSize(const double& size);
	double getSize() const;
private:
	volatile double size;//文件大小
	volatile double speed;//下载速度
	mutex mtx;
};



namespace utils {
	bool getNowInfo(CURL* curl_fd, Info& info);
}
//主线程负责显示
void mainThreadFunc(Info &info) {
	//CURL* curl_fd_old = curl_fd;
	//double size = 0;
	//double speed = 0;
	//Info info(size, speed);
	while (1) {
		//utils::getNowInfo(curl_fd_old, info);
		if (curl_fd == nullptr) {
			//cout << "main thread : fd == nullptr" << endl;
			continue;
		}
		cout << "main thread start:" << endl;
		cout << "info\n";
		cout << "speed = " << info.getSpeed() << endl;
		cout << "size = " << info.getSize() << endl;
		Sleep(2);
		//system("cls");
	}
}
//辅助线程负责下载以及参数传递
namespace test2 {
	size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream);
}

void subThreadFunc() {
	const string url = "http://183.207.248.71:80/cntv/live1/CCTV-4/cctv-4";
	string urlRedirect2 = utils::GetRedirectUrl2(url, 2);
	cout << "redirect url is" << urlRedirect2 << endl;
	utils::getM3U8WithUrl(urlRedirect2);
	int seqBeg;
	auto hls = utils::printM3U8File("temp.m3u8", seqBeg);
	//hls还没有去除query参数
	auto fileName = utils::getFileName(hls);
	cout << "filename are:" << endl;
	for (int i = 0; i < fileName.size(); i++) {
		cout << fileName[i] << endl;
	}
	//filename就是去除query参数后的文件名
	cout << "seq begin:" << seqBeg << endl;

	string urlWithoutQuery = utils::getUrlWithoutQuery(urlRedirect2);
	cout << "without query is:" << urlWithoutQuery << endl;
	auto hlsUrl = utils::getHLSUrl(urlWithoutQuery, hls);
	cout << "hls url are:" << endl;
	for (int i = 0; i < hlsUrl.size(); i++) {
		cout << hlsUrl[i] << endl;
	}
	//这里很难返回url，故不返回
	//utils::downloadHls(hlsUrl, fileName, curl_fd);
	//下载hls文件，但是下载的过程会将当前的线程阻塞掉
	//这里将curl的fd传回，但是切记需要解析
	//这里把download函数嵌入即可
	for (int i = 0; i < hlsUrl.size(); i++) {
		if (utils::checkRemoteRuleFileExist(hlsUrl[i].c_str()) == 404) {
			cout << "page " << hlsUrl[i] << "404 not found" << endl;
		}
		//CURL* curl;
		FILE* fp;
		CURLcode res;

		string file = fileName[i];
		curl_fd = curl_easy_init();
		if (curl_fd) {
			fp = fopen(file.c_str(), "wb");
			curl_easy_setopt(curl_fd, CURLOPT_URL, hlsUrl[i].c_str());
			curl_easy_setopt(curl_fd, CURLOPT_WRITEFUNCTION, test2::write_data);
			curl_easy_setopt(curl_fd, CURLOPT_WRITEDATA, fp);

			res = curl_easy_perform(curl_fd);
			
			if (!res) {
				utils::printNowInfo(curl_fd);
			}
			/* always cleanup */
			curl_easy_cleanup(curl_fd);
			fclose(fp);
		}
	}
	cout << "download success" << endl;
}
//次辅助线程2负责参数采集相关
void subSubThreadFunc(Info &info) {
	while (curl_fd == nullptr) {
		//cout << "sub sub :fd == nullptr" << endl;
		continue;
	}
	cout << "sub sub start:" << endl;
	CURL* curl_fd_old = curl_fd;
	while (1) {
		CURL* curl_fd_new = curl_fd;
		if (curl_fd_new != curl_fd_old) {
			curl_fd_old = curl_fd_new;
		}
		if (utils::getNowInfo(curl_fd_old, info)) {
			cout << "info success" << endl;
		}
		Sleep(0.5);
	}
}
int main() {
	double size = 0;
	double speed = 0;

	Info info(size,speed);

	thread th1(mainThreadFunc,ref(info));//慢速刷新
	thread th2(subThreadFunc);//正常下载
	thread th3(subSubThreadFunc, ref(info));//高速采集

	th3.join();
	th2.join();
	th1.join();

	return 0;
}
void Info::setSpeed(const double& speed) {
	if (this->mtx.try_lock()) {
		this->mtx.unlock();
	}
	this->mtx.unlock();
	this->speed = speed;
	this->mtx.lock();
}
double Info::getSpeed() const {
	return this->speed;
}
void Info::setSize(const double& size) {
	if (this->mtx.try_lock()) {
		this->mtx.unlock();
	}
	this->mtx.unlock();
	this->size = size;
	this->mtx.lock();
}
double Info::getSize() const {
	return this->size;
}
bool utils::getNowInfo(CURL* curl_fd, Info& info) {
	double speed;
	CURLcode code = curl_easy_getinfo(curl_fd, CURLINFO_SPEED_DOWNLOAD, &speed);
	if (code != CURLE_OK) {
		cout << "not get speed success" << endl;
		return false;
	}
	info.setSpeed(speed);

	double fileSize;
	code = curl_easy_getinfo(curl_fd, CURLINFO_SIZE_DOWNLOAD, &fileSize);
	if (code != CURLE_OK) {
		cout << "not get size success" << endl;
		return false;
	}
	info.setSize(fileSize);
	return true;
}

size_t test2::write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

#endif // !__TEST2__
