#include "../CMakeProject1.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "../unistd.h"
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libavutil/opt.h"
//#include "libswscale/swscale.h"
//#include "libavutil/dict.h"

//¼òµ¥µÄcurl²âÊÔ

int main() {
	const string url1 = "http://www.baidu.com/";
	const string url2 = "http://223.110.241.17:80/wh7f454c46tw2615186739_51562970/ott.mobaibox.com/PLTV/3/224/3221225534/index.m3u8?servicetype=1&unionUrl=cntv%2Flive1%2FCCTV-4%2Fcctv-4&icpid=3&RTS=1708154058&from=38&ocs=2_223.110.241.6_80&hms_devid=414&online=1708154058&vqe=1";
	const string url3 = "http://223.110.241.17:80/wh7f454c46tw2615186739_51562970/ott.mobaibox.com/PLTV/3/224/3221225534/";
	utils::getM3U8WithUrl(url2);
	//int tets = utils::test();
	int beg1;
	auto res = utils::printM3U8File("temp.m3u8",beg1);
	cout << "res is" << endl;
	for (int i = 0; i < res.size(); i++) {
		cout << res[i] << endl;
	}
	string res1 = utils::getUrlWithoutQuery(url2);
//http://223.110.241.17:80/wh7f454c46tw2615186739_51562970/ott.mobaibox.com/PLTV/3/224/3221225534/
	int beg;
	auto m3u8 = utils::printM3U8File("temp.m3u8",beg);
	auto res2 = utils::getHLSUrl(url3, m3u8);
	for (int i = 0; i < res.size(); i++) {
		cout << res2[i] << endl;
	}
}