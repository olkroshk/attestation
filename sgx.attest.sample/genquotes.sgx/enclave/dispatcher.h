// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#pragma once
#include <string>
#include "attestation.h"
#include "crypto.h"

using namespace std;

class ecall_dispatcher
{
  private:
    bool m_initialized;
    Crypto* m_crypto;
    Attestation* m_attestation;

  public:
    ecall_dispatcher();
    ~ecall_dispatcher();
    int get_remote_report_with_pubkey(
        uint8_t** pem_key,
        size_t* key_size,
        uint8_t** remote_report,
        size_t* remote_report_size);

  private:
    bool initialize();

    // Generate a remote report for the given data. The SHA256 digest of the
    // data is stored in the report_data field of the generated remote report.
    bool generate_remote_report(
        const uint8_t* data,
        size_t data_size,
        uint8_t** remote_report_buf,
        size_t* remote_report_buf_size);
};
