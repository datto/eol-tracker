Name:           eol-tracker
Version:        0.0.1
Release:        1%{?dist}
Summary:        Web service for monitoring distro package and dependency versions

License:        LGPLv3+
URL:            https://github.com/datto/eol-tracker
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(systemd)
BuildRequires:  cmake >= 3.5
BuildRequires:  gcc-c++
BuildRequires:  cmake(Cutelyst2Qt5)
BuildRequires:  cmake(Qt5Core)
BuildRequires:  cmake(Qt5Network)
BuildRequires:  cmake(Qt5Sql)
BuildRequires:  cmake(cpptoml)
BuildRequires:  pkgconfig(libdnf) >= 0.39.1
BuildRequires:  %{_bindir}/npx
BuildRequires:  %{_bindir}/yarnpkg

Requires:       distribution-gpg-keys

# plugins that aren't found during build
Requires:       cutelyst2-plugins
Requires:       qt5-qtbase-postgresql

# translation files
Requires:       libCutelyst2Qt5-2-lang
Requires:       libCutelyst2Qt5ViewGrantlee2-lang

%description
eol-tracker is an application that allows you to monitor what package versions
different Linux distributions have available. With this new, powerful information,
you can make decisions on what features you can rely on for your project given a
set of target distributions.

%global webroot %{_datadir}/%{name}

%prep
%autosetup


%build
mkdir -p %{_vpath_builddir}
%cmake -S %{_vpath_srcdir} -B %{_vpath_builddir} -DEOLTRACKER_WEBROOT="%{webroot}" -DEOLTRACKER_CONFDIR="%{_sysconfdir}"
%make_build -C %{_vpath_builddir}


%install
mkdir -p %{buildroot}%{webroot}/static/media
mkdir -p %{buildroot}%{_localstatedir}/cache/%{name}
%make_install -C %{_vpath_builddir}


%files
%license COPYING* LICENSING.md
%doc README.md
%config(noreplace) %{_sysconfdir}/eol-tracker.conf
%{_localstatedir}/cache/%{name}
%{_libexecdir}/eoltrackerweb
%{_libdir}/libeoltracker.so
%{webroot}
%{_unitdir}/eol-tracker.service


%changelog
* Mon Jan 20 2020 Dakota Williams <drwilliams@datto.com>
- Initial packaging
