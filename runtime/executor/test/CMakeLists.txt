# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

# This file should be formatted with
# ~~~
# cmake-format -i CMakeLists.txt
# ~~~
# It should also be cmake-lint clean.
#

cmake_minimum_required(VERSION 3.19)

set(EXECUTORCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

include(${EXECUTORCH_ROOT}/tools/cmake/Test.cmake)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ModuleAddHalf.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleAdd.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleDynamicCatUnallocatedIO.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleIndex.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleLinear.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleLinearProgram.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleLinearProgram.ptd"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleMultipleEntry.pte"
         "${CMAKE_CURRENT_BINARY_DIR}/ModuleSimpleTrain.pte"
  COMMAND
    python3 -m test.models.export_program --modules
    "ModuleAdd,ModuleAddHalf,ModuleDynamicCatUnallocatedIO,ModuleIndex,ModuleLinear,ModuleMultipleEntry,ModuleSimpleTrain"
    --outdir "${CMAKE_CURRENT_BINARY_DIR}" 2> /dev/null
  COMMAND
    python3 -m test.models.export_program --modules "ModuleLinear"
    --external-constants --outdir "${CMAKE_CURRENT_BINARY_DIR}" 2> /dev/null
  COMMAND
    python3 -m test.models.export_delegated_program --modules "ModuleAddMul"
    --backend_id "StubBackend" --outdir "${CMAKE_CURRENT_BINARY_DIR}" || true
  WORKING_DIRECTORY ${EXECUTORCH_ROOT}
)

add_custom_target(
  generated_pte_files
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/ModuleAddHalf.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleAdd.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleDynamicCatUnallocatedIO.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleIndex.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleLinear.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleLinearProgram.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleLinearProgram.ptd"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleMultipleEntry.pte"
          "${CMAKE_CURRENT_BINARY_DIR}/ModuleSimpleTrain.pte"
)

set(test_env
    "DEPRECATED_ET_MODULE_LINEAR_CONSTANT_BUFFER_PATH=${EXECUTORCH_ROOT}/test/models/deprecated/ModuleLinear-no-constant-segment.pte"
    "ET_MODULE_ADD_HALF_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleAddHalf.pte"
    "ET_MODULE_ADD_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleAdd.pte"
    "ET_MODULE_DYNAMIC_CAT_UNALLOCATED_IO_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleDynamicCatUnallocatedIO.pte"
    "ET_MODULE_INDEX_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleIndex.pte"
    "ET_MODULE_LINEAR_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleLinear.pte"
    "ET_MODULE_LINEAR_PROGRAM_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleLinearProgram.pte"
    "ET_MODULE_LINEAR_DATA_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleLinearProgram.ptd"
    "ET_MODULE_MULTI_ENTRY_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleMultipleEntry.pte"
    "ET_MODULE_SIMPLE_TRAIN_PATH=${CMAKE_CURRENT_BINARY_DIR}/ModuleSimpleTrain.pte"
)

et_cxx_test(
  runtime_executor_test SOURCES executor_test.cpp EXTRA_LIBS portable_ops_lib
  portable_kernels
)
add_dependencies(runtime_executor_test generated_pte_files)
set_property(TEST runtime_executor_test PROPERTY ENVIRONMENT ${test_env})

et_cxx_test(
  allocation_failure_stress_test
  SOURCES
  allocation_failure_stress_test.cpp
  EXTRA_LIBS
  portable_ops_lib
  portable_kernels
  extension_data_loader
  extension_runner_util
)
add_dependencies(allocation_failure_stress_test generated_pte_files)
set_property(
  TEST allocation_failure_stress_test PROPERTY ENVIRONMENT ${test_env}
)

et_cxx_test(
  method_test
  SOURCES
  method_test.cpp
  EXTRA_LIBS
  portable_ops_lib
  portable_kernels
  extension_data_loader
  extension_flat_tensor
  extension_runner_util
)
add_dependencies(method_test generated_pte_files)
set_property(TEST method_test PROPERTY ENVIRONMENT ${test_env})

# TODO(T191569140): Enable this test. et_cxx_test(method_meta_test SOURCES
# method_meta_test.cpp EXTRA_LIBS extension_data_loader)

# TODO(T191569140): Enable this test. et_cxx_test( program_test SOURCES
# program_test.cpp EXTRA_LIBS extension_data_loader )

# target_include_directories( program_test PRIVATE
# "${CMAKE_INSTALL_PREFIX}/schema/include"
# "${EXECUTORCH_ROOT}/third-party/flatbuffers/include" )

et_cxx_test(
  kernel_resolution_test SOURCES kernel_resolution_test.cpp EXTRA_LIBS
  extension_data_loader
)
add_dependencies(kernel_resolution_test generated_pte_files)
set_property(TEST kernel_resolution_test PROPERTY ENVIRONMENT ${test_env})

et_cxx_test(
  kernel_integration_test SOURCES kernel_integration_test.cpp EXTRA_LIBS
  extension_data_loader extension_runner_util
)
add_dependencies(kernel_integration_test generated_pte_files)
set_property(TEST kernel_integration_test PROPERTY ENVIRONMENT ${test_env})

# TODO(T191569140): Enable this test. et_cxx_test( backend_integration_test
# SOURCES backend_integration_test.cpp EXTRA_LIBS extension_data_loader
# extension_runner_util )

et_cxx_test(memory_manager_test SOURCES memory_manager_test.cpp)
add_dependencies(memory_manager_test generated_pte_files)
set_property(TEST memory_manager_test PROPERTY ENVIRONMENT ${test_env})

et_cxx_test(
  tensor_parser_test
  SOURCES
  tensor_parser_test.cpp
  EXTRA_LIBS
  portable_ops_lib
  portable_kernels
  extension_data_loader
  extension_runner_util
)
add_dependencies(tensor_parser_test generated_pte_files)
set_property(TEST tensor_parser_test PROPERTY ENVIRONMENT ${test_env})

target_include_directories(
  tensor_parser_test
  PRIVATE "${CMAKE_INSTALL_PREFIX}/schema/include"
          "${EXECUTORCH_ROOT}/third-party/flatbuffers/include"
)
