Summary: A general purpose content screening and reputation solution
Name: csr-framework
Version: 2.0.0
Release: 0
Source: %{name}-%{version}.tar.gz
License: Apache-2.0 and BSL-1.0
Group: Security/Service
URL: http://tizen.org
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(libsystemd-daemon)
Requires:      lib%{name}-common = %{version}-%{release}
%{?systemd_requires}

%description
General purpose content screening and reputation solution. Can scan
file contents and checking url to prevent malicious items.

%global service_name csr
%global bin_dir      %{_bindir}
%global sbin_dir     /sbin
%global ro_data_dir  %{_datadir}

%package -n lib%{name}-common
Summary: Common library package for %{name}
License: Apache-2.0
Group:   Security/Libraries
Requires: %{sbin_dir}/ldconfig
Requires: %{sbin_dir}/ldconfig

%description -n lib%{name}-common
csr-framework common library package.

%package -n lib%{name}-client
Summary: Client library package for %{name}
License: Apache-2.0
Group:   Security/Libraries
BuildRequires: pkgconfig(capi-base-common)
Requires: %{name} = %{version}-%{release}
Requires: %{sbin_dir}/ldconfig
Requires: %{sbin_dir}/ldconfig

%description -n lib%{name}-client
csr-framework client library package.

%package devel
Summary: Development files for %{name}
LICENSE: Apache-2.0
Group:   Security/Development
BuildRequires: pkgconfig(capi-base-common)
Requires:      %{name} = %{version}-%{release}

%description devel
csr-framework developemnt files including headers and pkgconfig file.

%package test
Summary: test program for %{name}
License: Apache-2.0 and BSL-1.0
Group:   Security/Testing
BuildRequires: boost-devel
Requires:      %{name} = %{version}

%description test
test program of csr-framework

%prep
%setup -q

%build
%cmake . \
    -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE} \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DSERVICE_NAME=%{service_name} \
    -DVERSION=%{version} \
    -DINCLUDE_INSTALL_DIR:PATH=%{_includedir} \
    -DBIN_DIR:PATH=%{bin_dir} \
    -DSYSTEMD_UNIT_DIR=%{_unitdir}

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}%{_unitdir}/multi-user.target.wants
mkdir -p %{buildroot}%{_unitdir}/sockets.target.wants
ln -s ../%{service_name}.service %{buildroot}%{_unitdir}/multi-user.target.wants/%{service_name}.service
ln -s ../%{service_name}.socket %{buildroot}%{_unitdir}/sockets.target.wants/%{service_name}.socket

mkdir -p %{buildroot}%{ro_data_dir}/license
cp LICENSE %{buildroot}%{ro_data_dir}/license/%{name}
cp LICENSE.BSL-1.0 %{buildroot}%{ro_data_dir}/license/%{name}.BSL-1.0
cp LICENSE %{buildroot}%{ro_data_dir}/license/lib%{name}-client
cp LICENSE %{buildroot}%{ro_data_dir}/license/lib%{name}-common
cp LICENSE %{buildroot}%{ro_data_dir}/license/%{name}-test
cp LICENSE.BSL-1.0 %{buildroot}%{ro_data_dir}/license/%{name}-test.BSL-1.0

%post
systemctl daemon-reload
if [ $1 = 1 ]; then
    systemctl start %{service_name}.socket
    systemctl start %{service_name}.service
fi

if [ $1 = 2 ]; then
    systemctl restart %{service_name}.socket
    systemctl restart %{service_name}.service
fi

%preun
if [ $1 = 0 ]; then
    systemctl stop %{service_name}.service
    systemctl stop %{service_name}.socket
fi

%postun
if [ $1 = 0 ]; then
    systemctl daemon-reload
fi

%post -n lib%{name}-common -p %{sbin_dir}/ldconfig
%post -n lib%{name}-client -p %{sbin_dir}/ldconfig
%postun -n lib%{name}-common -p %{sbin_dir}/ldconfig
%postun -n lib%{name}-client -p %{sbin_dir}/ldconfig

%files
%defattr(-,root,root,-)
%manifest %{service_name}.manifest
%{ro_data_dir}/license/%{name}
%{ro_data_dir}/license/%{name}.BSL-1.0
%{bin_dir}/%{service_name}-server
%{_unitdir}/multi-user.target.wants/%{service_name}.service
%{_unitdir}/%{service_name}.service
%{_unitdir}/sockets.target.wants/%{service_name}.socket
%{_unitdir}/%{service_name}.socket

%files -n lib%{name}-common
%defattr(-,root,root,-)
%manifest %{service_name}-common.manifest
%{ro_data_dir}/license/lib%{name}-common
%{_libdir}/lib%{service_name}-common.so.*

%files -n lib%{name}-client
%defattr(-,root,root,-)
%manifest %{service_name}-client.manifest
%{ro_data_dir}/license/lib%{name}-client
%{_libdir}/lib%{service_name}-client.so.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/csr/csr/content-screening.h
%{_includedir}/csr/csr/content-screening-types.h
%{_includedir}/csr/csr/web-protection.h
%{_includedir}/csr/csr/web-protection-types.h
%{_includedir}/csr/csr/error.h
%{_includedir}/csr/csr/engine-manager.h
%{_libdir}/pkgconfig/%{service_name}.pc
%{_libdir}/lib%{service_name}-client.so
%{_libdir}/lib%{service_name}-common.so

%files test
%defattr(-,root,root,-)
%{ro_data_dir}/license/%{name}-test
%{ro_data_dir}/license/%{name}-test.BSL-1.0
%{bin_dir}/%{service_name}-test
