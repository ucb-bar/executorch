/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <sys/sysconf.h>

namespace executorch::extension::cpuinfo {

// Ignore revisions (last digit (4 LSBs))
#define CPUINFO_ARM_MIDR_CORTEX_A520 UINT32_C(0x410FD800)
#define CPUINFO_ARM_MIDR_CORTEX_A53 UINT32_C(0x410FD030)
#define CPUINFO_ARM_MIDR_CORTEX_A55 UINT32_C(0x410FD050)
#define CPUINFO_ARM_MIDR_CORTEX_A57 UINT32_C(0x410FD070)

#define RIVISION_MASK UINT32_C(0xFFFFFFF0)

uint32_t get_num_performant_cores() {
  uint32_t num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  return num_cores;
}

} // namespace executorch::extension::cpuinfo
