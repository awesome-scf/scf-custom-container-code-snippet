# 使用镜像部署 SCF 云函数 - Python Flask

本文介绍镜像中使用`Python`语言，部署为腾讯云 SCF 云函数的方式。

## 文件组织结构


```
|--  python-flask
|   |-- Dockerfile          构建镜像文件。
|   |-- app.py              Demo 代码。
|   |-- requirements.txt    代码依赖。
```

## 使用 Docker 进行镜像构建

```
# 镜像构建，$YOUR_IMAGE_NAME 请替换为您所使用的镜像地址
docker build -t $YOUR_IMAGE_NAME .

# 切换到文件下载目录
cd /opt

# 下载 Demo ，并进入工作目录
git clone https://github.com/awesome-scf/scf-custom-container-code-snippet.git
cd scf-custom-container-code-snippet/python-flask

# 镜像构建，$YOUR_IMAGE_NAME 请替换为您所使用的镜像地址
docker build -t $YOUR_IMAGE_NAME .

# 本地测试，启动容器，由于云函数当前可写层只读仅tmp可写
docker run -it -p 9000:9000 --read-only -v /tmp:/tmp $YOUR_IMAGE_NAME

# 本地测试，事件函数调用
curl -X POST -H "X-Scf-Request-Id: scf-request-1" 0.0.0.0:9000/event-invoke -d '{"hello": "SCF"}'

#本地测试，Web 函数调用
curl -X POST -H "X-Scf-Request-Id: scf-request-1" 0.0.0.0:9000/web-invoke/python-flask-http -d '{"hello": "SCF"}'

# 登录镜像仓库，$YOUR_REGISTRY_URL请替换为您所使用的镜像仓库，$USERNAME、$PASSWORD分别替换为您的登录凭证
docker login $YOUR_REGISTRY_URL --username $USERNAME --password $PASSWORD

# 镜像推送
docker push $YOUR_IMAGE_NAME
```

