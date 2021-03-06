%global binname qt5-virt-manager

Name:           qt-virt-manager
Version:        0.45.75
Release:        1%{?dist}
Summary:        Qt Virtual Machine Manager

License:        GPLv2+
URL:            http://f1ash.github.io/%{name}
Source0:        https://github.com/F1ash/%{name}/archive/%{version}.tar.gz

# be compatible with prior subpackages
Provides:       virt-manager-qt4 = %{version}-%{release}
Provides:       virt-manager-qt5 = %{version}-%{release}
Obsoletes:      virt-manager-qt4 <= 0.25.47
Obsoletes:      virt-manager-qt5 <= 0.25.47

Requires:       hicolor-icon-theme

Requires:       libvirt
Requires:       qtermwidget >= 0.7.1
Requires:       qt-remote-viewer

# for use qemu-kvm (more useful)
Requires:       qemu-kvm

# for use SPICE viewer
Requires:       spice-vdagent

# optional runtime extensions
%if 0%{?fedora}
# for scrubbing
Suggests:       scrub
# for use VNC client with tls
Suggests:       gnutls
%endif

BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qtmultimedia-devel
BuildRequires:  qt5-qtsvg-devel
BuildRequires:  qtermwidget-devel >= 0.7.1

BuildRequires:  libvirt-devel
BuildRequires:  glibc-headers
BuildRequires:  desktop-file-utils
BuildRequires:  cmake
BuildRequires:  glib2-devel
BuildRequires:  spice-protocol
BuildRequires:  spice-glib-devel
BuildRequires:  libvncserver-devel
%if %{?fedora}>=24
BuildRequires:  libcacard-devel
%endif

%description
Qt Virtual Machine Manager provides a graphical tool for
administering virtual machines for QEMU/KVM, Xen, LXC, OpenVZ,
VBox, VMWare, HyperV, IBM PowerVM, Bhyve virtual machines
and other Virtual Entities.
Start, stop, add or remove virtual devices, connect to a
graphical or serial console, and see resource usage
statistics for existing VMs on local or remote machines.
Uses libvirt as the back-end management API.
Uses Spice/VNC viewers for control.

%package -n     qt-remote-viewer
Summary:        Qt Remote Viewer

%description -n qt-remote-viewer
Qt viewer for remote access to Spice/VNC desktops.


%prep
%setup -q

%build
mkdir -p %{_target_platform}
pushd %{_target_platform}
%cmake -DBUILD_QT_VERSION=5 \
%if %{?fedora}>=24
 -DWITH_LIBCACARD=1 \
%endif
 ..
popd
%make_build -C %{_target_platform}

%install
%make_install -C %{_target_platform}

%check
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{binname}.desktop
desktop-file-validate %{buildroot}/%{_datadir}/applications/qt5-remote-viewer.desktop


%post
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :


%files
%license LICENSE Licenses
%doc README.md Changelog
%{_bindir}/%{binname}
%{_datadir}/applications/%{binname}.desktop
%{_datadir}/%{binname}/
%{_datadir}/icons/hicolor/256x256/apps/virtual-engineering.png

%files -n       qt-remote-viewer
%license LICENSE
%{_bindir}/qt5-remote-viewer
%{_datadir}/applications/qt5-remote-viewer.desktop
%{_datadir}/icons/hicolor/256x256/apps/remote-desktop-viewer.png


%changelog
* Wed Oct  4 2017 Fl@sh <kaperang07@gmail.com> - 0.45.75-1
- version updated;

* Mon Jun 19 2017 Fl@sh <kaperang07@gmail.com> - 0.43.72-1
- version updated;

* Fri May 12 2017 Fl@sh <kaperang07@gmail.com> - 0.43.70-1
- enhanced %%description;
- remove Qt4 build part;
- version updated;

* Fri Mar 17 2017 Fl@sh <kaperang07@gmail.com> - 0.42.67-1
- enhanced %%description;
- version updated;

* Mon Jan 30 2017 Fl@sh <kaperang07@gmail.com> - 0.39.60-1
- removed libvirt-daemon-driver-qemu, libvncserver R;
- version updated;

* Tue Oct 18 2016 Fl@sh <kaperang07@gmail.com> - 0.37.57-1
- version updated;
- added changes for qt-remote-viewer subpackages;

* Wed Oct  5 2016 Fl@sh <kaperang07@gmail.com> - 0.37.56-1
- version updated;

* Wed Sep 21 2016 Fl@sh <kaperang07@gmail.com> - 0.36.55-1
- version updated;
- added remote-desktop-viwer into %%check and %%files;

* Sat Jul 16 2016 Fl@sh <kaperang07@gmail.com> - 0.34.50-1
- version updated;

* Wed May 11 2016 Fl@sh <kaperang07@gmail.com> - 0.27.50-1
- version updated;
- clear %%changelog;
