## ufica - Microscope Image Unmixing with FastICA
----------------------------------------------------------------------------


### Descriptions
----------------------------------------------------------------------------

This shared library provides a simple way to remove bleed-through/cross-talk
artifacts, purify and enhance fluorescence microscopic images. It was
developed as a demonstration for the paper ***"Use of independent component
analysis to improve signal-to-noise ratio in multi-probe fluorescence
microscopy"***.

### Build
----------------------------------------------------------------------------
#### 0. Requirements

- GNU C++/Make
- MinGW Windows cross-compiler (for compiling MS Windows DLL on Linux)
- Visual Studio (for compiling on MS Windows)

#### 1. Linux / Mac OSX

At terminal prompt

	$ cd fastica
	$ make

#### 2. Cross build MS Windows DLL from Linux

	$ CXX=/path/to/cross-build/g++ make

Ex: Compiling FastICA shared library for MS Windows 64-bit on Fedora 22 64-bit with MinGW64

	$ CXX=x86_64-w64-mingw32-g++ WRC=x86_64-w64-mingw32-windres make

#### 3. Build with Visual Studio C++

- Open FastICA.sln in Visual Studio C++ (13 or higher)
- Select solution platform (Win32/x64)
- Please F7


### Testing FastICA shared library with [vipy](https://github.com/lamdao/vipy)
----------------------------------------------------------------------------

#### 1. Download [vipy](https://github.com/lamdao/vipy) and [sample data](https://www.dropbox.com/sh/4mgijapnn7hh83a/AACisJ973BxbP5J-jempSqYGa/data)

* Download vipy using git

	  $ git clone https://github.com/lamdao/vipy.git

* Download vipy zip package

	  $ wget https://github.com/lamdao/vipy/archive/master.zip -O vipy-master.zip
	  $ unzip vipy-master.zip
	  $ mv vipy-master vipy

* Create a data folder inside **vipy** and download [sample data](https://www.dropbox.com/sh/4mgijapnn7hh83a/AACisJ973BxbP5J-jempSqYGa/data)

	  $ cd vipy
	  $ mkdir data
	  $ cd data
	  $ wget https://www.dropbox.com/sh/4mgijapnn7hh83a/AADrZVI-VCPuqfJuxMT3LWc4a/data/c1.miv
	  $ wget https://www.dropbox.com/sh/4mgijapnn7hh83a/AACBKR0wIOh5g8vACdx9r6DNa/data/c2.miv
	  $ wget https://www.dropbox.com/sh/4mgijapnn7hh83a/AAAmEWTnEcPIX59vjTTDhREFa/data/c3.miv
      $ cd ..	# return to vipy folder

#### 2. Test 

* Copy **FastICA** shared library (.dll, .so, .dylib) to **vipy** folder. For example, in Linux 64-bit system, do as follows:

	  $ cp /path/to/fastica/FastICA64.so ./FastICA.so

* Start python/ipython

	  $ python
	  Python 2.7.10 (default, Sep 24 2015, 17:50:09) 
	  [GCC 5.1.1 20150618 (Red Hat 5.1.1-4)] on linux2
	  Type "help", "copyright", "credits" or "license" for more information.
	  >>> import numpy as np
	  >>> from SimpleMIV import MIV_Load
	  >>> c1,_ = MIV_Load(r'data/c1.miv')
	  >>> c2,_ = MIV_Load(r'data/c2.miv')
	  >>> c3,d = MIV_Load(r'data/c3.miv')
	  >>> print "Image size =", d[:2]
	  Image size = [512 512]
	  >>> Dx = np.vstack((c1.ravel(), c2.ravel(), c3.ravel()))
	  >>> from DFastICA import DFastICA
	  >>> dfica = DFastICA()
	  --------------------------------------------
	  FastICA Engine for Microscope Image Unmixing
	                 Version 1.0.1
	  --------------------------------------------
	  >>> Mx = dfica.Run(Dx)
	  >>> print Mx	# show Mixing matrix
	  [[ 0.7127181   0.02186181  0.03956823]
	   [ 0.42947737  0.40057376  0.17753497]
	   [ 0.0320502   0.04985656  0.40218329]]
	  >>>

* The above script can be download at [Test_DFastICA.py](https://www.dropbox.com/sh/4mgijapnn7hh83a/AAB0y2mWfidbiqD7WjdrRVsxa/Test_DFastICA.py)

* See [test]() 

### References
----------------------------------------------------------------------------
* L. Dao, B. Lucotte, B. Glancy, L.-C. Chang, L.-Y.Hsu & R.S. Balaban,
*Use of independent component analysis to improve signal-to-noise ratio in
multi-probe fluorescence microscopy, Journal of Microscopy*