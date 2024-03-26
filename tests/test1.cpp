#include "../CMakeProject1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../unistd.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
#include "libavutil/dict.h"

//线程阻塞版本
int main() {
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
	utils::downloadHls(hlsUrl, fileName);
	//下载hls文件，但是下载的过程会将当前的线程阻塞掉
	return 0;
}