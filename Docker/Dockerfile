FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Instalacja zależności
RUN apt update && apt install -y \
    build-essential \
    cmake \
    git \
    libgdal-dev \
    gdal-bin \
    libboost-all-dev \
    libjsoncpp-dev \
    libproj-dev \
    && rm -rf /var/lib/apt/lists/*


RUN git clone --depth 1 https://github.com/PDAL/PDAL.git /usr/src/PDAL

WORKDIR /usr/src/PDAL/build

# Konfiguracja CMake
RUN cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DWITH_GDAL=ON \
    -DGDAL_INCLUDE_DIR=/usr/include/gdal \
    -DBUILD_PLUGIN_PCL=OFF \
    -DBUILD_PLUGIN_CPD=OFF \
    -DWITH_TESTS=OFF

# Kompilacja i instalacja
RUN make -j$(nproc)
RUN make install


FROM ubuntu:24.04 AS final_stage

ENV DEBIAN_FRONTEND=noninteractive

# Instalacja runtime dependencies
RUN apt update && apt install -y \
    gdal-bin \
    libjsoncpp25 \
    libproj25 \
    libboost-filesystem1.83.0 \
    libboost-program-options1.83.0 \
    libboost-iostreams1.83.0 \
    && rm -rf /var/lib/apt/lists/*

# Kopiowanie binarek
COPY --from=builder /usr/local/bin/pdal /usr/local/bin/pdal
COPY --from=builder /usr/local/lib/libpdal* /usr/local/lib/

# Linkowanie
RUN ldconfig

# Konfiguracja robocza
WORKDIR /app
COPY classify.json /app/

# Komenda
CMD ["pdal", "pipeline", "classify.json", "--readers.las.filename=data/*.las", "--writers.las.filename=output.las"]