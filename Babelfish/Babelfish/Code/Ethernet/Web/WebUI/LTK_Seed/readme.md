
#I. Environment Set Up

##**Stuff You'll Need - Global Dependencies**

###Node

* **[Node](http://nodejs.org/)** : [Download and install zip](http://nodejs.org/download/) (http://nodejs.org/download/)
* **[NPM](https://www.npmjs.com/)** - [Node Package Manager](https://www.npmjs.com/) now is already included in the node install, so you should be able to run `npm -v` now. NPM allows you to install tools we'll be using like Bower and Grunt.


###Proxies 

To download packages with the 'npm install' command, you will probably have to set npm to use the Eaton proxy. Run both: 
	
	npm config set proxy http://proxy.etn.com:8080
	npm config set https-proxy http://proxy.etn.com:8080


While you're at it, set proxies for git. 
	
	git config --global http.proxy http://proxy.etn.com:8080
	git config --global https.proxy http://proxy.etn.com:8080



## **Staging Git**

[Stash - Getting Started with Git](https://www.atlassian.com/git/tutorials/setting-up-a-repository).

We'll be using Stash. Please contact a Stash admin to get this set up if it's not already. Your username/password are same as Eaton network ID.  

You should be able to [log in and view all](http://pxgit.raleigh.software.ch.etn.com:7990/) projects (I'd assume if you're reading this). 

Open cmd line and navigate to the local directory you'll be working in. When we add the repository, it will create a new directory with the name of the project. 


* Log into Stash with your E number. *Make sure a Stash admin has added you to the project (email ericatha@eaton.com :)*
* Navigate to the current project. Click the elipsise button in the left menu, and select 'Clone.' Copy the address in the selection pop-out. 


* In cmd, type `git clone <address>` where address is the URL you copied (right-click paste). 
	* It should ask for your password. After it's cloned, you can see the new directory in Explorer. 
	* Navigate to the project root `cmd cd <project name>`


### Install Project Dependencies

This should take a few moments... Node Package Manager is going to install everything referenced in the `package.json` file. 

In the root project directory, run:
	
	npm install

Once you have these dependencies installed to node-modules folder, run

	npm run install

This downloads and installs all our dependencies in `bower.json`.

Occasionally `bower.json` will be updated to include a new dependency. If that is the case, `npm run install` should be run again to install that new package. When adding a new dependency, please mention this in the commit message or email to inform other team members.

It may be necessary to add (or remove) proxy and ssl settings to `.bowerrc` if there are issues connecting from behind a proxy.

	"proxy": "http://proxy.etn.com:8080",
	"https-proxy": "http://proxy.etn.com:8080",
	"strict-ssl": false

###Ways to View (update to reflect our project)

To view your work, run `npm run serve` and go to `http://localhost:9000/`

`npm run serve` only works when the LTK is running with CORS enabled.

---

#II. Developing new code and testing

Any time new code is saved, `npm run serve` will update the web page for you

- add dependencies with `npm run add -- [package] --save`

---

#III. Exporting to GIT

To see files that have been changed:

    git status

To add files to your commit use:

    git add

To commit your changes (where -m means message)

    git commit -m ""

To commit your changes (where -a means all *tracked* files)

    git commit -a

To push your commit to the repository:
    
    git push origin master

To pull new changes from the repository:

    git pull



