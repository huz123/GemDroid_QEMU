GemDroid: QEMU Part
====================
Description
----------
Welcome to GemDroid, An Infrastructure to Evaluate Mobile Platforms. GemDroid uses traces to feed its simulator. This patch generates traces of an application from the android emulator. The trace contains CPU/Memory access, frame buffer activity and raw instructions.

System Requirement
-------------------
AOSP [android-4.4.4_r2](https://source.android.com/source/downloading.html)


In this Repository
-----------------------------
1. gemdroid_qemu : 	__GemDroid QEMU part__
2. ReadMe.md 	 :	__This readme__

<!-- 3. FrameBuffer.cpp For DVI users -->

To Patch
-------- 
Assume that $AOSP is your aosp folder, follow the following commands you will have an emulator for GemDroid.

```bash
	# backup your original qemu folder
	cd $AOSP
	git clone https://github.com/huz123/GemDroid_QEMU
	cd $AOSP/sdk/emulator
	git checkout -b gemdroid cbf40c
	cd $AOSP/external
	mv qemu qemu.bk
	cp -r $AOSP/GemDroid_QEMU/gemdroid_qemu qemu
	cd qemu
	./android-configure.sh
	# remove "-Wl" in objs/config.make, compile d4-7 with -m32
	make 
	# use emulators in objs/
```



To use
------
0. Need an android virtual device [follow this link](https://developer.android.com/tools/devices/index.html);
1. start the emulator with your android virtual device as usual;
2. press "volume up" button will trace the Frame Buffer and Camera activities;
3. press "volume down" button will trace the CPU/Memory accesses;
4. press "call" button will trace raw instructions.

Acknowledgements
-------
GemDroid project is supported in part by NSF grants – #1302557, #0963839, #1205618, #1213052, #1320478, #1317560, #1302225, #1017882, and grants from Intel. 
More details about the NSF Expedition Project - Visual Corext On Silicon can be found at: http://www.cse.psu.edu/research/visualcortexonsilicon.expedition/index.html

FAQ
-------
<!-- Q: I use a DVI monitor. The numbers of Frame Buffer seem when I press "volume up" button.

A: It is a bug in the emulator. A quick fix is to replace $AOSP/sdk/emulator/opengl/host/libs/libOpenglRender/FrameBuffer.cpp using the file we provided. 
 -->
Contacts
------
If you have any questions and comments, please send to [haibo](http://huz123.github.io/) at cse.psu.edu or [nachi](http://www.cse.psu.edu/~nzc5047/) at cse.psu.edu.
