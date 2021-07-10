# BScAero

Software for estimating basic aerodynamics characteristics of an aircraft wing.

The Prandtl-Lanchester lifting line theory is used for computations. Input data are airfoil aerodynamic charachteristics and wing geometry. The application estimates lift and drag coefficients against angle of attack, and distribution of this coefficients along wing span. Due to limitation of lifting line theory the software can by applied for calculations of wings with aspect ratio greater than 5 and sweep angle not less than 15 degrees.

## Installation

### Linux

Linux [dpkg package](https://github.com/marek-cel/bscaero/releases/download/0.2/bscaero_0.2_focal_amd64.deb) is provided for Ubuntu 20.04 LTS / LinuxMint 20.x.

### Windows

Windows [zip package](https://github.com/marek-cel/bscaero/releases/download/0.2/bscaero_0.2_win64.zip) is provided. It does not require installation.

## Building from source

### Dependencies

BScAero requires Qt and Qt Widgets for Technical Applications.

* [Qt5](https://www.qt.io/)
* [Qwt](https://qwt.sourceforge.io/)

#### Linux

On Ubuntu based Linux distributions those dependencies can be installed with the following command:

```apt install libqt5svg5-dev libqwt-qt5-dev qtbase5-dev```

#### Windows

On Windows those dependencies have to be installed manually.

### Building using qmake

BScAero can be built with ```qmake``` tool. Run qmake on ```bscaero.pro``` file and then use appropriate building system. You can also use Qt Creator.

![Screenshot](bscaero_01.jpg)

Visit [http://marekcel.pl/bscaero](http://marekcel.pl/bscaero) for more info.
