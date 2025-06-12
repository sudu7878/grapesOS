<p align = "center">
  <img src = "Media\Logo\Outputs\WithText\DarkModeVariant\grapesOS_logo_d_png.png" width ="270" alt="grapesOS logo">
</p>

<h2 align="center">A CLI-based Operating System, designed to run on Raspberry Pi 3 and 4.</h2>


<p>grapesOS is a lightweight, CLI-based operating system designed to run on Raspberry Pi using ARM assembly. The current focus is on building a command-line interface, though GUI support might be explored later. It features a modular architecture where CLI commands can be run under a potential GUI layer. Multimedia support and other complex subsystems are being skipped for now due to their complexity.</p>

<p>This OS is tailored for bare-metal Raspberry Pi hardware and bypasses the GPU on boot. I'm currently experimenting with different CPU scheduling algorithms, though many are resource-heavy and may cause unresponsiveness. A major upcoming feature is file system interaction: implementing FAT32 support, enabling read/write/delete operations through a disk interface.</p>

<p>Once the base system is stable, I plan to build reusable API-like modules, which could be used by other apps. So far, GPIO drivers have been implemented (still need testing!), and UART/IRQ handling is functional. Long-term, grapesOS aims to become a small, efficient, API-supportive embedded OS that balances performance and power.</p>

<p>If you're interested in the story behind this OS, check out my blog (link soon!)—where I’ll post updates on each commit, my thought process, struggles, experiments, and whatever fried my brain that day.</p>

<p>Finally, this is purely a passion project to explore how computers truly work under the hood. It’s been an amazing journey so far, and I hope to keep growing it. If you’d like to contribute, ask questions, or just say hi, my Discord is: sudu_493412. Thanks for reading, and cheers!</p>