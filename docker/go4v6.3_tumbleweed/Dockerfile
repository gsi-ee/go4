##################################################
##        get minimal distro from cloud         ##
##################################################

FROM opensuse/tumbleweed



##################################################
##                prerequisites                 ##
##################################################

### install system packages with opensuse package manager
RUN zypper --gpg-auto-import-keys ref && zypper dup -y && zypper --non-interactive  in \
  wget curl \
  tar zlib \
  cmake gcc gcc-c++ \
  git subversion \
  patterns-openSUSE-devel_qt5 libqt5-qtwebengine-devel \
  git bash cmake gcc-c++ gcc binutils \
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
  patterns-lxqt-lxqt	 





##################################################
##              Go4+ root               ##
##################################################

# we just install ROOT and Go4 here JAM2019:
RUN mkdir -p /installations/root
ENV ROOTFILE=root_v6.28.12.source.tar.gz
ENV ROOTFTP=https://root.cern.ch/download/${ROOTFILE}
ENV ROOTDIR=root-6.28.12
RUN wget -P /installations/root ${ROOTFTP}
RUN cd /installations/root/; ls -al; tar zxvf ${ROOTFILE}
RUN rm -rf /installations/root/rootbuild
RUN mkdir -p /installations/root/rootbuild
RUN cd /installations/root/rootbuild; \
cmake /installations/root/${ROOTDIR} -DCMAKE_CXX_STANDARD=17 -Droot7=on -Dwebgui=on -Dqt5web=on -Dhttp=ON -Dpython_version=3 -Dmysql=OFF; \
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
#enable this for tag 6.2.0
#svn co https://subversion.gsi.de/go4/tags/602-00 602-00/
git clone https://github.com/gsi-ee/go4.git 603-00

# we change default to build against hdf5 libs here:
COPY build_files/Makefile.discover /installations/go4/603-00/build/
RUN . /installations/root/rootbuild/bin/thisroot.sh; \
    cd /installations/go4/603-00; git checkout 6.3 \
    > /tmp/go4_make_exit_value; \
    { make -j6 withweb=1 >>makelog.txt 2>&1;  echo $? > /tmp/go4_make_exit_value; killall tail; } & \ 
  echo -e "\n\n---- display Go4 make log: ----\n\n"; \
  tail -F /installations/go4/603-00/makelog.txt & \
  wait; \
  echo -e "\n\n---- end of Go4 make log: ----\n\n"; \
  $( exit $(cat /tmp/go4_make_exit_value) )
RUN echo "Installation of Go4 6.3.0 and $ROOTDIR done!"
#RUN mv /root/.vnc/xstartup /root.vnc/xstartup.old 
#COPY conf/xstartup_lxqt /root/.vnc/xstartup












