# build-env stage
ARG UBUNTU_VERSION
FROM ubuntu:$UBUNTU_VERSION as build-env

MAINTAINER sadjad "https://github.com/sadjad"

RUN apt-get update -qq
RUN apt-get install -y -q gcc-7 g++-7 libcap-dev libncurses5-dev \
                          libboost-dev libssl-dev autopoint help2man texinfo \
                          python3 python3-pip libhiredis-dev protobuf-compiler \
                          git libprotobuf-dev libcrypto++-dev texinfo automake \
                          libtool pkg-config python-minimal

RUN pip3 install python_magic
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 99
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 99

RUN mkdir /app
COPY . /app/gg

ENV LANG C.UTF-8
ENV LANGUAGE C:en
ENV LC_ALL C.UTF-8

WORKDIR /app/gg

# dev-env stage
FROM build-env as dev-env

RUN pip3 install boto3 numpy python_magic
RUN apt-get install -y -q vim wget unzip cmake ffmpeg
