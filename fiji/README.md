## Fiji / uFica

### Descriptions

This Fiji/ImageJ plugin works a front-end (GUI) for **ufica** engine
(FastICA*.{so,dll,dylib}). It is used to manipulate images/stacks before
and after passing through FastICA engine for estimation and unmixing.

<img src="http://lamdao.github.io/ufica/ufica.png" width="720px"/>

*3-channel images before and after unmixing using uFica, estimated
mixing matrix is showed in log window*

### Compile

- Open this project in Eclipse
- Adjust Referenced Libraries to include jna.jar and ij-[version].jar
(default is set to D:/Fiji/jars/{jna.jar,ij-150b.jar}).
- Select "Build Project" in menu Project if "Build Automatically" is not set.

### Installation

* For Fiji:
  - In folder plugins of Fiji, create folder uFica
  - Copy contents of bin folder to Fiji/plugins/uFica
  - Copy FastICA*.{so,dll,dylib} to Fiji/plugins/uFica

* For ImageJ
  - Do the same as in Fiji
  - Copy jna.jar to folder ImageJ/jars

### Execute

- Start Fiji/ImageJ
- Look for uFica in menu Plugins
