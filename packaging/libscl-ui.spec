%define _optdir /opt
%define _appdir %{_optdir}/apps

Name:       libscl-ui
Summary:    A library for developing XML-based software keyboards
Version:    0.3.9
Release:    1
Group:      TO BE / FILLED IN
License:    TO BE / FILLED IN
Source0:    libscl-ui-%{version}.tar.gz
BuildRequires:  gettext-tools
BuildRequires:  cmake
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(sensor)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(tts)


%description
A library that helps developing S/W Keyboard

%package devel
Summary:    SCL header file
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
A devel pacakge of libscl-ui library that helps developing S/W Keyboard

%prep
%setup -q


%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%make_install



%post

%postun

%files
%defattr(-,root,root,-)
%{_libdir}/libscl-ui.so
%{_datadir}/libscl-ui/metadata.xml

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/libscl-ui.pc
%{_bindir}/xml2binary
