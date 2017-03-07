FROM ubuntu:trusty

RUN apt-get update && apt-get install -yy software-properties-common
RUN add-apt-repository 'ppa:ubuntu-toolchain-r/test' && \
	add-apt-repository 'ppa:beineri/opt-qt562-trusty'


RUN apt-get update && apt-get install -yy \
	qt56base qt56script qt56declarative qt56tools qt56x11extras qt56svg \
	ninja-build libedit-dev libxcb-keysyms1-dev libxml2-utils libudev-dev build-essential git mesa-common-dev \
	gettext libz-dev libxrender-dev libxfixes-dev libxcb1-dev libxcb-xfixes0-dev bison flex libstdc++-6-dev g++-6 gcc-6 wget

# build cmake
RUN cd / && wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz && tar xf cmake-3.7.2.tar.gz
RUN cd cmake-3.7.2/ && ./configure --prefix=/usr && make -j`nproc` install
 
