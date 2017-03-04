FROM ubuntu:trusty

RUN apt-get update && apt-get install -yy software-properties-common wget
RUN add-apt-repository 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-3.9 main' && \
	apt-add-repository 'ppa:beineri/opt-qt58-trusty' && \
	apt-add-repository 'ppa:presslabs/gitfs' && \
	wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -


RUN apt-get update && apt-get install -yy clang-3.9 llvm-3.9-dev libclang-3.9-dev \
	qt58base qt58speech qt58script qt58declarative qt58tools qt58x11extras qt58svg \
	ninja-build libedit-dev libxcb-keysyms1-dev libxml2-utils libudev-dev build-essential git


