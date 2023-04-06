SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\windows\system32;C:\windows;C:\windows\system32\wbem;C:\windows\system32\WindowsPowershell\v1.0
SET CPU_TYPE=LPC1768
SET CPU_VENDOR=NXP
SET UV2_TARGET=AE Lib SIM
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sim\ae.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sim\ae_tasks_util.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sim\ae_timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sim\ae_util.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sim\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\sim\ae_tasks13_g30.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAr" --Via ".\Objects\SIM\ArInp.Scr"
