FROM ubuntu:18.04
RUN apt-get update 
RUN apt-get -y install sudo make g++ libreadline-dev gdb lsb-release libc6-dbg libstdc++6-7-dbg bc libpcre3 libpcre3-dev libssl1.1 libssl-dev libbz2-dev libtool automake pkg-config python-ply libcap-dev zlib1g-dbg zlib1g-dev uuid-runtime uuid-dev libexpat1-dev libsasl2-2 libsasl2-dev git vim clang unzip
RUN adduser --disabled-password --gecos '' syspart
RUN echo '%syspart ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER syspart
RUN sudo apt-get update
WORKDIR /home/syspart/
RUN git clone --recursive https://github.com/vidyalakshmir/SysPartArtifact.git
RUN cd SysPartArtifact/binaries && ./make_bind.sh && ./make_httpd.sh && ./make_lighttpd.sh && ./make_memcached.sh && ./make_nginx.sh && ./make_redis.sh && ./set_env.sh
RUN cd SysPartArtifact/analysis/tools/egalito && make -j 8 && cd ../ && tar -xvf pin-3.11-97998-g7ecce2dac-gcc-linux.tar.gz && cd ../app && make
