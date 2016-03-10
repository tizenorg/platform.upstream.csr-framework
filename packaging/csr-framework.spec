%global csr_fw_server_build 0
%global csr_fw_common_Build 0
%global csr_fw_test_build 0

Summary: A general purpose content screening and reputation solution
Name: csr-framework
Version: 2.0.0
Release: 0
Source: %{name}-%{version}.tar.gz
License: BSD-2.0
Group: Security/Service
URL: http://tizen.org
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)

%description
General purpose content screening and reputation solution. Can scan
file contents and checking url to prevent malicious items.

%package -n lib%{name}-client
Summary: Client library package for %{name}
License: BSD-2.0
Group:   Security/Libraries
Requires: %{name} = %{version}-%{release}

%description -n lib%{name}-client
csr-framework client library package.

%package devel
Summary:    Development files for %{name}
Group:      Security/Development
Requires:   %{name} = %{version}

%description devel
csr-framework developemnt files including headers and pkgconfig file.

%if 0%{?csr_fw_test_build}
%package test
Summary:    test program for %{name}
Group:      Security/Testing
Requires:   %{name} = %{version}

%description test
Comaptilibty test program
%endif

%prep
%setup -q

# assign client name as secfw to support backward compatibility
%global client_name secfw
%global bin_dir %{_bindir}

%build
%cmake . \
    -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE} \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
%if 0%{?csr_fw_server_build}
    -DCSR_FW_COMMON_BUILD=1 \
%endif
%if 0%{?csr_fw_server_build}
    -DCSR_FW_SERVER_BUILD=1 \
%endif
%if 0%{?csr_fw_test_build}
    -DCSR_FW_TEST_BUILD=1 \
%endif
    -DSERVICE_NAME=%{name} \
    -DVERSION=%{version} \
    -DINCLUDE_INSTALL_DIR:PATH=%{_includedir} \
    -DBIN_DIR:PATH=%{bin_dir} \
    -DCLIENT_NAME=%{client_name}

make %{?jobs:-j%jobs}

%install
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%license LICENSE
%if 0%{?csr_fw_server_build}
# TODO: list up server files here
%endif
%if 0%{?csr_fw_common_build}
# TODO: list up common library files here
%endif

%files -n lib%{name}-client
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/lib%{client_name}.so.*

%files devel
%doc README
%doc doc/
%{_includedir}/TCSErrorCodes.h
%{_includedir}/TCSImpl.h
%{_includedir}/TWPImpl.h
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/lib%{client_name}.so

%if 0%{?csr_fw_test_build}
%files test
%defattr(-,root,root,-)
%{bin_dir}/csr-test
%endif
