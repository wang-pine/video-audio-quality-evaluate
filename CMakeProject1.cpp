// CMakeProject1.cpp: 定义应用程序的入口点。
//

#include "CMakeProject1.h"
#include <time.h>

int utils::getOS() {
    int res = 0;
#ifdef _WIN32
    std::cout << "Windows\n";
#elif __linux__
    std::cout << "Linux\n";
    res = 1;
#elif __APPLE__
    std::cout << "Apple\n";
    res = 2;
#endif
    return res;
}


string utils::GetRedirectUrl(const string& url) {
    CURL* curl = curl_easy_init();
    if (curl) {
        char* res = NULL;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_perform(curl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        //1表示重定向次数，最多允许一次重定向
        CURLcode code =  curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &res);
        //该函数若返回301，说明是永久重定向；若返回302，说明临时重定向
        //cout << "code is" << code << endl;
        string str(res);
        curl_easy_cleanup(curl);//防止内存泄漏 切记
        return str;
    }
    else {
        return "";
    }
}

string utils::GetRedirectUrl2(const string &url,const int& cycleTime){
    string oldUrl = "";
    string newUrl = url;
    for (int i = 0; i < cycleTime; i++) {
        oldUrl = newUrl;
        newUrl = GetRedirectUrl(oldUrl);
    }
    return newUrl;
}

#define FILENAME "temp.m3u8"

size_t downloadM3U8(void* ptr, size_t size, size_t nmemb, void* stream);

void utils::getM3U8WithUrl(const string& url){
    //cout << "url is:" << url << endl;
    // 初始化 curl 库
    CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
    if (code != CURLE_OK) {
        fprintf(stderr, "初始化失败: %s\n", curl_easy_strerror(code));
        return;
    }

    // 获取 curl 库的版本信息
    curl_version_info_data* data = curl_version_info(CURLVERSION_NOW);
    printf("curl 库的版本: %s\n", data->version);

    // 创建一个 easy 句柄
    CURL* curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "创建句柄失败\n");
        curl_global_cleanup();
        return;
    }

    // 设置 URL
    code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK) {
        fprintf(stderr, "设置 URL 失败: %s\n", curl_easy_strerror(code));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return;
    }


    // 设置写入数据的回调函数
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadM3U8);
    if (code != CURLE_OK) {
        fprintf(stderr, "设置回调函数失败: %s\n", curl_easy_strerror(code));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return;
    }

    // 打开本地文件
    string fileName = FILENAME;
    FILE* file = fopen(fileName.c_str(), "wb");
    if (file == NULL) {
        fprintf(stderr, "打开文件失败: %s\n", fileName.c_str());
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return;
    }

    // 设置写入数据的文件指针
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    if (code != CURLE_OK) {
        fprintf(stderr, "设置文件指针失败: %s\n", curl_easy_strerror(code));
        fclose(file);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return;
    }

    // 执行传输
    code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        fprintf(stderr, "执行传输失败: %s\n", curl_easy_strerror(code));
        fclose(file);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return;
    }

    // 获取响应状态码
    long status;
    code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    if (code == CURLE_OK) {
        printf("响应状态码: %ld\n", status);
    }
    else {
        fprintf(stderr, "获取状态码失败: %s\n", curl_easy_strerror(code));
    }

    // 获取目标 IP 地址
    char* ip;
    code = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
    if (code == CURLE_OK) {
        printf("目标 IP 地址: %s\n", ip);
        //free(ip); // 需要释放字符串指针
    }
    else {
        fprintf(stderr, "获取 IP 地址失败: %s\n", curl_easy_strerror(code));
    }

    // 获取内容类型
    char* type;
    code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &type);
    if (code == CURLE_OK) {
        printf("内容类型: %s\n", type);
        //free(type); // 需要释放字符串指针
    }
    else {
        fprintf(stderr, "获取内容类型失败: %s\n", curl_easy_strerror(code));
    }

    // 关闭文件
    fclose(file);

    // 清理 easy 句柄
    curl_easy_cleanup(curl);

    // 清理全局资源
    curl_global_cleanup();

    return;
}

// 写入数据的回调函数
size_t downloadM3U8(void* ptr, size_t size, size_t nmemb, void* stream) {

    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);

    if (written != size * nmemb) {
        fprintf(stderr, "写入文件出错\n");
        return -1; // 返回一个负值，停止传输
    }

    return written;
}
/**
 * .此处的参考
 * https://cloud.tencent.com/developer/article/2345090
 * curl的基本使用方法
 */

vector<string> utils::printM3U8File(const string& fileName, int& seqBeg){
    ifstream infile;
    vector<string> res;
    infile.open(fileName,ios::in);
    if (!infile.is_open()) {
        cout << "read fail" << endl;
        return res;
    }
    string data;
    while (infile >> data) {
        if (data.find("#EXT-X-MEDIA-SEQUENCE:")!= -1) {
            string beg = data.substr(22);
            seqBeg = stoi(beg);
        }
        if (data[0] == '#') {
            continue;
        }
        //cout << data << endl;
        res.push_back(data);
    }
    return res;
}

string utils::getUrlWithoutQuery(const string& url){
    int end = url.find("index.m3u8");
    string res = url.substr(0, end);
    return res;
}
vector<string> utils::getHLSUrl(const string& url, vector<string>& hlsAddr){
    vector<string> res;
    for (int i = 0; i < hlsAddr.size(); i++) {
        string temp = url + hlsAddr[i];
        res.push_back(temp);
    }
    return res;
}
vector<string> utils::getFileName(const vector<string>& hlsUrl) {
    vector<string> res;
    for (int i = 0; i < hlsUrl.size(); i++) {
        int interrogationPos = hlsUrl[i].find('?');
        string temp = hlsUrl[i].substr(0, interrogationPos);
        res.push_back(temp);
    }
    return res;
}

//判断文件是否存在
int checkRemoteRuleFileExist(const char* URL); 
//给文件写入信息的函数
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream);

void utils::downloadHls(const vector<string>& hlsUrl,const vector<string> &fileName){
    for (int i = 0; i < hlsUrl.size(); i++) {
        clock_t start, end;
        start = clock();
        if (checkRemoteRuleFileExist(hlsUrl[i].c_str()) == 404) {
            cout << "page "<<hlsUrl[i]<<"404 not found" << endl;
        }
        CURL* curl;
        FILE* fp;
        CURLcode res;

        string file = fileName[i];
        curl = curl_easy_init();
        if (curl) {
            fp = fopen(file.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, hlsUrl[i].c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            
            res = curl_easy_perform(curl);
            
            if (!res) {
                utils::printNowInfo(curl);
            }
            /* always cleanup */
            curl_easy_cleanup(curl);
            fclose(fp);
        }
        end = clock();
        cout << "total time '" << i << "':" << (double)(end - start) / CLOCKS_PER_SEC << endl;
    }
    cout << "download success" << endl;
}
//判断文件是否存在
int utils::checkRemoteRuleFileExist(const char* URL) {

    CURL* curl_fd = curl_easy_init();
    CURLcode code = CURLE_OK;
    CURLINFO response_code = CURLINFO_NONE;
    curl_easy_setopt(curl_fd, CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl_fd, CURLOPT_TIMEOUT, 3);
    curl_easy_setopt(curl_fd, CURLOPT_URL, URL);
    code = curl_easy_perform(curl_fd);

    if (code == CURLE_OK) {
        curl_easy_getinfo(curl_fd, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl_fd);

    return response_code;
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void utils::printNowInfo(CURL* curl_fd){
    double speed;
    CURLcode code = curl_easy_getinfo(curl_fd, CURLINFO_SPEED_DOWNLOAD,&speed);
    if (code != CURLE_OK) {
        cout << "not get speed success" << endl;
        return;
    }
    cout << "download speed is:" << speed << endl;

    double fileSize;
    code = curl_easy_getinfo(curl_fd, CURLINFO_SIZE_DOWNLOAD, &fileSize);
    if (code != CURLE_OK) {
        cout << "not get size success" << endl;
        return;
    }
    cout << "download size is:" << fileSize << endl;
    cout << "predict time is:" << fileSize / speed << endl;
}

void print_fps(double d, const char* postfix);
//ts流时长估算
void estimate_timings_from_bit_rate(AVFormatContext* ic);
void video::getInfoOfVideo(const string& videoName){
    int ret;
    int streams;
    char buf[256];
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* avctx = NULL;
    const AVInputFormat* fmt = NULL;

    //设置日志的输出级别
    av_log_set_level(AV_LOG_INFO);
    //打开视频的上下文
    //@1音视频上下文 
    //@2文件路径 
    //@3文件格式(不指定根据文件名判断)
    //@4获取配置项的字典
    ret = avformat_open_input(&fmt_ctx, "./1708314632-1-1695377756.hls.ts", fmt, NULL);
    //cout << "res=" << ret << endl;
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Can't open file:%s\n", ret);
        return ;
    }
    //获取封装格式
    const char* format_name = fmt_ctx->iformat->name;
    //通过上下文获取视频时长
    //cout << "duration is" << fmt_ctx->duration << endl;
    //ts流没有时长信息，因此需要估算
    if (fmt_ctx->duration != AV_NOPTS_VALUE)
    {
        int64_t hours, mins, secs, us;
        int64_t duration = fmt_ctx->duration + (fmt_ctx->duration <= INT64_MAX - 5000 ? 5000 : 0);
        secs = duration / AV_TIME_BASE;
        us = duration % AV_TIME_BASE;
        mins = secs / 60;
        secs %= 60;
        hours = mins / 60;
        mins %= 60;
        av_log(NULL, AV_LOG_INFO, "duration:%02 PRId64:%02 PRId64:%02 PRId64:%02PRId64\n", hours, mins, secs,
            (100 * us) / AV_TIME_BASE);
    }
    else
    {
        av_log(NULL, AV_LOG_INFO, "N/A");
    }
    //查看视频文件中流的个数
    streams = fmt_ctx->nb_streams;
    av_log(NULL, AV_LOG_INFO, "file has:%d streams\n", streams);
    //查看流的类型,判断是视频流还是音频流
    for (int index = 0; index < streams; ++index)
    {
        const AVStream* stream = fmt_ctx->streams[index];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            av_log(NULL, AV_LOG_INFO, "streams %d is Video Stream\n", index);
        }
        else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            av_log(NULL, AV_LOG_INFO, "streams %d is Audio Stream\n", index);
        }
    }
    //获取视频帧率fps
    const AVStream* stream = fmt_ctx->streams[0];
    double fps = av_q2d(stream->avg_frame_rate);
    print_fps(fps, "fps\n");
    //获取流的类型名称(video/audio)
    avctx = avcodec_alloc_context3(NULL);
    ret = avcodec_parameters_to_context(avctx, stream->codecpar);
    const char* codec_type = av_get_media_type_string(avctx->codec_type);
    //获取视频编码类型(h264/h265)
    const char* codec_name = avcodec_get_name(avctx->codec_id);
    av_log(NULL, AV_LOG_INFO, "stream type:%s, stream codec:%s\n", codec_type, codec_name);

    cout << "nb_frames=" << stream->nb_frames << endl;
    //cout << "avg_frame rate" << stream->avg_frame_rate << endl;
    //获得视频的码率
    int bitrate = avctx->bit_rate;
    //av_log(NULL, AV_LOG_INFO, "% kb/s\n", bitrate / 1000);
    cout << "bit rate=" << bitrate / 1000 << endl;
    //获取视频的分辨率
    int width = avctx->width;
    int height = avctx->height;
    av_log(NULL, AV_LOG_INFO, "%d x %d\n", width, height);

    //音频参数
    const AVStream* streamAudio = fmt_ctx->streams[1];
    avctx = avcodec_alloc_context3(NULL);
    ret = avcodec_parameters_to_context(avctx, streamAudio->codecpar);
    const char* codec_type2 = av_get_media_type_string(avctx->codec_type);
    //获取视频编码类型(h264/h265)
    const char* codec_name2 = avcodec_get_name(avctx->codec_id);
    av_log(NULL, AV_LOG_INFO, "stream type:%s, stream codec:%s\n", codec_type2, codec_name2);
    //1

    //av_dump_format(fmt_ctx,0,"./1708314632-1-1695377756.hls.ts",0);
    
    int64_t totalSize = 0;
    int64_t totalDuration = 0;

    // Iterate through packets
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream* stream = fmt_ctx->streams[i];
        //totalSize += stream->nb_frames * stream->avg_frame_rate;
        totalDuration += stream->duration;
    }

    // Calculate bitrate
    //int64_t bitrate = totalSize * 8 / totalDuration;

    //printf("Bitrate: %" PRId64 " bps\n", bitrate);
    cout << "duration=" << totalDuration << endl;
    //11
    avcodec_free_context(&avctx);
    //关闭上下文
    avformat_close_input(&fmt_ctx);
    //getchar();
}
void print_fps(double d, const char* postfix)
{
    uint64_t v = lrintf(d * 100);
    if (!v)
        av_log(NULL, AV_LOG_INFO, " %1.4f %s", d, postfix);
    else if (v % 100)
        av_log(NULL, AV_LOG_INFO, " %3.2f %s", d, postfix);
    else if (v % (100 * 1000))
        av_log(NULL, AV_LOG_INFO, " %1.0f %s", d, postfix);
    else
        av_log(NULL, AV_LOG_INFO, " %1.0fk %s", d / 1000, postfix);
}

//void estimate_timings_from_bit_rate(AVFormatContext* ic)
//{
//    int64_t filesize, duration;
//    int bit_rate, i;
//    AVStream* st;
//
//    /* if bit_rate is already set, we believe it */
//    if (ic->bit_rate <= 0) {
//        bit_rate = 0;
//        for (i = 0; i < ic->nb_streams; i++) {//通过累积各个子流的平均码率得到文件的平均码率
//            st = ic->streams[i];
//            if (st->codec->bit_rate > 0)
//                bit_rate += st->codec->bit_rate;
//        }
//        ic->bit_rate = bit_rate;
//    }
//
//    /* if duration is already set, we believe it */
//    if (ic->duration == AV_NOPTS_VALUE &&
//        ic->bit_rate != 0) {
//        filesize = ic->pb ? avio_size(ic->pb) : 0;
//        if (filesize > 0) {
//            for (i = 0; i < ic->nb_streams; i++) {
//                st = ic->streams[i];
//                duration = av_rescale(8 * filesize, st->time_base.den, ic->bit_rate * (int64_t)st->time_base.num);//通过文件大小除以文件平均码率得到文件时长，之所以还有time_base信息，是因为最后要把秒转换为以time_base为单位的值
//                if (st->duration == AV_NOPTS_VALUE)
//                    st->duration = duration;
//            }
//        }
//    }
//}
