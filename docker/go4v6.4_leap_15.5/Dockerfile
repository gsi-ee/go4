##################################################
##        get minimal distro from cloud         ##
##################################################

FROM opensuse/leap:15.5



##################################################
##                prerequisites                 ##
##################################################

### install system packages with opensuse package manager
RUN zypper --gpg-auto-import-keys ref && zypper dup -y && zypper --non-interactive  in \
  wget curl \
  tar zlib \
  cmake gcc14 gcc14-c++ \
  git subversion \
  qt6-webenginecore-devel qt6-webenginewidgets-devel qt6-base-common-devel qt6-gui-devel \
  git bash cmake gcc14-c++ gcc14 binutils \
  libpng16-devel libpng16-compat-devel\
  gcc-fortran libopenssl-devel \
  pcre-devel Mesa glew-devel pkg-config libmysqlclient-devel \
  fftw3-devel cfitsio-devel graphviz-devel \
  libdns_sd avahi-compat-mDNSResponder-devel openldap2-devel \
  python3-devel libxml2-devel krb5-devel gsl-devel\
  glu-devel xorg-x11-Xvnc \
  libXpm-devel libXext-devel libXft-devel \
  libzstd-devel libuuid-devel \
  xterm screen xvfb-run x11vnc openbox \
  boost-devel \
  vim \
  rpcbind \
  gnuplot \
  ImageMagick \
  perl-XML-LibXML \
  glibc-locale \
  tmux \
  htop \
  psmisc \
  python3-pip \
  lxpanel \
  libtirpc-devel \
  perl-File-chdir \
  perl-TimeDate \
  gzip \
  python311-numpy \
  dialog \
  hdf5-devel\
  twm\
  lxde-common\
  patterns-lxqt-lxqt\
  davix-devel \
  tbb-devel


# need to put more recent compilers as default:
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 50
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 50
RUN update-alternatives --install /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-14 50
RUN update-alternatives --install /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-14 50
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-14 50
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-14 50



##################################################
##              Go4+ root               ##
##################################################

# we just install ROOT and Go4 here JAM2019:
RUN mkdir -p /installations/root
ENV ROOTFILE=root_v6.32.10.source.tar.gz
ENV ROOTFTP=https://root.cern.ch/download/${ROOTFILE}
ENV ROOTDIR=root-6.32.10
RUN wget -P /installations/root ${ROOTFTP}
RUN cd /installations/root/; ls -al; tar zxvf ${ROOTFILE}
RUN rm -rf /installations/root/rootbuild
RUN mkdir -p /installations/root/rootbuild
RUN cd /installations/root/rootbuild; \
cmake /installations/root/${ROOTDIR} -Dwebgui=on -Dqt5web=on -Dhttp=ON ; \
> /tmp/root_make_exit_value;\
{ make -j6 >>makelog.txt 2>&1;  echo $? > /tmp/root_make_exit_value; killall tail; } & \
  echo -e "\n\n---- display ROOT make log: ----\n\n"; \
  tail -F /installations/root/rootbuild/makelog.txt & \
  wait; \
  echo -e "\n\n---- end of ROOT make log: ----\n\n"; \
  $( exit $(cat /tmp/root_make_exit_value) )

RUN mkdir -p /installations/go4/git; \
mkdir -p /installations/go4/604-00; \
cd /installations/go4/git;\
git clone https://github.com/gsi-ee/go4 .
RUN . /installations/root/rootbuild/bin/thisroot.sh; \
    cd /installations/go4/git; git checkout 6.4 \
    > /tmp/go4_make_exit_value; \
    { cd /installations/go4/604-00; cmake ../git; make -j6 >>makelog.txt 2>&1;  echo $? > /tmp/go4_make_exit_value; killall tail; } & \
  echo -e "\n\n---- display Go4 make log: ----\n\n"; \
  tail -F /installations/go4/604-00/makelog.txt & \
  wait; \
  echo -e "\n\n---- end of Go4 make log: ----\n\n"; \
  $( exit $(cat /tmp/go4_make_exit_value) )
RUN echo "Installation of Go4 6.4.0 and $ROOTDIR done!"
COPY conf/xstartup_lxqt /root/.vnc/xstartup












