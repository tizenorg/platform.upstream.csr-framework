
Summary:       Content Security Framework
Name:          csr-framework
Version:       1.0.0
Release:       0
License:       BSD-3-Clause
Group:         Security/Libraries
URL:           http://tizen.org
Source:        %{name}-%{version}.tar.gz
Source1001:    csr-framework.manifest
BuildRequires: pkgconfig(libtzplatform-config)

%description
A general purpose content screening and reputation solution. 

%prep
%setup -q
cp %{SOURCE1001} .

%build 
make all

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_libdir}/
install -D lib/libsecfw.so %{buildroot}%{_libdir}/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files 
%manifest %{name}.manifest
%{_libdir}/libsecfw.so

