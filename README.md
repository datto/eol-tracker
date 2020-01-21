# eol-tracker

eol-tracker is an application that allows you to monitor what package versions different Linux distributions have
available. With this new, powerful information, you can make decisions on what features you can rely on for your project
given a set of target distributions.

It uses [libdnf](https://github.com/rpm-software-management/libdnf) to query contents of RPM repositories and retrieves
the data to present in a web page with timeline information (input by the user) to show the lifetimes of packages and
the versions available of those packages in the distribution.

The web application is powered by Cutelyst and uses PostgreSQL as the backend database.
