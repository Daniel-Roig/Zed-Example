%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 ZedBoard OOB Design 14.1
 September 10, 2012

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Table of Contents
=================

  * Introduction
  * Package Contents
  * Restoring OOB Image to SD Card
  * Building the FPGA Hardware Bitsream
  * Building the First Stage Boot Loader
  * U-boot Source
  * Building a Zynq Boot Image
  * Building the Linux Kernel and Device Tree
  * The Root File System
  * References


Introduction
============

This README describes the contents of the ZedBoard Out Of Box (OOB) Design and 
details how to obtain and build the various sources. All Xilinx related files were 
generated using Xilinx EDK 14.1. 


Package Contents
================
***This document assumes that this package was extracted to a location referred
   to as <pkg>

boot_image: 
   -- system.bit – Bitstream used to configure the programmable logic
   -- u-boot.elf – Second-Stage boot loaded used to load Linux
   -- zynq_fsbl.elf – First-Stage boot loader used to configure the processing 
                      system

doc:
   -- README.txt – This file

hw:
   -- XPS project designed in EDK 14.1. Used to generate the hardware profile 
      and bitstream

linux:
   -- devicetree_ramdisk.dts – Device tree source code
   -- .config – The kernel configuration used to build the kernel (zImage)

sd_image:
   -- BOOT.BIN – Zynq configuration file that was generated using the 3 files
                 found in the boot_image folder.
   -- devicetree_ramdisk.dtb – Prebuilt device tree
   -- ramdisk8M.image.gz – Prebuilt Linux filesystem
   -- README – readme document that describes how to run the OOB design
   -- zImage – Prebuilt Linux kernel

sw:
   --Hardware profile
   --First-Stage bootloader source


Restoring OOB Image to SD Card
==============================

Do the following to restore an SD Card to the state it was in when shipped with
the ZedBoard:

1) Format the SD card as a single FAT32 partition
2) Copy the 5 files from the <pkg>/sd_image folder onto the SD card


Building the FPGA Hardware Bitstream
====================================

These steps explain how to generate the FPGA Hardware Bitsream using Xilinx 
Platform Studio (XPS).

1) Open Xilinx XPS 14.1

2) Open an existing XPS project
  -- Select "Open Project" from the Getting Started section
  -- Browse to <pkg>/hw/xps_proj folder and select system.xmp
  -- Press "OK"

3) Generate the Bitstream
  -- Under the "Hardware" menu, select "Generate Bitstream"
  -- The generated bitstream can be found at
     <pkg>/hw/xps_proj/implementation/system.bit


Building the First Stage Boot Loader
====================================

These steps explain how to compile the First Stage Boot Loader (FSBL) from the
SDK projects provided in this package.

1) Open Xilinx SDK 14.1

2) Select a workspace
  -- Press "Browse..." and browse to the "<pkg>/sw" directory
  -- Press "OK"
  -- Close the Welcome screen by clicking on the "x" in the Welcome tab

3) Import existing SDK projects
  -- Select from menu "File -> Import"
  -- Select "General -> Existing Projects into Workspace" and press "Next"
  -- Press "Browse..." next to "Select Root Directory:"
  -- Browse to the "<pkg>/sw" directory
  -- Press "OK"
  -- Make sure the following projects are checked: "hw_platform", "zynq_fsbl"
  -- Press "Finish"

4) Build SDK projects
  -- The build process will start automatically. This option can be changed by
     unchecking "Project -> Build Automatically" from the menu
  -- To manually build the project, right click on "zynq_fsbl" and select
     "Build Project"
  -- To clean the project, right click on "zynq_fsbl" and select
     "Clean Project"
  -- The generated Zynq FSBL executable can be found at
     <pkg>/sw/zynq_fsbl/Debug/zynq_fsbl.elf

Notes:
  -- The "zynq_fsbl" project has a dependency on the "hw_platform" project


U-boot Source
=============

The u-boot source code has been made publicly available by Xilinx. For 
information on obtaining and building it, refer to wiki.xilinx.com/zynq-uboot.

Note that if you rebuild u-boot.elf, you will have to rename the device tree
from "devicetree_ramdisk.dtb" to "devicetree.dtb".


Building a Zynq Boot Image
==========================

These steps explain how to create a boot image file (BOOT.BIN) from pre-compiled
binaries provided in this package. The pre-compiled binaries are located at
<pkg>/boot_image and include: "zynq_fsbl.elf", "system.bit", "u-boot.elf"

1) Open Xilinx SDK

2) Select a workspace
  -- It is recommended to use the same workspace as in the
     "Building the First Stage Boot Loader" step

3) Select "Xilinx Tools -> Create Boot Image" from the menu

4) Select the following in the "Create Zynq Boot Image" dialog box:
  -- Bif file: "Creat a new bif file"
  -- FSBL elf: "Browse" and select <pkg>/boot_image/zynq_fsbl.elf; Press OK
  -- Press "Add" and select <pkg>/boot_image/system.bit; Press OK
  -- Press "Add" and select <pkg>/boot_image/u-boot.elf; Press OK
  -- Output file: "Browse" and select <pkg>/boot_image for folder location and
     BOOT.BIN for file name
  -- Press "Create Image"
     Sometimes, an internal error message pops up which can be ignored.

4) BOOT.BIN file is ready at the specified location.

5) To test this file, copy BOOT.BIN onto the SD card

Notes:
  -- The boot image file has to be named "BOOT.BIN" so the fsbl executable can
     find it
  -- The order in which you add the files matters:
     it has to be FSBL first, then bitstream, then u-boot
  -- Instead of using the pre-compiled binaries from <pkg>/boot_image,
     you can also use your own binaries from the previous tutorials


Building the Linux Kernel and Device Tree
=========================================

Hardware requirement: Linux host on which to build the kernel.
Software requirement: ARM GNU tools. See wiki.xilinx.com/zynq-tools
                      git. See wiki.xilinx.com/using-git

To build the kernel, make sure Xilinx Linux cross compiler is installed and
environment variable is pointing to it. In particular, be sure environmental
variable CROSS_COMPILE is set and PATH includes the CodeBench tools.

The Linux kernel (zImage) used for this design is built off of the Digilent
Linux repository, at the commit tagged v3.3.0-digilent-12.07-zed-beta. To
download and build the source, do the following:

1) Clone the latest version of the Digilent Linux repository into the current
   working directory:

   bash> git clone https://github.com/Digilent/linux-digilent.git

2) Create a new branch starting at the commit tagged 
   v3.3.0-digilent-12.07-zed-beta:

   bash> cd linux-digilent
   bash> git checkout -b zedboard_oob v3.3.0-digilent-12.07-zed-beta

3) Copy the kernel configuration from this package to the kernel source:

   bash> cp <pkg>/linux/.config ./.config

4) Build the kernel:

   bash> make ARCH=arm

After the build is completed, the kernel may be found at ./arch/arm/boot/zImage.
To build the device tree, continue on and do the following:

5) Compile the device tree:

   bash> ./scripts/dtc/dtc -O dtb -I dts -o ./devicetree_ramdisk.dtb \
         <pkg>/linux/devicetree_ramdisk.dts

The compiled device tree will be placed at ./devicetree_ramdisk.dtb.


The Root File System
====================

The file system provided was built by following the guide available at 
http://wiki.xilinx.com/zynq-rootfs.

This file system was then modified in the following ways

--/etc/init.d/rcS: modified to initialize the demos upon boot up

--/etc/init.d/rcK: was modified to power off the OLED display on shutdown

--/var/www/: A website describing the ZedBoard was added here

--/usr/bin/: read_sw, write_led, load_oled, and unload_oled scripts were added

--/root/logo.bin: This file was added that contains the data for the OLED image
                  that is displayed. For further information on this format, see
                  the OLED driver documentation that is included with the kernel
                  source at linux-digilent/Documentation/pmods/pmodoled.txt

--/lib/modules/v3.3.0-digilent-12.07-zed-beta/pmodoled-gpio.ko:
         This is the loadable module for the OLED driver that is built along 
         with the kernel. After building the kernel as described above, it can
         be found at linux-digilent/drivers/pmods/pmodoled-gpio.ko


References
==========

Zynq Base TRD 14.1


