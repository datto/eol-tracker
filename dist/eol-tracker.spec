Name:           eol-tracker
Version:        0.0.1
Release:        1%{?dist}
Summary:        Web service for monitoring distro package and dependency versions

License:        LGPLv3+
URL:            https://github.com/datto/eol-tracker
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz

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

%description
eol-tracker is an application that allows you to monitor what package versions
different Linux distributions have available. With this new, powerful information,
you can make decisions on what features you can rely on for your project given a
set of target distributions.

%prep
%autosetup


%build
mkdir -p %{_vpath_builddir}
%cmake -S %{_vpath_srcdir} -B %{_vpath_builddir}
%make_build -C %{_vpath_builddir}


%install
#make_install -C %{_vpath_builddir}


%files
%license COPYING* LICENSING.md
%doc README.md



%changelog
* Mon Jan 20 2020 Dakota Williams <drwilliams@datto.com>
- Initial packaging
