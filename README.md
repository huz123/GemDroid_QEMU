GemDroid: QEMU Part
====================
Description
----------
Welcome to GemDroid, An Infrastructure to Evaluate Mobile Platforms. GemDroid uses traces to feed its simulator. This patch generates traces of an application from the android emulator. The trace contains CPU/Memory access, frame buffer activity and raw instructions.

System Requirement
-------------------
AOSP 4.4.4 from https://source.android.com/source/downloading.html


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
	# remove "-Wl" in objs/config.make
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

FAQ
-------
<!-- Q: I use a DVI monitor. The numbers of Frame Buffer seem when I press "volume up" button.

A: It is a bug in the emulator. A quick fix is to replace $AOSP/sdk/emulator/opengl/host/libs/libOpenglRender/FrameBuffer.cpp using the file we provided. 
 -->
Contacts
------
If you find any bugs, please send to [haibo](http://huz123.github.io/) at cse.psu.edu or [nachi](http://www.cse.psu.edu/~nzc5047/) at cse.psu.edu.
