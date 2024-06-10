## How to build the Default UI page

### Install NPM and Grunt's build environment

1. Install NodeJS https://nodejs.org (with commandline enabled)
2. Run the following commands in current folder (using your **local proxy server**):
   1. "npm config set proxy http://proxy.etn.com:8080"
   2. "npm config set https-proxy http://proxy.etn.com:8080"
   3. "npm install grunt-cli -g"
   4. "npm install"

### Minimize UI page

1. Run the following command: "grunt"
2. Find minimized files in ./dist
3. Find compressed files in ./gzip
