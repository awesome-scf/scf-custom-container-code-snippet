# 使用镜像部署 SCF 云函数 - Springboot Demo

本文介绍镜像中使用`JAVA`语言，部署为腾讯云 SCF 云函数的方式。



## 下载代码本地测试

```
# 切换到文件下载目录
$cd /opt

# 下载 Demo ，并进入工作目录(无git安装：yum -y install git)
git clone https://github.com/awesome-scf/scf-custom-container-code-snippet.git
cd scf-custom-container-code-snippet/springbootDemo

# 使用项目中mvnw打包
./mvnw clean package

# 启动应用（本地测试：curl localhost:9000）
java -jar target/springboot-demo-0.0.1-SNAPSHOT.jar
```
## 使用Docker进行镜像构建

```
# 镜像构建
./mvnw install dockerfile:build

# 查询镜像
docker image ls

# 启动容器，由于云函数当前可写层只读仅tmp可写
docker run -it -p 9000:9000 --read-only -v /tmp:/tmp $YOUR_IMAGE_NAME

# 本地测试，Web 函数调用
curl -X POST -H "X-Scf-Request-Id: scf-request-1" 0.0.0.0:9000 -d ''
```

## 推送镜像到容器镜像服务（请参考镜像仓库中具体镜像的快捷指令）

```
# 创建容器镜像服务（参考：https://cloud.tencent.com/document/product/1141/39287）

# 登录镜像仓库，$YOUR_REGISTRY_URL请替换为您所使用的镜像仓库，$USERNAME、$PASSWORD分别替换为您的登录凭证
docker login $YOUR_REGISTRY_URL --username $USERNAME --password $PASSWORD

# 镜像推送
docker tag $YOUR_IMAGE_NAME $YOUR_REGISTRY_URL/$NAMESPACE/$REMOTE_IMAGE_NAME
docker push $YOUR_REGISTRY_URL/$NAMESPACE/$REMOTE_IMAGE_NAME
```
## mvnw/docker加速

```
# 设置mvnw加速
    ./mvnw -v
    Maven home中 conf/settings.xml 文件中的 mirrors 标签中添加 mirror 子节点：
    <mirror>
        <id>tencent-cloud</id>
        <mirrorOf>central</mirrorOf>
        <url>http://mirrors.tencentyun.com/nexus/repository/maven-public/</url>
    </mirror>
    * 公网访问地址：http://mirrors.cloud.tencent.com/
    * 内网访问地址：http://mirrors.tencentyun.com/

# docker加速（无docker安装：https://cloud.tencent.com/document/product/213/46000）
    sudo systemctl start docker
    vi /etc/docker/daemon.json ： 
	{
        "registry-mirrors": [
        "https://mirror.ccs.tencentyun.com"
        ]
    }
    sudo systemctl daemon-reload
    sudo systemctl restart docker 
    docker info