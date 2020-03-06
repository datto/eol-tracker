# eol-tracker

eol-tracker is an application that allows you to monitor what package versions different Linux distributions have
available. With this new, powerful information, you can make decisions on what features you can rely on for your project
given a set of target distributions.

It uses [libdnf](https://github.com/rpm-software-management/libdnf) to query contents of RPM repositories and retrieves
the data to present in a web page with timeline information (input by the user) to show the lifetimes of packages and
the versions available of those packages in the distribution.

The web application is powered by Cutelyst and uses PostgreSQL as the backend database.

## Setup

In `dist/eol-tracker.conf`'s `db` section, there are options to customize postgres access.
Location is the hostname/ip address of where the database is located.
Username/password are the database username and password that the service should use to connect.
Database is the name of the database to login to.
Tables will be auto-populated by the first run of the application, so this should be a fresh database if you're not doing a migration.

You should also set the value of `dnf.cacheDir` if you're not happy with the default, and then proceed to make that directory with proper r/w permissions for the user running the app.
This directory is where each of the platforms specified in the app will set up their "chroots".
Each chroot only contains a dnf cache and an extremely basic os-release.

## Building and Running

There's a few dependencies that you'll need:

- cmake, for making makefiles
- g++, for compiling
- libdnf, for reading repos
- distribution-gpg-keys, for the repo keys
- Qt5 (Core, Network, Sql), for cutelyst
- Cutelyst2, for the web framework
- cpptoml, for the config file I/O
- yarn, for the js deps

Building and running is simple after that.
Make will take care of both compiling the C++ and pulling the yarn deps and webpacking them into the frontend js.

```sh
mkdir build; cd build
cmake ..
make
src/eoltrackerweb
```

## Container Setup

The main container is built with the Dockerfile in dist:

```sh
podman build . -f dist/Dockerfile --dns 8.8.8.8 -t eol-tracker
```

There is a kubernetes/podman pod definition in `dist/pod.yaml` for setting up a pod that includes the main eol-tracker image as well as the dependant postgres service.
Note the environment options for the postgres service and the web config volume mount before running.

