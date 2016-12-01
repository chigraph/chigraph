FROM centos:6.8

RUN yum -y update && yum -y install wget && yum -y install centos-release-scl epel-release && yum -y update && yum -y install devtoolset-4-gcc devtoolset-4-gcc-c++ devtoolset-4-binutils wget tar bzip2 git libtool which fuse fuse-devel libpng-devel automake cppunit-devel cmake glibc-headers libstdc++-devel gcc-c++ freetype-devel fontconfig-devel libxml2-devel libstdc++-devel libXrender-devel patch xcb-util-keysyms-devel libXi-devel libudev-devel.x86_64 openssl-devel sqlite-devel.x86_64 gperftools.x86_64 gperf.x86_64 libicu-devel.x86_64 boost-devel.x86_64 libxslt-devel.x86_64 docbook-style-xsl.noarch python27.x86_64 cmake3.x86_64 ruby bison flex bison-devel ruby-devel flex-devel xz pcre-devel pcre2-devel pcre pcre2 mesa-libEGL-devel mesa-libGL-devel glib-devel gettext perl-URI.noarch bzip2-devel.x86_64 subversion-devel.x86_64 subversion.x86_64 sqlite2-devel.x86_64 vim sudo
RUN cd /tmp && wget wget http://opensource.wandisco.com/rhel/6/svn-1.9/RPMS/x86_64/subversion-1.9.4-3.x86_64.rpm http://opensource.wandisco.com/rhel/6/svn-1.9/RPMS/x86_64/subversion-devel-1.9.4-3.x86_64.rpm http://opensource.wandisco.com/rhel/6/svn-1.9/RPMS/x86_64/serf-1.3.7-1.x86_64.rpm && yum -y install subversion* serf*; rm subversion* serf*

RUN echo ". /opt/rh/devtoolset-4/enable && chmod +x /opt/rh/python27/enable && . /opt/rh/python27/enable" >> /root/.bashrc


ENV LC_ALL=en_US.UTF-8 LANG=en_us.UTF-8 QTVERSION=5.7.0 QVERSION_SHORT=5.7 LLVM_ROOT=/opt/llvm/ QTV=5.7.0 LD_LIBRARY_PATH=$QTDIR/lib/ KF5_VER_SHORT=5.28 KF5_VER_LONG=5.28.0 LLVM_VER=3.9.0
ENV QTDIR=/usr/local/Qt-${QTVERSION}/


# Install llvm 3.9
ADD install_llvmclang.sh /install_llvmclang.sh
RUN bash /install_llvmclang.sh

# Install qt
ADD install_qt5.sh /install_qt5.sh
RUN bash /install_qt5.sh

CMD /bin/bash
