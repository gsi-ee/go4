##################################################
##        get minimal distro from cloud         ##
##################################################

FROM opensuse/tumbleweed




##################################################
##                prerequisites                 ##
##################################################

### install system packages with opensuse package manager
RUN zypper ref && zypper --non-interactive in \
  wget curl \
  tar zlib \
  cmake gcc gcc-c++ \
  libX11-devel libXext-devel libXft-devel libXpm-devel\
  git subversion \
  patterns-openSUSE-devel_qt5 libqt5-qtwebengine-devel \
  git bash cmake gcc-c++ gcc binutils \
  xorg-x11-libX11-devel xorg-x11-libXpm-devel xorg-x11-devel \
  xorg-x11-proto-devel xorg-x11-libXext-devel \
  libpng16-devel libpng16-compat-devel\
  gcc-fortran libopenssl-devel \
  pcre-devel Mesa glew-devel pkg-config libmysqlclient-devel \
  fftw3-devel libcfitsio-devel graphviz-devel \
  libdns_sd avahi-compat-mDNSResponder-devel openldap2-devel \
  python3-devel libxml2-devel krb5-devel gsl-devel\
  glu-devel \
  xterm screen xvfb-run x11vnc openbox \
  boost-devel \
  vim \
  dhcp-server\
  rpcbind \
  gnuplot \
  ImageMagick \
  perl-XML-LibXML \
  glibc-locale \
  tmux \
  xorg-x11-Xvnc \
  emacs \
  htop \
  ncdu \
  psmisc \
  python3-pip \
  firefox \
  lxpanel \
  libtirpc-devel \
  perl-File-chdir \
  perl-TimeDate \
  gzip \
  tig \
  python3-numpy \
  dialog \
  hdf5-devel\
  twm\
  lxde-common\
  patterns-lxqt-lxqt	 





##################################################
##              Go4+ root               ##
##################################################

# we just install ROOT and Go4 here JAM2019:
RUN mkdir -p /installations/root
ENV ROOTFILE=root_v6.24.00.source.tar.gz
ENV ROOTFTP=https://root.cern.ch/download/${ROOTFILE}
ENV ROOTDIR=root-6.24.00
RUN wget -P /installations/root ${ROOTFTP}
RUN cd /installations/root/; ls -al; tar zxvf ${ROOTFILE}
RUN rm -rf /installations/root/rootbuild
RUN mkdir -p /installations/root/rootbuild
RUN cd /installations/root/rootbuild; \
cmake /installations/root/${ROOTDIR} -DCMAKE_CXX_STANDARD=14 -Droot7=on -Dwebgui=on -Dqt5web=on -Dhttp=ON -Dpython_version=3 -Dmysql=OFF; \
> /tmp/root_make_exit_value; \
{ make -j6 >>makelog.txt 2>&1;  echo $? > /tmp/root_make_exit_value; killall tail; } & \ 
  echo -e "\n\n---- display ROOT make log: ----\n\n"; \
  tail -F /installations/root/rootbuild/makelog.txt & \
  wait; \
  echo -e "\n\n---- end of ROOT make log: ----\n\n"; \
  $( exit $(cat /tmp/root_make_exit_value) )
RUN mkdir -p /installations/go4; \
cd /installations/go4; \
# enable this to get most recent developer version
#svn co https://subversion.gsi.de/go4/trunk 601-00/
#enable this for tag 6.1.2
svn co https://subversion.gsi.de/go4/tags/601-04 601-04/

# we change default to build against hdf5 libs here:
COPY build_files/Makefile.discover /installations/go4/601-04/build/
RUN . /installations/root/rootbuild/bin/thisroot.sh; \
    cd /installations/go4/601-04; \
    > /tmp/go4_make_exit_value; \
    { make -j6 withweb=1 >>makelog.txt 2>&1;  echo $? > /tmp/go4_make_exit_value; killall tail; } & \ 
  echo -e "\n\n---- display Go4 make log: ----\n\n"; \
  tail -F /installations/go4/601-02/makelog.txt & \
  wait; \
  echo -e "\n\n---- end of Go4 make log: ----\n\n"; \
  $( exit $(cat /tmp/go4_make_exit_value) )
RUN echo "Installation of Go4 6.1.2 and $ROOTDIR done!"
#RUN mv /root/.vnc/xstartup /root.vnc/xstartup.old 
COPY conf/xstartup_lxqt /root/.vnc/xstartup












