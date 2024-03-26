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

//�߳������汾
int main() {
	const string url = "http://183.207.248.71:80/cntv/live1/CCTV-4/cctv-4";
	string urlRedirect2 = utils::GetRedirectUrl2(url, 2);
	cout << "redirect url is" << urlRedirect2 << endl;
	utils::getM3U8WithUrl(urlRedirect2);
	int seqBeg;
	auto hls = utils::printM3U8File("temp.m3u8", seqBeg);
	//hls��û��ȥ��query����
	auto fileName = utils::getFileName(hls);
	cout << "filename are:" << endl;
	for (int i = 0; i < fileName.size(); i++) {
		cout << fileName[i] << endl;
	}
	//filename����ȥ��query��������ļ���

	cout << "seq begin:" << seqBeg << endl;

	string urlWithoutQuery = utils::getUrlWithoutQuery(urlRedirect2);
	cout << "without query is:" << urlWithoutQuery << endl;
	auto hlsUrl = utils::getHLSUrl(urlWithoutQuery, hls);
	cout << "hls url are:" << endl;
	for (int i = 0; i < hlsUrl.size(); i++) {
		cout << hlsUrl[i] << endl;
	}
	utils::downloadHls(hlsUrl, fileName);
	//����hls�ļ����������صĹ��̻Ὣ��ǰ���߳�������
	return 0;
}