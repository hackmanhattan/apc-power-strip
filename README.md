apc-power-strip
===============

Sketch to wirelessly control the APC power strip.

Setup
=====

Depending on your platform, you may need to install the FTDI driver to be able to communicate with
the XBee.  Go to http://www.ftdichip.com/Drivers/VCP.htm and select the driver appropriate for your platform.

You may need to reboot after installing the driver :-/

<a name="conn">Connecting</a>
==========

The XBee built into the power strip is set to talk with the XBee on Robby O'Connor's XBee explorer.  If you are
well versed in the ways of the XBee, you may install 
[XCTU-ng](http://www.digi.com/support/productdetail?pid=3430&type=utilities) and change the settings.

Mind you, XBees are relatively hard to work with, there's a bit of voodoo involved, so really be sure you know what
you're doing or you'll brick one or both XBees, or make them unable to talk to each other.  

I set the external XBee
to be a coordinator, and I'm pretty sure I set the destination address of the power strip XBee to 0/0, so it will 
always talk to whoever the coordinator is, so setting any XBee S2 up as a coordinator should make it able to talk.
If the previous sentence makes no sense to you, don't mess with the settings!

Usage
=====

You can hit 'h' to get help.  The help is as follows:

        *** HELP ***
        Commands are followed by one or more numbers indicating the relay to change
            nxx... turn relays on
            fxx... turn relays off
            txx... toggle relays
            c turn off all relays

xx is one or more numbers between 0 and 7.  For now, 7 corresponds to relay 1, and 0 to relay 8.  The sketch
in this repository fixes the ordering.

Also, if you don't have local echo set and forget your place or make a mistake, you can hit ESC or x while
entering numbers and cancel the current command.

Hacking
=======

The XBee is fairly hard to work with, a bluetooth solution may be better, and/or a Nordic chip.  If you want
to change the wireless configuration, please create a topic branch named for the technology used, and update
the <a href="#conn">Connecting</a> section of this readme with instructions.
