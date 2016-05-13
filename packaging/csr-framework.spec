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
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(libsmack)
Requires:      lib%{name}-common = %{version}-%{release}
%{?systemd_requires}

%description
General purpose content screening and reputation solution. Can scan
file contents and checking url to prevent malicious items.

%global service_user                 system
%global service_group                system
%global service_name                 csr
%global bin_dir                      %{_bindir}
%global sbin_dir                     /sbin
%global ro_data_dir                  %{_datadir}
%global rw_data_dir                  /opt/share
%global ro_db_dir                    %{ro_data_dir}/%{service_name}/dbspace
%global rw_db_dir                    %{rw_data_dir}/%{service_name}/dbspace
%global sample_engine_ro_res_dir     %{ro_data_dir}/%{service_name}/engine
%global sample_engine_rw_working_dir %{rw_data_dir}/%{service_name}/engine
%global sample_engine_dir            %{_libdir}
%global test_dir                     %{rw_data_dir}/%{service_name}-test

%package -n lib%{name}-common
Summary: Common library package for %{name}
License: Apache-2.0
Group:   Security/Libraries
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

%description -n lib%{name}-client
csr-framework client library package.

%package devel
Summary: Development files for %{name}
LICENSE: Apache-2.0
Group:   Security/Development
BuildRequires: pkgconfig(capi-base-common)
Requires:      %{name} = %{version}-%{release}

%description devel
csr-framework development files including headers and pkgconfig file.

%package engine-devel
Summary: Development files for %{name} engine
LICENSE: Apache-2.0
Group:   Security/Development

%description engine-devel
csr-framework engine development files including headers and pkgconfig file.

%package test
Summary: test program for %{name}
License: Apache-2.0 and BSL-1.0
Group:   Security/Testing
BuildRequires: boost-devel
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(glib-2.0)
Requires:      %{name} = %{version}

%description test
test program of csr-framework

%prep
%setup -q

%build

# define build architecture
%ifarch %{ix86}
%define test_target emulator
%else
%define test_target target
%endif

%cmake . \
    -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE} \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DSERVICE_USER=%{service_user} \
    -DSERVICE_GROUP=%{service_group} \
    -DSERVICE_NAME=%{service_name} \
    -DVERSION=%{version} \
    -DINCLUDE_INSTALL_DIR:PATH=%{_includedir} \
    -DBIN_DIR:PATH=%{bin_dir} \
    -DSYSTEMD_UNIT_DIR=%{_unitdir} \
    -DSYSTEMD_UNIT_USER_DIR=%{_unitdir_user} \
    -DRO_DBSPACE:PATH=%{ro_db_dir} \
    -DRW_DBSPACE:PATH=%{rw_db_dir} \
    -DSAMPLE_ENGINE_RO_RES_DIR:PATH=%{sample_engine_ro_res_dir} \
    -DSAMPLE_ENGINE_RW_WORKING_DIR:PATH=%{sample_engine_rw_working_dir} \
    -DSAMPLE_ENGINE_DIR:PATH=%{sample_engine_dir} \
    -DTEST_TARGET=%{test_target} \
    -DTEST_DIR:PATH=%{test_dir}

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}%{_unitdir}/multi-user.target.wants
mkdir -p %{buildroot}%{_unitdir}/sockets.target.wants
ln -s ../%{service_name}.service %{buildroot}%{_unitdir}/multi-user.target.wants/%{service_name}.service
ln -s ../%{service_name}-cs.socket %{buildroot}%{_unitdir}/sockets.target.wants/%{service_name}-cs.socket
ln -s ../%{service_name}-wp.socket %{buildroot}%{_unitdir}/sockets.target.wants/%{service_name}-wp.socket
ln -s ../%{service_name}-admin.socket %{buildroot}%{_unitdir}/sockets.target.wants/%{service_name}-admin.socket
ln -s ../%{service_name}-popup.socket %{buildroot}%{_unitdir}/sockets.target.wants/%{service_name}-popup.socket

mkdir -p %{buildroot}%{ro_data_dir}/license
cp LICENSE %{buildroot}%{ro_data_dir}/license/%{name}
cp LICENSE.BSL-1.0 %{buildroot}%{ro_data_dir}/license/%{name}.BSL-1.0
cp LICENSE %{buildroot}%{ro_data_dir}/license/lib%{name}-client
cp LICENSE %{buildroot}%{ro_data_dir}/license/lib%{name}-common
cp LICENSE %{buildroot}%{ro_data_dir}/license/%{name}-test
cp LICENSE.BSL-1.0 %{buildroot}%{ro_data_dir}/license/%{name}-test.BSL-1.0

mkdir -p %{buildroot}%{ro_db_dir}
mkdir -p %{buildroot}%{rw_db_dir}
mkdir -p %{buildroot}%{sample_engine_ro_res_dir}
cp data/scripts/*.sql %{buildroot}%{ro_db_dir}

%post
systemctl daemon-reload
if [ $1 = 1 ]; then
    systemctl start %{service_name}-cs.socket
    systemctl start %{service_name}-wp.socket
    systemctl start %{service_name}-admin.socket
    systemctl start %{service_name}-popup.socket
    systemctl start %{service_name}.service
fi

if [ $1 = 2 ]; then
    systemctl restart %{service_name}-cs.socket
    systemctl restart %{service_name}-wp.socket
    systemctl restart %{service_name}-admin.socket
    systemctl restart %{service_name}-popup.socket
    systemctl restart %{service_name}.service
fi

%preun
if [ $1 = 0 ]; then
    systemctl stop %{service_name}.service
    systemctl stop %{service_name}-cs.socket
    systemctl stop %{service_name}-wp.socket
    systemctl stop %{service_name}-admin.socket
    systemctl stop %{service_name}-popup.socket
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
%{bin_dir}/%{service_name}-popup
%{_unitdir}/multi-user.target.wants/%{service_name}.service
%{_unitdir}/%{service_name}.service
%{_unitdir}/%{service_name}-popup.service
%{_unitdir}/sockets.target.wants/%{service_name}-cs.socket
%{_unitdir}/sockets.target.wants/%{service_name}-wp.socket
%{_unitdir}/sockets.target.wants/%{service_name}-admin.socket
%{_unitdir}/sockets.target.wants/%{service_name}-popup.socket
%{_unitdir}/%{service_name}-cs.socket
%{_unitdir}/%{service_name}-wp.socket
%{_unitdir}/%{service_name}-admin.socket
%{_unitdir}/%{service_name}-popup.socket

%dir %{ro_data_dir}/%{service_name}
%dir %attr(-, %{service_user}, %{service_group}) %{rw_data_dir}/%{service_name}
%dir %{ro_db_dir}
%dir %attr(-, %{service_user}, %{service_group}) %{rw_db_dir}
%attr(444, %{service_user}, %{service_group}) %{ro_db_dir}/*.sql

# sample engine related files
%dir %{sample_engine_dir}
%dir %{sample_engine_ro_res_dir}
%dir %attr(775, %{service_user}, %{service_group}) %{sample_engine_rw_working_dir}
%{sample_engine_dir}/lib%{service_name}-cs-engine.so
%{sample_engine_dir}/lib%{service_name}-wp-engine.so
%attr(-, %{service_user}, %{service_group}) %{sample_engine_rw_working_dir}/*

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
%{_includedir}/csr/content-screening.h
%{_includedir}/csr/content-screening-types.h
%{_includedir}/csr/web-protection.h
%{_includedir}/csr/web-protection-types.h
%{_includedir}/csr/error.h
%{_includedir}/csr/engine-manager.h
%{_libdir}/pkgconfig/%{service_name}.pc
%{_libdir}/lib%{service_name}-client.so
%{_libdir}/lib%{service_name}-common.so

%files engine-devel
%defattr(-,root,root,-)
%{_includedir}/csre/content-screening-engine-info.h
%{_includedir}/csre/content-screening.h
%{_includedir}/csre/content-screening-types.h
%{_includedir}/csre/error.h
%{_includedir}/csre/web-protection-engine-info.h
%{_includedir}/csre/web-protection.h
%{_includedir}/csre/web-protection-types.h
%{_libdir}/pkgconfig/%{service_name}-engine.pc

%files test
%defattr(-,root,root,-)
%manifest %{service_name}-test.manifest
%{ro_data_dir}/license/%{name}-test
%{ro_data_dir}/license/%{name}-test.BSL-1.0
%attr(-, %{service_user}, %{service_group}) %{bin_dir}/%{service_name}-test
%attr(-, %{service_user}, %{service_group}) %{bin_dir}/%{service_name}-internal-test
%attr(-, %{service_user}, %{service_group}) %{bin_dir}/%{service_name}-popup-test
%attr(-, %{service_user}, %{service_group}) %{bin_dir}/%{service_name}-threadpool-test
# test resources
%dir %attr(-, %{service_user}, %{service_group}) %{test_dir}
%attr(-, %{service_user}, %{service_group}) %{test_dir}/*
