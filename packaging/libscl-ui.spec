%bcond_with x
%bcond_with wayland

%define _optdir /opt
%define _appdir %{_optdir}/apps

Name:       libscl-ui
Summary:    A library for developing XML-based software keyboards
Version:    0.6.10
Release:    1
Group:      Graphics & UI Framework/Input
License:    Apache-2.0
Source0:    libscl-ui-%{version}.tar.gz
BuildRequires:  gettext-tools
BuildRequires:  cmake
BuildRequires:  pkgconfig(elementary)
%if %{with wayland}
%else
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
Summary:    SCL header file
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
A devel package of libscl-ui library that helps developing S/W Keyboard

%prep
%setup -q


%build
rm -rf CMakeFiles
rm -rf CMakeCache.txt

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
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%make_install



%post

%postun

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libscl-ui.so
%{_datadir}/libscl-ui/metadata.xml
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/libscl-ui.pc
%{_bindir}/xml2binary
