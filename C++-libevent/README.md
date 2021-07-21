# 基于镜像部署函数 C++ Demo  

# 一、Demo 说明
## 使用场景  
本篇文档详细说明了如何使用镜像方式部署云函数，并附带 C++ 版本的函数 Demo ，该 Demo 包含了如何启动一个 httpserver 、接入 COS C++ 版 SDK 以及 COS SDK 部分 API 的使用。  

<img src="https://main.qcloudimg.com/raw/6e88e132f97be0e4f7bef9e771b0d7dd.png" width="50%" height="50%">  

### Demo 流程  
<img src="https://main.qcloudimg.com/raw/c5669a18fc2557d0c2a0d68477dc13eb.png" width="50%" height="50%">  

### Demo 主体功能
    1.启动 httpserver 监听9000端口  
    2.注册 POST 方法 /event-invoke  
    3.启动 COS 客户端  
    4.handle 方法中实现从 COS 中下载文件，修改文件名，上传至 COS  
    5.提供了 callback 参数解析示例，用户可根据自身框架调用对应接口，实现回调功能。


# 二、操作步骤  
整体步骤包含：创建 COS 桶、开发代码、依赖环境、制作镜像、推送镜像、根据推送镜像部署云函数、云函数功能测试。  

## 1. 创建 COS Bucket 桶  
    1.打开官网：https://cloud.tencent.com/  
    2.产品搜索：COS 或对象存储  
    3.点击立即使用  
    4.点击存储桶列表  
    5.点击创建存储桶  
    6.自定义名称  
    7.选择私有读写  
    8.就近选择地域  
    9.创建好之后就可以点击对应的桶名称，对桶中对象进行操作（上传、下载等）  
    点击查看[官网操作指引文档](https://cloud.tencent.com/document/product/436/11365#null)  

### 存储桶列表
<img src="https://main.qcloudimg.com/raw/77b44855da0496c6a960ff89ac8bccc6.png" width="50%" height="50%">  

### 创建存储桶
<img src="https://main.qcloudimg.com/raw/224e9dd794cbeeb9ee34b4be77e571fb.png" width="50%" height="50%">  

### 上传文件至桶内  
<img src="https://main.qcloudimg.com/raw/659e26d09c1377cacb06c2034822eb08.png" width="50%" height="50%">  


注意： COS 配置文件 config.json 中 Region 配置需跟随桶地域，如成都地域， Region 为 ap-chengdu 。  
  

## 2. 代码

### Demo 主要函数注解，详细内容参见附带 code
```
    // 初始化 COS ，读取配置文件及环境变量获取参数  
    qcloud_cos::CosAPI initCos()  
    //设置 Handle 函数 
    evhttp_set_cb(http,"/event-invoke",httpHandlerPostMsg,(void*)&paramsInfo)  
    // COS 下载对象  
    int downloadFileFromCos(qcloud_cos::CosAPI *cos)  
    // COS 上传对象  
    int uploadFileToCos(qcloud_cos::CosAPI *cos)  
    // localpath  镜像内 cos 下载文件存储地址
    const string localPath = "/tmp/test.txt"
    //监听端口，云函数部署必须实现对9000端口的监听  
    const int PORT = 9000  
```  
  

## 3.环境依赖  
主要依赖 libevent、scons、jsoncpp、openssl、poco、g++、cmake、wget 以及 COS C++SDK  
```
ldd http-server(Demo目标文件)  

linux-vdso.so.1  
/$LIB/libonion.so  
libevent-2.1.so.7  
libpthread.so.0  
libPocoFoundation.so.64  
libPocoNet.so.64  
libPocoNetSSL.so.64  
libcrypto.so.10  
libPocoJSON.so.64  
libstdc++.so.6   
libm.so.6  
libgcc_s.so.1  
libc.so.6  
libdl.so.2  
/lib64/ld-linux-x86-64.so.2  
libz.so.1 => /lib64/libz.so.1  
```
COS C++SDK 地址  
```
git clone https://github.com/tencentyun/cos-cpp-sdk-v5  
```


## 4.制作镜像  
Dockerfile Base 镜像选用 centos:7 ，用户也可根据实际情况选用合适的 Base 镜像。  
```
    FROM centos:7
    ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:$LD_LIBRARY_PATH
    WORKDIR /work
    COPY . . 
    COPY ./include/. /usr/include
    COPY ./lib/. /usr/lib
    ENTRYPOINT ["./http-server"]
``` 
构建镜像命令  
```
    docker build -t [镜像名]:[版本号] .
```  


## 5.推送镜像
推送镜像包含步骤  
1.开通镜像仓库  
2.创建镜像空间  
3.创建镜像(此处创建镜像为控制台概念，以免混淆)  
4.推送镜像到镜像仓库  
详细步骤点击[镜像推送详细指引](https://cloud.tencent.com/document/product/1141/50332)  
  

## 6.镜像部署云函数

### 创建云函数  
1.点击[我的镜像](https://console.cloud.tencent.com/tke2/registry/user/self?rid=1)

2.点击新建  
<image src="https://main.qcloudimg.com/raw/df51b5bff0f47ad4c9f6d263c7a71273.png" width="50%" height="50%">  

3.选择自定义创建  
4.选择函数类型，本 demo 使用 event 函数   
5.自定函数名  
6.勾选地域（与仓库同地域即可）  
<image src="https://main.qcloudimg.com/raw/1fbfe03a0323368e3bda4b8c977d0de0.png" width="50%" height="50%">  
7.点击完成  
<image src="https://main.qcloudimg.com/raw/8c4ae1b9b2b0b01f24dc6b99cc6d1ce2.png" width="50%" height="50%">  

### COS认证信息传入方式
创建环境变量  
该demo中创建了两个环境变量，分别用来传送 COS 的 SecretID 和 SecretKey  
<image src="https://main.qcloudimg.com/raw/8b433a25065946d43ccf30bd939a04ce.png" width="50%" height="50%">  
  


## 7.控制台测试方法
1.点击[函数服务](https://console.cloud.tencent.com/scf/list?rid=1&ns=default)  
2.找到并点击目标函数  
<image src="https://main.qcloudimg.com/raw/aee581f152c2832e94a17a086395fc76.png" width="50%" height="50%">  

3.选择函数代码  
填入测试参数，填写规则如下所示
```
1. 回调服务接口：callBackFunc
2. bucket 名： bucketName  
3. 下载对象名： downloadObjName  
4. 上传对象名： uploadObjName  
```
填写样例，请根据您实际所使用的对象存储和操作目标文件调整示例。
```
{
	"callBackFunc": "call_back_func",
	"bucketName": "20210716test-1253970226",
	"downloadObjName": "test.txt",
	"uploadObjName": "test2.txt"
}
```
<image src="https://main.qcloudimg.com/raw/8efffb5453f44be87b6b6cdd33b40c87.png" width="50%" height="50%">  


4.点击测试按钮   
5..观察返回值，是否成功  
6.从 COS 的控制台中观察桶内文件是否有新增  
<image src="https://main.qcloudimg.com/raw/208f56640ae2c81d0e2ae2d348c9f41f.png" width="50%" height="50%">  


手动测试示例：  
```
curl -H "Content-Type:application/json" -X POST -d'{"callBackFunc":"call_back_func","bucketName":"20210716test-1253970226","downloadObjName":"test.txt", "uploadObjName":"test2.txt"}' 0.0.0.0:9000/event-invoke  
```  
