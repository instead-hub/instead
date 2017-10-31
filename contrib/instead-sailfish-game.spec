Summary:	Color Lines game
Name:		instead-ilines
Version:	1.1
Release:	1%{?dist}
License:	GPL
URL:		http://instead-games.ru
Source0:	%{name}-%{version}.zip
Group:		Amusements/Games
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:	noarch
Requires: instead

%description
Classic Color Lines game

%prep
cd %{_builddir}
b=`echo %{name} | sed -e 's/instead-//g'`
rm -rf %{name}
unzip %{name}-%{version}.zip
mv "$b" %{name}

%setup

%build

%install
rm -rf $RPM_BUILD_ROOT
install -d -m 0755 $RPM_BUILD_ROOT/usr/share/instead/games/%{name}
cd %{name}
tar cf - . | (cd $RPM_BUILD_ROOT/usr/share/instead/games/%{name} && tar xBf -)

cat >%{name}.desktop <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=%{name}
Comment=%{summary}
Keywords=game;
Exec=/usr/bin/sdl-instead -game %{name} -standalone
Categories=Game;
Icon=%{name}
EOF

install -d -m 0755 $RPM_BUILD_ROOT/usr/share/applications
install -m 0644 %{name}.desktop $RPM_BUILD_ROOT/usr/share/applications/
install -d -m 0755 $RPM_BUILD_ROOT/usr/share/icons/hicolor/128x128/apps/*
install -m 0644 ../%{name}.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/128x128/apps/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
#%doc doc/*
%{_datadir}/instead/games/%{name}/*
%{_datadir}/applications/*
%{_datadir}/icons/hicolor/128x128/apps/*
