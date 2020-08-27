
Unikraft-VMI project
===========


This repository contains the code for GSOC project "LibVMI as a Unikraft Application".

The goal is to port the virtual machine introspection library [LibVMI](https://github.com/libvmi/libvmi/) to be a [Unikraft](https://xenproject.org/developers/teams/unikraft/) unikernel application.
This allows to run the introspector (libvmi) in a dedicated Virtual Machine (VM), which is beneficial to security.

To achive this, it's necessary to port libmi and all dependencies and subdependencies to Unikraft. That means in practice to build them out of their config/build systems and ensuring they integrate with the Unikraft ecosystem. 
LibVMI has three dependencies that are not yet part of the Unikraft environment and that needed porting. 
Two of them are linked statically - Glib, Jsonc - and once they are ported, it's enough to declare them as a dependency in the vmi-example-application and Unikraft build system will handle the rest.

The last dependency, and the most difficult one to port - xentools - is linked against dynamically by libvmi.
xentools is a collection of libraries that ship with Xen, of interest to us are libxenstore (libxs) and libxencontrol (libxc).
Danymic linking is so a compiled libvmi installation can work with upgraded Xen installations without needing to be recompiled.
The code handling the dynamic loading can be found in 'libvmi/drivers/xen/libxc_wrapper.c and libxs_wrapper.c'.
Thus we need to declare the dependence on xentools, but also change the dynamic importing to static importing, as there is obviously no dynamic linking in a Unikraft application.


All of this has been done and should "just work" if one follows the build steps below. However, while the library itself was ported, not all functionality of xentools works yet - see section "What's left to be done".



Unikraft's porting guide covers all the important details related to porting the dependencies - [http://docs.unikraft.org/developers-app.html](http://docs.unikraft.org/developers-app.html).
Some things to keep in mind when looking at my ported libraries that may not be explicitly in the docs:

1. Unikraft will collect objects files in one folder without regard to their paths. Thus if you have dir1/linux.c and dir2/linux.c there will be linking errors.
Solution is to make them distinct by attachting "|dir1" and "|dir2" when adding them in the unikraft makefile. See vmi/Makefile.uk for examples.

2. One can execute arbitrary shell commands from within unikraft makefiles via `$(call build_cmd,CP,libxxx,$@, cp -r a b)`. See xentools/Makefile.uk for examples.


# This repository

This repository contains the libvmi library as well a its subdependencies, all ported to Unikraft, and an example of a libvmi-unikraft application.
Specifically, the subfolders contain:


* glib, jsonc, xentools - the sub-dependencies of libvmi
* vmi - libvmi itself
* zzz - a sample libvmi application, in fact one of the examples in libvmi/examples



# Setup testing environment

This section will cover how to setup a virtual testing environment based on nested virtualization. 
This means we will setup a Ubuntu VM, and inside this VM we will install Xen and execute the monitoring and target VMs.
Alternatively you can use a physical system directly. Generally it's advisable to turn off hyperthreading in the BIOS if possible, for performance reasons.


# Setting up the outer-vm


Install ubuntu 20.04 into a VMware VM. Create a standard VM with plenty of Memory (I am using 4GB) and one vcpu. In the "processors" tab of the VM configuration, make sure the three checkboxes under "Virtualization Engine" are all checked. This is required for nested virtualization. Follow the standard Ubuntu installation.

After installing the Ubuntu 20.04 LTS VM (minimal mode is enough), install the necessary package requirements to compile and run Xen and Unikraft.


    sudo apt update && sudo apt upgrade

    sudo apt install   git vim build-essential cmake make htop

    sudo apt install   bridge-utils grub-xen-bin grub-xen-host ibverbs-providers ipxe-qemu libaio1 libcacard0 libfdt1 libibverbs1 libiscsi7 libpmem1 python2 python2-minimal python2.7 python2.7-minimal libpython2-stdlib libpython2.7-minimal libpython2.7-stdlib librados2 librbd1 librdmacm1 libslirp0 libspice-server1 libusbredirparser1  libvirglrenderer1 seabios sharutils

    sudo apt install   bin86 bcc lzma lzma-dev liblzma-dev python python-dev gettext iasl uuid-dev uuid-runtime libncurses-dev libglib2.0-dev libpixman-1-dev libyajl-dev libvncserver-dev libsdl-dev libjpeg-dev libnl-route-3-200 libnl-3-dev libnl-cli-3-dev libnl-genl-3-dev libnl-route-3-dev iasl libbz2-dev e2fslibs-dev git-core uuid-dev ocaml ocaml-findlib libx11-dev bison flex xz-utils libyajl-dev libaio-dev markdown pandoc libsystemd-dev gawk bridge-utils libcurl4-openssl-dev bzip2 transfig tgif 

    sudo apt install   checkpolicy libc6-dev-i386 multiboot lvm2 gvncviewer libyaml-dev python3-setuptools socat python3-pip python3-testresources


Depending on the exact gcc version available, it might be necessary to upgrade to version 8 by installing gcc-8 package and forcing it as default:

    sudo apt install gcc-8
    sudo ln -fs gcc-8 /usr/bin/gcc


Now we will build Xen with xsm-security module enabled. (See also: [https://tinyvmi.github.io/step-by-step/install-xen-with-flask/#install-xsm](https://tinyvmi.github.io/step-by-step/install-xen-with-flask/#install-xsm) )


Download Xen 4.14 ([https://downloads.xenproject.org/release/xen/4.14.0/xen-4.14.0.tar.gz](https://downloads.xenproject.org/release/xen/4.14.0/xen-4.14.0.tar.gz)), extract and configure:

    ./configure --enable-systemd --enable-stubdom

Enable xsm security module support in menuconfig:

1. make -C xen menuconfig
2. enable "xen sec modules support"
3. enable the subitem "Flux Advanced Security Kernel support"
4. disable (!) the sub-subitem "compile xen with a built-in Flask security policy"

Then make and install:

    make dist
    sudo make install

Afterwards, we need to enable some services by default:

    sudo ldconfig

    sudo systemctl enable xen-qemu-dom0-disk-backend.service
    sudo systemctl enable xen-init-dom0.service
    sudo systemctl enable xenconsoled.service
    sudo systemctl enable xencommons
    sudo systemctl enable xen-watchdog.service

Also we need to setup a flask security policy. (See also: [https://tinyvmi.github.io/step-by-step/configuration/update-xsm-flask-policy/](https://tinyvmi.github.io/step-by-step/configuration/update-xsm-flask-policy/))
You should read up on the details here: [https://wiki.xenproject.org/wiki/Xen_Security_Modules_:_XSM-FLASK#Enabling_XSM_in_Xens](https://wiki.xenproject.org/wiki/Xen_Security_Modules_:_XSM-FLASK#Enabling_XSM_in_Xens)
To do so, we copy the default security policy from xen build to /boot/ and enable it in xen.cfg:


    sudo mkdir /boot/flaskpolicy
    sudo cp tools/flask/policy/xenpolicy-4.14.0 /boot/flaskpolicy/


edit or create `/etc/default/grub.d/xen.cfg` with:

    GRUB_CMDLINE_XEN_DEFAULT="dom0_mem=2048M,max:2048M flask=permissive"

Note how we set flask mode to `"permissive"` (for testing) and force dom0 memory reservation to half of our available memory (2 of 4 GB) - [see https://wiki.xenproject.org/wiki/Xen_Project_Best_Practices#Why_should_I_dedicate_fixed_amount_of_memory_for_Xen_Project_dom0.3F](see https://wiki.xenproject.org/wiki/Xen_Project_Best_Practices#Why_should_I_dedicate_fixed_amount_of_memory_for_Xen_Project_dom0.3F)

We also need to ensure the flask policy is loaded. To do so we edit the grub config file `/etc/grub.d/20_linux_xen` and insert in the function `"linux_entry ()"` after the line `${module_loader}     --nounzip ..."`:

    ${module_loader} /boot/flaskpolicy/xenpolicy-4.14.0

Then update grup:

    sudo update-grub2


Reboot into xen. That means, reboot the Ubuntu 20.04 VM, hold escape and boot into the `" .... (with xen)"` boot option.
Now check if the flask policy was loaded correctly:

    $sudo xl getenforce
    Permissive


# Setting up Unikraft


We can now also install kraft (See also: [http://www.unikraft.org/getting-started](http://www.unikraft.org/getting-started):


    pip3 install git+https://github.com/unikraft/kraft.git

and update its cache

    kraft list



Congratulations! Setting up the environment is done, now we can move to compiling the code we are actually working on.
To do so, we need to checkout the additional libraries need to build libvmi that were ported by me for GSOC 2020.
Check out the repository.

Now we can move the libraries (folders glib, jsonc, vmi, xentools) to `~/.unikraft/libs/` and the application ("zzz") to `~/.unikraft/app`.

    cp -r glib jsonc vmi xentools ~/.unikraft/libs
    cp -r zzz ~/.unikraft/apps


cd into `~/.unikraft/apps/zzz` and build the introspection image

    make menuconfig
    make

This takes a while and needs an active internet connection for every full rebuild.
If everything worked fine, you can now run the application with

    kraft run -p xen <vmname>

The zzz application contains libvmi, and executes libvmis' cr3 example by default. To run another libvmi-example, just copy the source file from libvmi/examples to ~/.unikraft/app, and adjust the Makefile.uk file.


# Creating a target VM

Now it's time to create a target VM to be monitored inside the Ubuntu 20.04 VM where we installed Xen. On this target-vm we will also install Ubuntu. Remember, all the following steps are execute *inside* the outer-vm we setup above. Use the following dom1.cfg file:

    type='hvm'
    memory = 1500
    vcpus=1
    name = "dom1"
    disk = ['phy:/dev/vg0/dom1,hda,w','file:/home/usar/ubuntu-20.04-desktop-amd64.iso,hdc:cdrom,r']
    acpi = 1
    device_model_version = 'qemu-xen'
    boot="d"
    sdl=0
    serial='pty'
    vnc=1
    vnclisten="127.0.0.1"
    vncpasswd="x"

First we must setup LVM2 volumen for storage. Easiest way is to add a second HDD to the outer-VM and setup LVM management for it:

    pvcreate /dev/sdaxxx   (replace sdaxxx with the newly added hdd)
    vgcreate vg0 /dev/sdaxxx //create volume group
    lvcreate -n dom1 -L 20G vg0

start the vm:

    sudo xl create dom1.cfg

you can now see it with its security context

    sudo xl list -z

connect to it via

    gvncviewer 127.0.0.1

and install Ubuntu as usual.


Before we can work with this, we also need to give the unikraft application access to the xenstore for dom1.
See [https://xenbits.xen.org/docs/4.6-testing/man/xenstore-chmod.1.html](https://xenbits.xen.org/docs/4.6-testing/man/xenstore-chmod.1.html)

Follow these steps:
[https://tinyvmi.github.io/step-by-step/run-tinyvmi/](https://tinyvmi.github.io/step-by-step/run-tinyvmi/)


# Useful links

* Xen beginners guide: [https://wiki.xenproject.org/wiki/Xen_Project_Beginners_Guide](https://wiki.xenproject.org/wiki/Xen_Project_Beginners_Guide)
* TinyVMI documentation: [https://tinyvmi.github.io/](https://tinyvmi.github.io/)
* Compiling Xen from source: [https://wiki.xenproject.org/wiki/Compiling_Xen_From_Source#Release_Tarballs](https://wiki.xenproject.org/wiki/Compiling_Xen_From_Source#Release_Tarballs)
* How to install Xen on Ubuntu: [https://www.zymr.com/how-to-install-the-xen-hypervisor-on-ubuntu/](https://www.zymr.com/how-to-install-the-xen-hypervisor-on-ubuntu/)
* XSM-FLASK documentation: [https://wiki.xenproject.org/wiki/Xen_Security_Modules_:_XSM-FLASK#Enabling_XSM_in_Xen](https://wiki.xenproject.org/wiki/Xen_Security_Modules_:_XSM-FLASK#Enabling_XSM_in_Xen)
* Unikraft documentation: [http://docs.unikraft.org/index.html](http://docs.unikraft.org/index.html)



# What's left to be done

While libvmi and all its dependencies were ported, compile and run fine, its functionality doesnt work yet. This is because it needs to access certain pseudo-files used by Xen for hypercalls and communication, such as /dev/xen/privcmd.
Since these are only available on the hosting Ubuntu 20.04 VM, and not inside the unikraft application, the app fails.
What needs to be done is to simply map the /dev/xen/* node into the unikraft image upon booting.
What will play a role here is the uk9p and the vfscore package you can enable on the "make menuconfig" step. If you check "Automatically mount a root filesystem", vfscore wil provide an empty / root.
Now what needs to be done is to map /dev/xen into it. This should be possible. For example, the python3 app maps the python standardlibrary into its image somehow: https://github.com/unikraft/app-python3/

However, I failed to get this to work in time for this writeup...


