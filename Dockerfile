FROM python:3.8.0

ENV VER="gcc-arm-none-eabi-9-2019-q4-major"
ENV HASH="bcd840f839d5bf49279638e9f67890b2ef3a7c9c7a9b25271e83ec4ff41d177a"
ENV HASH_SRC="f162a655f222319f75862d7aba9ff8a4a86f752392e4f3c5d9ef2ee8bc13be58"

RUN cd /opt && wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/RC2.1/${VER}-x86_64-linux.tar.bz2
RUN cd /opt && echo "${HASH} ${VER}-x86_64-linux.tar.bz2\n${HASH_SRC} ${VER}-src.tar.bz2" | sha256sum -c --ignore-missing
RUN cd /opt && tar xfj ${VER}-x86_64-linux.tar.bz2

ENV PATH=/opt/$VER/bin:$PATH
ENV LC_ALL=C.UTF-8 LANG=C.UTF-8
