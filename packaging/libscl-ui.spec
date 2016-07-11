%bcond_with x
%bcond_with wayland

Name:       libscl-ui
Summary:    A library for developing XML-based software keyboards
Version:    0.6.26
Release:    1
Group:      Graphics & UI Framework/Input
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(elementary)
%if %{with wayland}
%else
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(x11)
%endif
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(tts)
BuildRequires:  pkgconfig(libscl-common)


%description
A library that helps developing S/W Keyboard

%package devel
Summary:    SCL-UI header file
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
A devel package of libscl-ui library that helps developing S/W Keyboard

%prep
%setup -q


%build
rm -rf CMakeFiles
rm -rf CMakeCache.txt

export CFLAGS+=" -DTIZEN_DEBUG_ENABLE -Werror"
export CXXFLAGS+=" -DTIZEN_DEBUG_ENABLE -Werror"

%if "%{profile}" == "wearable"
CFLAGS+=" -D_WEARABLE";
CXXFLAGS+=" -D_WEARABLE";
%endif

%if "%{profile}" == "mobile"
CFLAGS+=" -D_MOBILE";
CXXFLAGS+=" -D_MOBILE";
%endif

%if "%{profile}" == "tv"
CFLAGS+=" -D_TV";
CXXFLAGS+=" -D_TV";
%endif

%if %{with wayland}
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DLIB_INSTALL_DIR:PATH=%{_libdir} -Dwith_wayland=TRUE
%else
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DLIB_INSTALL_DIR:PATH=%{_libdir}
%endif
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%make_install



%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/%{name}.so
%{_datadir}/%{name}/metadata.xml
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/%{name}.pc
%{_bindir}/xml2binary
