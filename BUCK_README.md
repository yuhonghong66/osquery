# Building with Buck

These are very rough instructions just for the preview version.


## Provision macOS

### Install tools

```
xcode-select --install

brew tap caskroom/cask
brew tap caskroom/versions
brew cask install java8
```

### Install Buck and Watchman

Watchman isn't mandatory but will make builds faster.

```
brew tap facebook/fb
brew install buck watchman
```


## Provision Ubuntu 18.10

### Install tools

```
sudo apt install openjdk-8-jre clang-7 libc++1 libc++-dev libc++abi1 libc++abi-dev python2 python
```

### Install dependencies

```
sudo apt install liblzma-dev
```

### Install Buck

```
wget 'https://github.com/facebook/buck/releases/download/v2018.10.29.01/buck.2018.10.29.01_all.deb'
sudo apt install ./buck.2018.10.29.01_all.deb
```


## Provision FreeBSD 11.2

### Install tools

```
sudo pkg install openjdk8 python3 python2 clang35
```

### Install Buck

```
sudo curl --output /usr/local/bin/buck 'https://jitpack.io/com/github/facebook/buck/v2018.10.29.01/buck-v2018.10.29.01.pex'
sudo chmod +x /usr/local/bin/buck
```

### Install dependencies

```
sudo pkg install glog thrift thrift-cpp boost-libs magic rocksdb-lite rapidjson zstd linenoise-ng augeas ssdeep sleuthkit yara aws-sdk-cpp lldpd libxml++-2 smartmontools lldpd
```


## Build

Replace <platform> bellow with the appropriate plaform, one of:

* `linux-x86_64`
* `macos-x86_64`
* `freebsd-x86_64`

```
cd osquery
buck build @mode/<platform>/release osquery:osqueryd
```

The binary will be available under `buck-out/gen/osquery/osqueryd`
