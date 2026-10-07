# Builds gmsv_file for win32, win64, linux x86 and linux x86_64.
# Use ./build.sh, which exports the binaries to gmsv_file/bin.

FROM debian:trixie-slim AS build

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      ca-certificates curl make upx-ucl \
      gcc-multilib g++-multilib \
      gcc-mingw-w64-i686 g++-mingw-w64-i686 \
      gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 \
 && rm -rf /var/lib/apt/lists/*

ARG PREMAKE_VERSION=5.0.0
ARG PREMAKE_SHA256=8e50e143402de3ce0f0fefe4bb3f4f6a7db46c7d66203dc9f134c0348ebfe6c5
RUN curl -fsSL -o /tmp/premake.tar.gz \
      "https://github.com/premake/premake-core/releases/download/v${PREMAKE_VERSION}/premake-${PREMAKE_VERSION}-linux.tar.gz" \
 && echo "${PREMAKE_SHA256}  /tmp/premake.tar.gz" | sha256sum -c - \
 && tar -xzf /tmp/premake.tar.gz -C /usr/local/bin premake5 \
 && chmod +x /usr/local/bin/premake5 \
 && rm /tmp/premake.tar.gz

WORKDIR /src
COPY include include
COPY gmsv_file/premake5.lua gmsv_file/premake5.lua
COPY gmsv_file/src gmsv_file/src
WORKDIR /src/gmsv_file

# Linux, native toolchain with multilib for x86.
RUN premake5 gmake \
 && make -C project config=x86 \
 && make -C project config=x86_64 \
 && mkdir /out \
 && cp bin/*.dll /out/ \
 && rm -rf bin project

# Windows, cross-compiled with MinGW-w64.
RUN premake5 --os=windows gmake \
 && make -C project config=x86 \
      CC=i686-w64-mingw32-gcc CXX=i686-w64-mingw32-g++ AR=i686-w64-mingw32-ar \
 && make -C project config=x86_64 \
      CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ AR=x86_64-w64-mingw32-ar \
 && cp bin/*.dll /out/

RUN upx --best /out/*.dll

FROM scratch
COPY --from=build /out/ /
