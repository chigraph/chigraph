FROM ubuntu:trusty

RUN apt-get update && apt-get install -yy software-properties-common wget
RUN add-apt-repository 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-3.9 main' && \
	add-apt-repository 'ppa:ubuntu-toolchain-r/test' && \
	add-apt-repository 'ppa:beineri/opt-qt58-trusty' && \
	add-apt-repository 'ppa:presslabs/gitfs' && \
	wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -


RUN apt-get update && apt-get install -yy clang-3.9 llvm-3.9-dev libclang-3.9-dev lldb-3.9 liblldb-3.9-dev \
	qt58base qt58speech qt58script qt58declarative qt58tools qt58x11extras qt58svg \
	ninja-build libedit-dev libxcb-keysyms1-dev libxml2-utils libudev-dev build-essential git mesa-common-dev \
	gettext libz-dev libxrender-dev libxfixes-dev libxcb1-dev libxcb-xfixes0-dev bison flex libstdc++-6-dev \
	libfuse-dev libglib2.0-dev libarchive-dev libcairo-dev zsync gnupg2 dh-autoreconf gcc-6 g++-6 gawk

# build cmake
RUN cd / && wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz && tar xf cmake-3.7.2.tar.gz
RUN cd cmake-3.7.2/ && ./configure --prefix=/usr && make -j`nproc` install

# build patchelf
RUN cd / && wget https://github.com/NixOS/patchelf/archive/0.9.tar.gz -O patchelf.tar.gz && tar xf patchelf.tar.gz
RUN cd /patchelf-0.9 && ./bootstrap.sh && ./configure --prefix=/usr && make -j`nproc` && make install

# build appimagetool
RUN cd / && git clone https://github.com/probonopd/AppImageKit --branch=8 --depth=1
RUN cd /AppImageKit/ && ./build.sh && cp ./build/* /usr/bin
