SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\windows\system32;C:\windows;C:\windows\system32\wbem;C:\windows\system32\WindowsPowershell\v1.0
SET CPU_TYPE=LPC1768
SET CPU_VENDOR=NXP
SET UV2_TARGET=AE Lib RAM
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ram\ae.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ram\ae_tasks_util.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ram\ae_timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ram\ae_util.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ram\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\ram\ae_tasks_1.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAr" --Via ".\Objects\RAM\ArInp.Scr"
