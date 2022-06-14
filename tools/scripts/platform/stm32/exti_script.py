import re
import sys
import os
import shutil

orig_file = str(sys.argv[1])
read_file = orig_file+"copy"
write_file = str(sys.argv[2])

with open(read_file, 'w') as f:
    f.write('/*Generated file*/')
f.close()

shutil.copyfile(orig_file, read_file)
pattern = re.compile("EXTI[0-9]+_[0-9]+_IRQ")
pattern_single_pin = re.compile("EXTI[0-9]+_IRQ")
pattern_pin_nb = re.compile("[0-9]+")

j = 0
k = 0
l = 0

lines_multiple_pins = []
lines_single_pin = []
single_pin_ids = []
multiple_pin_ids = []

# Parse file for EXTI instances and store
with open(read_file, 'r') as f:
    for i, line in enumerate(f,1):
        for match in re.finditer(pattern, line):
            j = j + 1
            lines_multiple_pins.insert(j, match.group())
        for match in re.finditer(pattern_single_pin, line):
            k = k + 1
            lines_single_pin.insert(k, match.group())
f.close()
os.remove(read_file)

# Remove duplicates
lines_multiple_pins = list(set(lines_multiple_pins))
lines_single_pin = list(set(lines_single_pin))

# Extract pin ids for EXTI instance on more pins
for i in range(len(lines_multiple_pins)):
    for match in re.finditer(pattern_pin_nb, lines_multiple_pins[i]):
        l = l + 1
        multiple_pin_ids.insert(l, match.group())

# Convert from string to int
multiple_pin_ids = [int(numeric_string) for numeric_string in multiple_pin_ids]

# Extract pin ids for EXTI instance on a single pin
for i in range(len(lines_single_pin)):
    for match in re.finditer(pattern_pin_nb, lines_single_pin[i]):
        single_pin_ids.insert(i, match.group())

# Generate needed file
with open(write_file, 'w') as f:
    f.write ("#include \"no_os_error.h\"\n")
    f.write ("#include \"stm32_hal.h\"\n")
    f.write("int stm32_get_exti_irq_id_from_pin(uint8_t pin_nb, IRQn_Type *irq_id)\n{\n")
    f.write("   switch(pin_nb) {\n")
    for i in range(len(lines_single_pin)):
        f.write("       case " + single_pin_ids[i] + ":\n")
        f.write("           *irq_id = " + lines_single_pin[i] + "n;\n")
        f.write("           break;\n")
    for i in range(len(lines_multiple_pins)):
            j = multiple_pin_ids[i*2+1]
            while (j <= multiple_pin_ids[i*2]): 
                f.write("       case " + str(j) + ":\n")
                j = j + 1
            f.write("           *irq_id = " + lines_multiple_pins[i] + "n;\n")
            f.write("           break;\n")
    f.write("       default:\n")
    f.write("           return -ENOSYS;\n")
    f.write("   }\n")
    f.write("   return 0;\n}")
f.close()
