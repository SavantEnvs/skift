# syntax=docker/dockerfile:1.7
#
# Commit image for skift (C++26 named modules, cutekit build system).
#
#   DOCKER_BUILDKIT=1 docker build -f mayhem/Dockerfile --build-arg COMMIT=$(git rev-parse HEAD) -t skift .
#
# skift needs clang >= 22 for its C++26 module code, so LLVM 22 comes from apt.llvm.org and
# /usr/lib/llvm-22/bin goes first on PATH (clang/clang++/clang-scan-deps/llvm-ar by name).
# The parsers the harnesses reach live in the externs pinned by project.lock (codeberg
# skift/karm, skift/vaerk, ...); they are cloned once, at the locked commits, into
# /opt/toolchains/deps/skift-externs/src and build.sh points cutekit at that copy, so a
# re-run of build.sh needs no network.
FROM ghcr.io/savantenvs/base:20261023

ARG COMMIT
ARG REPO_URL
ARG SOURCE_DATE_EPOCH
ARG SANITIZER_FLAGS
ARG MAYHEM_JOBS=

USER root
ARG LLVM_VER=22
RUN [ -n "$SOURCE_DATE_EPOCH" ] && repro-sources-list.sh || true ; \
    apt-get update && apt-get install -yq --no-install-recommends \
        ca-certificates curl gnupg git jq ninja-build pkg-config nasm python3-venv \
        liburing-dev libsdl2-dev libsdl3-dev libunwind-dev libseccomp-dev \
 && curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/apt-llvm.gpg \
 && echo "deb [signed-by=/usr/share/keyrings/apt-llvm.gpg] http://apt.llvm.org/trixie/ llvm-toolchain-trixie-${LLVM_VER} main" \
      > /etc/apt/sources.list.d/llvm.list \
 && apt-get update && apt-get install -yq --no-install-recommends \
        clang-${LLVM_VER} clang-tools-${LLVM_VER} llvm-${LLVM_VER} lld-${LLVM_VER} libclang-rt-${LLVM_VER}-dev \
 && rm -rf /var/lib/apt/lists/*
ENV PATH=/usr/lib/llvm-22/bin:/opt/toolchains/cutekit/bin:$PATH

# cutekit (the build system), pinned, in its own venv under /opt/toolchains.
RUN python3 -m venv /opt/toolchains/cutekit \
 && /opt/toolchains/cutekit/bin/pip install --no-cache-dir \
        cutekit==0.12.0 dataclasses-json==0.6.7 graphviz==0.20.3 marshmallow==3.26.2 \
        mypy_extensions==1.2.0 packaging==26.4 tomli_w==1.2.0 typing-inspect==0.9.0 \
        typing_extensions==4.16.0 \
 && chmod -R a+rX /opt/toolchains/cutekit

# The externs, cloned at the commits project.lock pins (what `cutekit model install` would fetch).
COPY project.lock /tmp/skift-project.lock
RUN d=/opt/toolchains/deps/skift-externs/src && mkdir -p "$d" \
 && jq -r '.externs | to_entries[] | "\(.key) \(.value.git) \(.value.commit)"' /tmp/skift-project.lock \
    | while read -r id url commit; do \
        git clone --quiet "$url" "$d/$id" && git -C "$d/$id" checkout --quiet "$commit" \
        && [ "$(git -C "$d/$id" rev-parse HEAD)" = "$commit" ] || exit 1; \
      done \
 && rm /tmp/skift-project.lock && chmod -R a+rX /opt/toolchains/deps \
 && git config --system --add safe.directory '*'

USER mayhem
WORKDIR /mayhem
COPY --chown=2000:2000 . /mayhem

RUN if [ -x /opt/mayhem/ccache/prefill ]; then /opt/mayhem/ccache/prefill bash mayhem/build.sh; \
    else bash mayhem/build.sh; fi

RUN if [ -f mayhem/test.sh ]; then bash mayhem/test.sh; else echo "no mayhem/test.sh"; fi

LABEL org.opencontainers.image.source="${REPO_URL}" \
      org.opencontainers.image.revision="${COMMIT}" \
      rlenv.tier="commit" \
      rlenv.source_date_epoch="${SOURCE_DATE_EPOCH}" \
      rlenv.user="mayhem"

USER mayhem
WORKDIR /mayhem
