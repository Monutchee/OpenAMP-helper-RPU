#!/usr/bin/env bash
set -euo pipefail

TEST_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
TEST_BUILD_DIR="$(mktemp -d)"
trap 'rm -rf -- "${TEST_BUILD_DIR}"' EXIT

for irq_port in legacy extension; do
    defines=()
    if [[ "${irq_port}" == extension ]]; then
        defines+=(-DMNC_LIBMETAL_XLNX_EXTENSION)
    fi
    "${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
        "${defines[@]}" -I"${TEST_DIR}/stubs" \
        -I"${TEST_DIR}/../../machine/zynqmp_r5" \
        "${TEST_DIR}/test.c" -o "${TEST_BUILD_DIR}/${irq_port}"
    "${TEST_BUILD_DIR}/${irq_port}"
    printf 'ZynqMP IRQ %s tests passed\n' "${irq_port}"
done
