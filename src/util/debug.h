#pragma once

#include "../vm/vm.h"

/**
 * Disassembles the current instruction, and prints stack contents
 * @param vm Pointer to the VM
 */
void verbose_log(vm *vm);