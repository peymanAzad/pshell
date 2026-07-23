Name:           pshell
Version:        0.1.0
Release:        1%{?dist}
Summary:        A simple POSIX shell

License:        MIT
URL:            https://github.com/peymanAzad/pshell
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc, make

%global debug_package %{nil}

%description
pshell is a minimal command interpreter supporting pipelines,
redirections, variable assignment, and conditional execution.

%prep
%setup -q

%build
make release VERSION=%{version}

%install
make install PREFIX=%{buildroot}/usr

%files
/usr/bin/pshell
/usr/share/man/man1/pshell.1*

%changelog
* Wed Jul 23 2026 Peyman Azad <you@example.com> - 0.1.0-1
- Initial package
