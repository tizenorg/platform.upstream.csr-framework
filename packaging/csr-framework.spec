Summary: A general purpose content screening and reputation solution
Name: csr-framework
Version: 1.0.0
Release: 1

Source: %{name}-%{version}.tar.gz

License: BSD
Group: System/Libraries
URL: http://tizen.org

%description


%prep
%setup -q

%build 

make all

%install
rm -rf %{buildroot}

install -D lib/libsecfw.so %{buildroot}/%{_libdir}/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files 
%{_libdir}/libsecfw.so



