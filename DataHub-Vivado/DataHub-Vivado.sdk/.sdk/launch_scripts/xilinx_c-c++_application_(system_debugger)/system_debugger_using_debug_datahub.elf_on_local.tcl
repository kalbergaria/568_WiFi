connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent Nexys4DDR 210292A4BB27A"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent Nexys4DDR 210292A4BB27A"} -index 0
dow C:/Users/Keith/Documents/Git/568_Hub/568_WiFi/DataHub-Vivado/DataHub-Vivado.sdk/DataHub/Debug/DataHub.elf
bpadd -addr &main
