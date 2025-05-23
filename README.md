# Myriota Flex SDK

The Myriota FlexSense provides an off-the-shelf ready-to-go hardware
solution for IoT sensing and monitoring applications with Myriota’s
bi-directional, direct-to-orbit (D2O) satellite communications. The
Myriota Flex SDK provides users with the necessary boilerplate
to get started writing custom application firmware for the Myriota
FlexSense. See the full docs [here](https://flex-docs.myriota.com/)
and find the source code [here](https://github.com/Myriota/Flex-SDK).

> [!TIP]
> Source code for reference applications using the Flex SDK can be found
> [here](https://github.com/Myriota/Flex-Reference-Applications). Pre-built
> binaries of the reference applications can be found
> [here](https://devicemanager.myriota.com/binaries).
> N.B. These reference applications should be used for lab testing
> only and are not suitable for production environments.

### Building applications

The Flex SDK currently supports building applications on Ubuntu 22.04. Other
platforms can be supported through the use of
[DevContainer](https://code.visualstudio.com/docs/devcontainers/containers)
from within [VSCode](https://code.visualstudio.com/) or via
[Github Codespaces](https://docs.github.com/en/codespaces/overview).

Detailed setup instructions are outlined in the following sections:
* [Ubuntu 22.04](#setting-up-ubuntu-2204)
* [DevContainer on Windows](#setting-up-devcontainer-on-windows)
* [Github Codespaces](#setting-up-github-codespaces)

### Programming devices

Myriota supports programming of the FlexSense from any version of Linux,
Windows and Mac that support the installation of the 'python3.10' and it's
corresponding version of 'pip'.

## Build Environment Setup

### Setting up Ubuntu 22.04

#### Install packages

Install these packages with apt:

```shell
sudo apt install \
  git \
  curl \
  xxd \
  meson \
  ninja-build \
  gcc \
  python3 \
  python3-pip \
  python-is-python3
```

>  [!TIP]
>  If you install these programs via other methods then make sure
>  they meet these version requirements:
>  - **meson >= 0.61.2**
>  - **ninja >= 1.10.1**
>  - **python >= 3.10.12**
>
>  You can ignore the gcc version as it is only installed as a
>  requirement of meson native builds, but not actually used in the cross
>  building process.

#### Install GNU ARM Toolchain

Myriota uses the GNU ARM Toolchain version 7.2.1 to compile user
applications for the Myriota FlexSense. Myriota hosts its own copy of
this toolchain and it can be installed in `/opt/gcc-arm/` with the command:

```shell
sudo ./scripts/install_arm_toolchain.sh
```

> [!IMPORTANT]
> You must update your `PATH` variable to include the toolchain. i.e.
> Add `export PATH="${PATH}:/opt/gcc-arm-13_2_1/bin"` to your `~/.profile` or
> wherever you configure your environment variables for your given
> shell.

#### Install Python Requirements

Install the correct python environment with:

```
pip3 install -r requirements.txt
```

> [!TIP]
> For simplicity we install the python packages globally, but you
> can also use a python virtual environment. See
> [here](https://docs.python.org/3/tutorial/venv.html)
> for details.

### Downloading libflex and system_image binaries

The `libflex.a` and `system_image.bin` binaries must downloaded and installed
in the correct location, in order to perform a build. This can be
achieved by running the following command:

```shell
python scripts/download_binaries.py
```

> [!IMPORTANT]
> You require valid Myriota Device Manager credentials in order to
> download the binaries. The script will prompt you to supply them.

### Setting Up DevContainer on Windows

To build applications using a DevContainer on Windows you should first install
[VSCode](https://code.visualstudio.com/). When you open the Flex SDK folder in
VSCode it should direct you through the steps that are required to setup your
[DevContainer](https://code.visualstudio.com/docs/devcontainers/containers).
Once you have the DevContainer open in a terminal you will be able to perform
the [build instruction](#building-the-user-application).

### Setting Up Github Codespaces

Codespaces is a cloud development environment provided by Github. Further
information on how to use Codespaces is available in the
[Github documentation](https://docs.github.com/en/codespaces/overview).

> [!TIP]
> Note that using Codespaces may incur a cost. Refer to the Github
> documentation for details regarding pricing, storage, and usage.

## Building The User Application

### 1. Setup
The following build instructions assume your current working directory
is the project root. Configure the build with the following command (this only
needs to be run once):

```shell
meson --cross-file ./flex-crossfile.ini build
```

> [!IMPORTANT]
> Make sure to provide the `--cross-file ./flex-crossfile.ini` otherwise
> the build will fail.

#### Setup Options
##### 1. Skip GNSS
To assist development where a GPS signal is difficult to obtain, for example
an indoor environment, the Flex SDK provides the `skip_gnss` option.<br />
By default the `skip_gnss` option is disabled. To enable this option in the build,
simply add `-Dskip_gnss=true` in the setup step, before building:
```shell
meson -Dskip_gnss=true --cross-file ./flex-crossfile.ini build
```

> [!IMPORTANT]
> During production builds, please validate that the `skip_gnss` option is
> disabled (set to false), to ensure the correct operation of the FlexSense device.

### 2. Build
Then to perform the build simply run the command:

```shell
meson compile -C build
```

> [!IMPORTANT]
> After building the source the compiled user application will be available
> under the `build/` directory as `user_application.bin`. You can also find
> compiled versions of examples under `build/examples/`.

> [!TIP]
> You can find the source code of the examples in the following
> directory `examples`.

### Pristine Build
To perform a pristine build, delete the `build` folder before running the
setup and build commands again.

## Programming The FlexSense

Programming a FlexSense device requires the flashing of two separate
images:

- System Image or 'system_image.bin'
- User Application or 'user_application.bin'

> [!IMPORTANT]
> The System Image only needs to be flashed once during development or on the
> release of a new Flex SDK. The User Application is what you
> will be developing so update it as you need. If you flash the System
> Image you must re-flash the User Application.

The instructions below outline how to program the System Image and User
Application via the command line. Myriota also provides the DeviceAssist GUI
tool for programming devices. The DeviceAssist tool is available from the
**Tools** section of the [Myriota Support Site](https://support.myriota.com/).

### Programming On Linux

The following programming instructions assume your current working directory
is the project root. To program the System Image run:

```shell
./scripts/updater.py -f ./subprojects/libflex/system_image/system_image-*.bin
```

To program the User Application:

```shell
./scripts/updater.py -m ./build/user_application.bin
```

### Programming On Windows

#### Install Python and Python Requirements

Programming the FlexSense requires Python version 3.10 or greater.

The following instructions can be used to install Python on a Windows platform.

> [!IMPORTANT]
> The following commands should all be performed from within a Windows Terminal
> Shell (CMD or PowerShell).

Install python:

```powershell
winget install -e --id Python.Python.3.10
```

Then navigate the Flex SDK projects root directory, and run the following:

```powershell
pip3 install -r requirements.txt
```
#### Programming instructions

The following programming instructions assume your current working directory
is the project root. To program the System Image run:

```shell
python .\scripts\updater.py -f .\subprojects\libflex\system_image\system_image-*.bin
```

To program the User Application run:

```shell
python .\scripts\updater.py -m .\build\user_application.bin
```
