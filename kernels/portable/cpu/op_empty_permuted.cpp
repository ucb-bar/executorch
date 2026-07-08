/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <executorch/runtime/kernel/kernel_includes.h>
#include <executorch/runtime/platform/assert.h>

namespace torch {
namespace executor {
namespace native {

using executorch::aten::IntArrayRef;
using executorch::aten::Tensor;

/*
 * Empty (uninitialized) out tensor with an explicit physical layout.
 *
 * empty_permuted.out(SymInt[] size, int[] physical_layout, *, Tensor(a!) out)
 *     -> Tensor(a!)
 *
 * The `out` tensor is pre-allocated by the ahead-of-time memory planner, whose
 * TensorSpec already encodes the physical (dim-order) layout derived from this
 * node's strides. The kernel therefore only needs to make `out` the requested
 * logical shape and return it (its contents are intentionally uninitialized);
 * `physical_layout` is advisory and already reflected in `out`'s allocation.
 * This mirrors `empty.out` exactly, which is why ExecuTorch's portable library
 * had no separate kernel and graphs emitting `empty_permuted` (e.g. SmolVLA)
 * failed method-load with "instructions don't have corresponding operator
 * registered".
 */
Tensor& empty_permuted_out(
    KernelRuntimeContext& context,
    IntArrayRef size,
    IntArrayRef physical_layout,
    Tensor& out) {
  (void)context;
  (void)physical_layout;

  // Resize for dynamic shape; a no-op for the common static-shape case where
  // the planner already sized `out`.
  ET_KERNEL_CHECK_MSG(
      context,
      resize_tensor(out, size) == Error::Ok,
      InvalidArgument,
      out,
      "Failed to resize output tensor.");

  return out;
}

} // namespace native
} // namespace executor
} // namespace torch
