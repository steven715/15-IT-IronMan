cd /usr/local/src && \
curl -LO https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.tar.gz && \
tar -xvf cmake-3.22.2-linux-x86_64.tar.gz && \
mv cmake-3.22.2-linux-x86_64 /usr/local/cmake && \
echo 'export PATH="/usr/local/cmake/bin:$PATH"' >> ~/.bashrc && \
source ~/.bashrc

