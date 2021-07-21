
#include "event2/http.h"
#include "event2/event.h"
#include "event2/buffer.h"

#include "cos_api.h"
#include "cos_sys_config.h"
#include "cos_defines.h"

#include <json/json.h>
#include <event.h>
#include <evhttp.h>

#include <string.h>
#include <iostream>
using namespace std;
using namespace qcloud_cos;

// 监听端口号
const int PORT = 9000;
// 缓冲区阈值
const int BUF_MAX = 10000;
// 本地存储路径
const string localPath = "/tmp/test.txt";

// 参数结构
struct ParamsInfo 
{
    qcloud_cos::CosAPI *cos;
};

// 从cos下载文件
int downloadFileFromCos(qcloud_cos::CosAPI *cos, string bucketName, string downloadObjName) 
{
    // 构造下载对象的请求
    std::string bucket_name = bucketName; // Bucket 名称
    std::string object_name = downloadObjName;
    std::string local_path = localPath;

    // request 需要提供 appid、bucketname、object,以及本地的路径（包含文件名）
    qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, local_path);
    qcloud_cos::GetObjectByFileResp resp;

    // 调用下载对象接口
    qcloud_cos::CosResult result = cos->GetObject(req, &resp);

    // 处理调用结果
    if (!result.IsSucc()) 
    {
        // 下载文件失败，可以调用 CosResult 的成员函数输出错误信息，例如 requestID 等
        std::cout << "ErrorInfo=" << result.GetErrorInfo()
        << "｜HttpStatus=" << result.GetHttpStatus() 
        << "｜ErrorCode=" << result.GetErrorCode() 
        << "｜ErrorMsg=" << result.GetErrorMsg()
        << "｜ResourceAddr=" << result.GetResourceAddr()
        << "｜XCosRequestId=" << result.GetXCosRequestId()
        << "｜XCosTraceId=" << result.GetXCosTraceId()
        << std::endl;
        return -1;
    }
    // 下载ok
    cout<< "download file suc!"<<endl;
    return 0;
} 

// 上传文件至cos
int uploadFileToCos(qcloud_cos::CosAPI *cos, string bucketName, string uploadObjName)
{
    // 构造上传文件的请求
    std::string bucket_name = bucketName; // 上传的目的 Bucket 名称
    std::string object_name = uploadObjName;
    // request 的构造函数中需要传入本地文件路径
    qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, localPath);
    req.SetXCosStorageClass("STANDARD_IA"); // 调用 Set 方法设置元数据等
    qcloud_cos::PutObjectByFileResp resp;

    // 调用上传文件接口
    qcloud_cos::CosResult result = cos->PutObject(req, &resp);

    // 处理调用结果
    if (!result.IsSucc()) 
    {
        std::cout << "ErrorInfo=" << result.GetErrorInfo()
        << "｜HttpStatus=" << result.GetHttpStatus() 
        << "｜ErrorCode=" << result.GetErrorCode() 
        << "｜ErrorMsg=" << result.GetErrorMsg()
        << "｜ResourceAddr=" << result.GetResourceAddr()
        << "｜XCosRequestId=" << result.GetXCosRequestId()
        << "｜XCosTraceId=" << result.GetXCosTraceId()
        << std::endl;
        return -1;
    }
    // 上传ok
    cout<< "upload file suc!"<<endl;
    return 0;
}

// 默认回调 测试用
void httpGenericCallback(struct evhttp_request* request, void* arg) 
{
    const struct evhttp_uri* evhttp_uri = evhttp_request_get_evhttp_uri(request);
    if (evhttp_uri == NULL) 
    {
        cout<< "evhttp_request_get_evhttp_uri err"<<endl;
    }
    char url[8192];
    evhttp_uri_join(const_cast<struct evhttp_uri*>(evhttp_uri), url, 8192);

    cout<< "accept request url:" << url <<endl;

    struct evbuffer* evbuf = evbuffer_new();
    if (!evbuf)
    {
        cout<<"create evbuffer failed!"<<endl;
        return ;
    }

    evbuffer_add_printf(evbuf, "Server response. Your request url is %s", url);
    evhttp_send_reply(request, HTTP_OK, "OK", evbuf);
    evbuffer_free(evbuf);
}

// 解析请求数据
void getPostMessage(char *buf, struct evhttp_request *req)
{
    size_t postSize = 0;

    postSize = evbuffer_get_length(req->input_buffer);
    cout<< "post len:" <<postSize<<endl;
    if (postSize <= 0)
    {
        cout<<"post msg empty"<<endl;
        return;
    }
    else
    {
        size_t copyLen = postSize > BUF_MAX ? BUF_MAX : postSize;
        cout<<"copyLen:" << copyLen<<endl;
        memcpy(buf, evbuffer_pullup(req->input_buffer, -1), copyLen);
        buf[postSize] = '\0';
    }
}

// 获取字符串类型参数
string getStringParam(char* buf, string paramName) 
{
    string bufStr(buf);
    Json::Reader  reader;
    Json::Value   value;
    if(!reader.parse(bufStr,value))
    {
        cout<<"json parse failed, buf:" << bufStr <<endl;
        return "";
    }
    if(value[paramName].isNull())
    {
        cout<<"not find "<< paramName <<endl;
        return "";
    }

    if (!value[paramName].isString()) {
        cout<<"not string"<<endl;
        return "";
    }
    return value[paramName].asString();
}

// 回包
void postResp(struct evhttp_request *req, const char* msg, int httpCode)
{
    struct evbuffer *retbuff = NULL;
    retbuff = evbuffer_new();
    if (retbuff == NULL)
    {
        cout<<"retbuff is null"<<endl;
        return;
    }
    evbuffer_add_printf(retbuff, msg);
    evhttp_send_reply(req, httpCode, "Client", retbuff);
    evbuffer_free(retbuff);
}

// 处理post请求
void httpHandlerPostMsg(struct evhttp_request *req, void *arg)
{
    if (req == NULL)
    {
        cout<<"req is null" <<endl;
        return;
    }

    char buf[BUF_MAX] = { 0 };
    getPostMessage(buf, req);
    if (buf == NULL)
    {
        cout<<"getPostMessage buf is null"<<endl;
        return;
    }

    string bucketName = getStringParam(buf, "bucketName");
    string downloadObjName = getStringParam(buf, "downloadObjName");
    string uploadObjName = getStringParam(buf, "uploadObjName");

    // cos操作
    ParamsInfo* params = (ParamsInfo*)arg;
    // 下载
    int ret = downloadFileFromCos(params->cos, bucketName, downloadObjName);
    if (ret != 0) 
    {
        cout<<"downloadFileFromCos faild ret:" << ret <<endl;
        postResp(req, "downloadFileFromCos faild!\n", HTTP_OK);
        return;
    }

    // Todo 对文件某某操作

    // 上传
    ret = uploadFileToCos(params->cos, bucketName, uploadObjName);
    if (ret != 0) 
    {
        cout<<"uploadFileToCos faild ret:" << ret <<endl;
        postResp(req, "uploadFileToCos faild!\n", HTTP_OK);
        return;
    }

    // 回调操作 
    string callBackFunc = getStringParam(buf, "callBackFunc");
    // Todo 调用回调 ->callBackFunc()

    // 回响应
    postResp(req, "Receive post request ok!\n", HTTP_OK);
}

// 初始化cos
qcloud_cos::CosAPI initCos() 
{
    qcloud_cos::CosConfig config("./config.json");

    config.SetAccessKey(getenv("SecretID"));
    config.SetSecretKey(getenv("SecretKey"));

    qcloud_cos::CosAPI cos(config);
    cout<<"[1] cos init ok ..."<<endl;
    return cos;
}

int main(int argc, char** argv)
{
    // 初始化cos
    qcloud_cos::CosAPI cos = initCos();
    struct ParamsInfo paramsInfo = {&cos};

    // 启动httpserver
    struct event_base* base = event_base_new();
    if (!base)
    {
        cout<<"create event_base failed!"<<endl;
        return 1;
    }

    struct evhttp* http = evhttp_new(base);
    if (!http)
    {
        cout<<"create evhttp failed!" <<endl;
        return 1;
    }

    if (evhttp_bind_socket(http, "0.0.0.0", PORT) != 0)
    {
        cout<<"bind socket failed! port:"<<PORT<<endl;
        return 1;
    }

    evhttp_set_cb(http,"/event-invoke",httpHandlerPostMsg,(void*)&paramsInfo);
    cout<<"[2] http server start ..."<<endl;
    event_base_dispatch(base);

    return 0;
}
