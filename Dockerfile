FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    libfmt-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*



WORKDIR /app

COPY . .


CMD ["./build/standalone_datalake"]
