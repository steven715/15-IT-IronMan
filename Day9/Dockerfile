FROM centos:7

RUN yum -y update
RUN yum -y install gcc
RUN yum -y install vim
RUN yum -y install perl-IPC-Cmd
RUN yum -y install net-tools

# CentOS devtoolset
RUN yum -y install centos-release-scl
RUN yum -y install devtoolset-7
RUN scl enable devtoolset-7 bash
RUN echo 'source scl_source enable devtoolset-7' >> ~/.bashrc

# CMake Insatll
WORKDIR /usr/local/src
RUN curl -LO https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.tar.gz
RUN tar -xvf cmake-3.22.2-linux-x86_64.tar.gz
RUN mv cmake-3.22.2-linux-x86_64 /usr/local/cmake
RUN echo 'export PATH="/usr/local/cmake/bin:$PATH"' >> ~/.bashrc

# git update version
RUN yum -y remove git
RUN yum -y install https://packages.endpointdev.com/rhel/7/os/x86_64/endpoint-repo.x86_64.rpm
RUN yum -y install git

# vcpkg
WORKDIR /opt
RUN git clone https://github.com/microsoft/vcpkg
RUN ./vcpkg/bootstrap-vcpkg.sh 
RUN echo 'export PATH="/opt/vcpkg:$PATH"' >> ~/.bashrc 
RUN source ~/.bashrc