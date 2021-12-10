# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/bionic64"
  config.vm.hostname = "Sumo"

  config.vm.provider "virtualbox" do |v|
    v.memory = 8192
    v.cpus = 4
  end

  config.vm.provision "shell", inline: <<-SHELL
		sudo apt-get update
		sudo apt-get -y upgrade
    sudo apt-get install -y valgrind make git curl wget cmake python3 g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev
    wget https://golang.org/dl/go1.16.5.linux-amd64.tar.gz
    rm -rf /usr/local/go && tar -C /usr/local -xzf go1.16.5.linux-amd64.tar.gz
    sudo apt-get install -y python3.7-dev
    sudo apt-get install -y python3.7
    sudo apt-get install -y pkg-config
    sudo apt-get install -y python3-pip
    git clone --recursive https://github.com/eclipse/sumo
    export SUMO_HOME="$PWD/sumo"
    mkdir sumo/build/cmake-build && cd sumo/build/cmake-build
    cmake ../..
    make -j$(nproc)
  SHELL

end
