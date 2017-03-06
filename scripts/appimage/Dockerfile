FROM ubuntu:trusty

RUN apt-get update && apt-get install -yy software-properties-common wget
RUN add-apt-repository 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-4.0 main' && \
	add-apt-repository 'ppa:ubuntu-toolchain-r/test' && \
	add-apt-repository 'ppa:beineri/opt-qt58-trusty' && \
	add-apt-repository 'ppa:presslabs/gitfs' && \
	wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -


RUN apt-get update && apt-get install -yy clang-4.0 llvm-4.0-dev libclang-4.0-dev lldb-4.0 liblldb-4.0-dev \
	qt58base qt58speech qt58script qt58declarative qt58tools qt58x11extras qt58svg \
	ninja-build libedit-dev libxcb-keysyms1-dev libxml2-utils libudev-dev build-essential git mesa-common-dev \
	gettext libz-dev libxrender-dev libxfixes-dev libxcb1-dev libxcb-xfixes0-dev bison flex libstdc++-6-dev

# build cmake
RUN cd / && wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz && tar xf cmake-3.7.2.tar.gz
RUN cd cmake-3.7.2/ && ./configure --prefix=/usr && make -j`nproc` install
