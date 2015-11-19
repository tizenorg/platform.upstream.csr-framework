%define csr_test_build 0

Name: csr-framework
Summary: A general purpose content screening and reputation solution
Version: 1.1.0
Release: 2
Group: System/Libraries
License: BSD-2.0
URL: http://tizen.org
Source0: %{name}-%{version}.tar.gz
Source1001: %{name}.manifest
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(libtzplatform-config)

%description
csr-framework

%package devel
Summary:    Development files for csr-framework
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
csr-framework (development files)

%if 0%{?csr_test_build}
%package test
Summary:    test program for csr-framework
Group:      Security/Testing
Requires:   %{name} = %{version}-%{release}

%description test
Comaptilibty test program
%endif


%prep
%setup -q
cp -a %SOURCE1001 .


%build
%{!?build_type:%define build_type "Release"}
%cmake . \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DINCLUDEDIR=%{_includedir}/csf \
    -DCMAKE_BUILD_TYPE=%build_type \
%if 0%{?csr_test_build}
    -DCSR_TEST_BUILD=1 \
%endif
    -DVERSION=%{version}

make %{?_smp_mflags}

%install
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


%files
%manifest %name.manifest
%license LICENSE
%{_libdir}/libsecfw.so.*

%files devel
%doc README
%doc doc/
%{_includedir}/csf/TCSErrorCodes.h
%{_includedir}/csf/TCSImpl.h
%{_includedir}/csf/TWPImpl.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libsecfw.so

%if 0%{?csr_test_build}
%files test
%{_bindir}/*
%endif
