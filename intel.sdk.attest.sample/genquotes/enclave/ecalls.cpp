// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

/*
#include "dispatcher.h"
#include "enclave_t.h"

static ecall_dispatcher dispatcher(NULL);

int get_remote_report_with_pubkey(
    uint8_t** pem_key,
    size_t* key_size,
    uint8_t** remote_report,
    size_t* remote_report_size)
{
    return dispatcher.get_remote_report_with_pubkey(
        pem_key, key_size, remote_report, remote_report_size);
}
*/

/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 */
#include "enclave_t.h"

#include "sgx_trts.h"
#include "sgx_error.h"
#include "sgx_report.h"
#include "sgx_utils.h"


uint32_t enclave_create_report(const sgx_target_info_t* p_qe3_target, sgx_report_t* p_report)
{
    sgx_report_data_t report_data = { 0 };

    // TODO: pass the data into the funcation, calc sha256 and get report for the hash, send the report and non-hashed data to MAA

    // Generate the report for the app_enclave
    sgx_status_t  sgx_error = sgx_create_report(p_qe3_target, &report_data, p_report);

    return sgx_error;
}
