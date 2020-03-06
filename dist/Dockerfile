
FROM fedora:31
WORKDIR /app
RUN dnf install -y dnf-plugins-core && \
    dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/buschmann23:/Cutelyst/Fedora_31/ && \
    rpm --import https://download.opensuse.org/repositories/home:/buschmann23:/Cutelyst/Fedora_31/repodata/repomd.xml.key && \
    dnf update -y && \
    dnf install -y \
        gcc-c++ \
        make \
        cmake \
        yarnpkg \
        libdnf-devel \
        distribution-gpg-keys \
        cpptoml-devel \
        cutelyst2-devel \
        qt5-qtbase-postgresql \
        libCutelyst2Qt5-2-lang \
        libCutelyst2Qt5ViewGrantlee2-lang
COPY . .
RUN rm -rf build && mkdir -p build/conf && cd build && cmake .. -DEOLTRACKER_CONFDIR=/app/build/conf && make -j4
CMD /app/build/src/eoltrackerweb 2>&1
