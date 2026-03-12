FROM centos:7

# Switch repos to vault
RUN sed -i 's/^mirrorlist=/#mirrorlist=/g' /etc/yum.repos.d/CentOS-*.repo && \
    sed -i 's|^#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*.repo

# Base build environment
RUN yum -y groupinstall "Development Tools" && \
    yum -y install \
        qt \
        qt-devel \
        qt-x11 \
        openssl-devel \
        which \
        make \
        gcc-c++ \
        git \
        perl \
        python3 \
        tar \
        gzip \
        file \
        pkgconfig && \
    yum clean all && \
    rm -rf /var/cache/yum

WORKDIR /workspace