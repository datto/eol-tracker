
FROM fedora:34
WORKDIR /app
RUN dnf install -y dnf-plugins-core && \
    dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/buschmann23:/Cutelyst/Fedora_34/ && \
    rpm --import https://download.opensuse.org/repositories/home:/buschmann23:/Cutelyst/Fedora_34/repodata/repomd.xml.key && \
    dnf update -y && \
    dnf install -y \
        systemd \
        gcc-c++ \
        make \
        cmake \
        nodejs yarnpkg \
        libdnf-devel \
        distribution-gpg-keys \
        cpptoml-devel \
        cutelyst3-qt5-devel \
        qt5-qtbase-postgresql
COPY . .
RUN rm -rf root/static/media && \
    mkdir root/static/media && \ 
    mkdir /var/cache/eol-tracker && \
    rm -rf build && \
    mkdir -p build/conf && \
    cd build && \
    cmake .. -DEOLTRACKER_CONFDIR=/app/build/conf && \
    make -j4
CMD ["/app/build/src/eoltrackerweb"]
