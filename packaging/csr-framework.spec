
Summary: A general purpose content screening and reputation solution
Name: csr-framework
Version: 1.0.0
Release: 1
License: BSD
Group: Security/Libraries
URL: http://tizen.org
Source: %{name}-%{version}.tar.gz

%description
A general purpose content screening and reputation solution. 

%prep
%setup -q

%build 

make all

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_libdir}/
install -D lib/libsecfw.so %{buildroot}%{_libdir}/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files 
%{_libdir}/libsecfw.so

