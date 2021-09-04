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
    sudo apt-get install -y valgrind make git cmake python3 g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev
    git clone --recursive https://github.com/eclipse/sumo
    export SUMO_HOME="$PWD/sumo"
    mkdir sumo/build/cmake-build && cd sumo/build/cmake-build
    cmake ../..
    make -j$(nproc)
  SHELL

end
