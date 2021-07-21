FROM centos:7

ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:$LD_LIBRARY_PATH

WORKDIR /work

COPY . . 
COPY ./include/. /usr/include
COPY ./lib/. /usr/lib

ENTRYPOINT ["./http-server"]