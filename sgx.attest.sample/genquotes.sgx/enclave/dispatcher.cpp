// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.
#include "dispatcher.h"

#include <stdio.h>

ecall_dispatcher::ecall_dispatcher()
    : m_crypto(NULL), m_attestation(NULL)
{
    m_initialized = initialize();
}

ecall_dispatcher::~ecall_dispatcher()
{
    if (m_crypto)
        delete m_crypto;
}

bool ecall_dispatcher::initialize()
{
    bool ret = false;
    m_crypto = new Crypto();
    if (m_crypto != NULL)
    {
        ret = true;
    }
    return ret;
}

/**
 * Return the public key of this enclave along with the enclave's remote report.
 * The enclave that receives the key will use the remote report to attest this
 * enclave.
 */
int ecall_dispatcher::get_remote_report_with_pubkey(
    uint8_t** pem_key,
    size_t* key_size,
    uint8_t** remote_report,
    size_t* remote_report_size)
{
    uint8_t pem_public_key[512];
    uint8_t* report = NULL;
    size_t report_size = 0;
    uint8_t* key_buf = NULL;
    int ret = 1;

    printf("get_remote_report_with_pubkey");
    if (m_initialized == false)
    {
        printf("ecall_dispatcher initialization failed.");
        goto exit;
    }

    m_crypto->retrieve_public_key(pem_public_key);

    // Generate a remote report for the public key so that the enclave that
    // receives the key can attest this enclave.
    if (this->generate_remote_report(
            pem_public_key, sizeof(pem_public_key), &report, &report_size))
    {
        // Allocate memory on the host and copy the report over.
        *remote_report = (uint8_t*)oe_host_malloc(report_size);
	//TODO [olkroshk] replace oe_host_malloc
        if (*remote_report == NULL)
        {
		//TODO olkroshk
            //ret = OE_OUT_OF_MEMORY;
            ret = -1;//OE_OUT_OF_MEMORY;
            goto exit;
        }
        memcpy(*remote_report, report, report_size);
        *remote_report_size = report_size;
        oe_free_report(report);

        key_buf = (uint8_t*)oe_host_malloc(512);
	//TODO [olkroshk] replace oe_host_malloc
        if (key_buf == NULL)
        {
       		//TODO olkroshk
            //ret = OE_OUT_OF_MEMORY;
            ret = -1;//OE_OUT_OF_MEMORY;
            goto exit;
        }
        memcpy(key_buf, pem_public_key, sizeof(pem_public_key));

        *pem_key = key_buf;
        *key_size = sizeof(pem_public_key);

        ret = 0;
        printf("get_remote_report_with_pubkey succeeded");
    }
    else
    {
        printf("get_remote_report_with_pubkey failed.");
    }

exit:
    if (ret != 0)
    {
        if (report)
	{
		//TODO
        //    oe_free_report(report);
	}
        if (key_buf)
	{
		//TODO
          //  oe_host_free(key_buf);
	}
        if (*remote_report)
	{
		//TODO
            //oe_host_free(*remote_report);
	}
    }
    return ret;
}

bool ecall_dispatcher::generate_remote_report(
        const uint8_t* data,
        size_t data_size,
        uint8_t** remote_report_buf,
        size_t* remote_report_buf_size)
{
// TODO write me
    bool ret = false;
    uint8_t sha256[32];
    oe_result_t result = OE_OK;
    uint8_t* temp_buf = NULL;

    if (m_crypto->Sha256(data, data_size, sha256) != 0)
    {
        goto exit;
    }

    // To generate a remote report that can be attested remotely by an enclave
    // running  on a different platform, pass the
    // OE_REPORT_FLAGS_REMOTE_ATTESTATION option. This uses the trusted
    // quoting enclave to generate the report based on this enclave's local
    // report.
    // To generate a remote report that just needs to be attested by another
    // enclave running on the same platform, pass 0 instead. This uses the
    // EREPORT instruction to generate this enclave's local report.
    // Both kinds of reports can be verified using the oe_verify_report
    // function.
//TODO olkroshk - repalce this call
    result = oe_get_report(
        OE_REPORT_FLAGS_REMOTE_ATTESTATION,
        sha256, // Store sha256 in report_data field
        sizeof(sha256),
        NULL, // opt_params must be null
        0,
        &temp_buf,
        remote_report_buf_size);
    if (result != OE_OK)
    {
        printf("oe_get_report failed.");
        goto exit;
    }
    *remote_report_buf = temp_buf;
    ret = true;
    printf("generate_remote_report succeeded.");
exit:
    return ret;
}

