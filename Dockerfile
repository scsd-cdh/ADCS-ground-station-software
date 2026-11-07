# Three stages:
#   dev      toolchain + cpplint, for development (see compose.yaml)
#   build    compiles cpp/ and runs the tests and the cpplint style check
#   runtime  (default) only the compiled contact_state program, plus curl
#
#   docker compose run --rm dev                      # dev shell, repo mounted at /work
#   docker build -t adcs-contact-state .             # deploy image (fails if tests or lint fail)
#   docker run --rm -e ADCS_NORAD_ID=25544 -v adcs-data:/data adcs-contact-state

FROM ubuntu:24.04 AS dev
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      build-essential cmake ninja-build curl ca-certificates python3-pip \
 && pip3 install --no-cache-dir --break-system-packages cpplint==2.0.0 \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /work
CMD ["bash"]

FROM dev AS build
COPY cpp/ /src/cpp/
RUN cmake -S /src/cpp -B /build -G Ninja -DCMAKE_BUILD_TYPE=Release \
 && cmake --build /build \
 && ctest --test-dir /build --output-on-failure

FROM ubuntu:24.04 AS runtime
# curl: CelesTrak requests; libgomp1: OpenMP runtime used by adcs_core.
RUN apt-get update \
 && apt-get install -y --no-install-recommends curl ca-certificates libgomp1 \
 && rm -rf /var/lib/apt/lists/* \
 && useradd --system adcs \
 && mkdir /data \
 && chown adcs /data
COPY --from=build /build/contact_state /usr/local/bin/contact_state
# TLE cache and CelesTrak pull log; keep it on a volume so the 5 pulls/day
# limit holds across container runs.
ENV ADCS_DATA_DIR=/data
VOLUME /data
USER adcs
ENTRYPOINT ["contact_state"]
