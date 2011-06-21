Summary:	simply text adventures/visual novels engine and game
Name:		instead
Version:	1.4.4
Release:	1%{?dist}
License:	GPLv2
URL:		http://instead.googlecode.com
Source0:	%{name}_%{version}.tar.gz
Group:		Amusements/Games
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n) 

BuildRequires: SDL-devel, SDL_mixer-devel, SDL_image-devel, SDL_ttf-devel, lua-devel

%description 
Simply text adventures/visual novels engine and game
Visual novell/text quest-like game in Russian with engine.

%prep
%setup -q
echo -e "2\n\/usr" | ./configure.sh

%build
make

%install
rm -rf $RPM_BUILD_ROOT

make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
#%doc doc/*
%{_bindir}/*
%{_datadir}/%{name}/*
%{_datadir}/applications/*
%{_datadir}/pixmaps/*
%{_datadir}/doc/*
%{_mandir}/*

