# NibblePoker's Windows Atom Utility (W.I.P)
A small CLI & GUI utility that allows you to monitor, list & manipulate entries
in [Windows' Global Atom Table](https://learn.microsoft.com/en-us/windows/win32/dataxchg/about-atom-tables)
in a simple, efficient and scriptable way.

This tool can also be used by developers to make sure their applications properly free
their atoms when exiting since they
[can easily leave them hanging](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa#remarks).


## Features
* Fully integrable in scripts
  * Customizable separator & output elements
* Listing of all atoms
  * <s>Type can also be detected (Class, ???, Generic)</s>
* Monitor all string atoms
  * Detects additions, removal & changes
  * Can catch up or ignore existing entries
* <s>Filtering</s> (TODO)
* <s>Manipulate</s> (TODO)
* <s>Can be used as a quick support tool</s>
  * <s>Get simple counts for taken & free slots</s>
  * <s>???</s>


## Usage
Simply go to the [release page]() and download the latest version of this tool.

This executable contains both the GUI and CLI version of the tool and will adapt
its mode to the context it is run from by default.

There should also be an MSI installer in there if you prefer to use those.


### GUI
If you double-click the exe, the application will launch itself with a fully fledged GUI.

If ran from a command prompt, just to use the `gui` verb to force the
application to run in GUI mode:
```shell
.\AtomUtil.exe gui
```

Once the application has launched, ... (TODO)


### CLI
TODO

#### Demonstration
TODO


## Issues
If you encounter any issues or weird behaviour when using the application, feel free to [create an issue](#) or email me if you don't have a GitHub account.

As long as you describe the problem and give a screenshot or show what was printed out it should be fine.<br>
You don't have to worry about issue formatting and whatnot. 😊


## Licenses
This project is dual-licensed under the following open-source licenses.<br>
You can choose the one that best suits your needs:
1. [MIT License](LICENSE-MIT)<br>
   &nbsp;&nbsp;● Just include the `LICENSE-MIT` file and be done with it.

2. [CC0 1.0 Universal (CC0 1.0) (Public Domain)](LICENSE-CC0)<br>
   &nbsp;&nbsp;● Do whatever you want with it.<br>
   &nbsp;&nbsp;● No credit, mentions or anything else is needed.<br>
   &nbsp;&nbsp;● Just have fun programming & tinkering with it :)
