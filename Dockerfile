from ubuntu:latest


RUN apt-get -y update                   && \
    apt-get -y install libnl-3-dev      && \
    apt-get -y install libnl-genl-3-dev && \
    apt-get -y install gcc              && \
    apt-get -y install make             && \
    apt-get -y install pkg-config

ADD Makefile /projekt/
ADD mynl.cpp /projekt/

WORKDIR /projekt/