<p align = "center">
  <img src = "Media\Logo\Outputs\WithText\DarkModeVariant\grapesOS_logo_d_png.png" width ="270" alt="grapesOS logo">
</p>

<h2 align="center">A CLI-based Operating System, designed to run on Raspberry Pi</h2>


<p>grapesOS is a lightweight, CLI-based operating system designed to run on Raspberry Pi 3 and 4 using ARM bare-metal assembly. The current focus is on building a command-line interface, though GUI support might be explored later. It features a modular architecture where CLI commands can be run under a potential GUI layer. Multimedia support and other complex subsystems are being skipped for now due to their complexity.</p>

<p>This OS is designed for bare-metal Raspberry Pi hardware and has some weird stuff which is relevant to Rpi (like talking to GPU instead of CPU on boot).</p>

<p>Currently, the system supports basic video rendering, GPIO pins, mini UART configs and ofcourse, the mailbox interface. I doubt if its stable, and I REALLY need to work on making it more cohesive. Plus, there's no memory management, no scheduler, nothing. Just a bare-metal terminal that has some *pretty* bad hard-coded commands. The command parser just *works* by pre-loaded strings. It's basically a toy for now. But hey, I plan to make it a functioning OS! Everyone starts from somewhere.</p>

<p>If you're interested in the story behind this OS, check out my blog (https://grapesos-dev.blogspot.com) where I regularly post updates on the project, my thought process, struggles, experiments, and whatever fried my brain that day.</p>

<p>Finally, this is purely a passion project to explore how computers truly work under the hood. It’s been an amazing journey so far, and I hope to keep growing it as I get time. If you’d like to contribute, ask questions, or just say hi, my Discord is: sudu_493412. Thanks for reading. Cheers!</p>
