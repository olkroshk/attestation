#include "enclave_t.h"
#include "sgx_trts.h"
#include "sgx_error.h"
#include "sgx_report.h"
#include "sgx_utils.h"

uint32_t enclave_create_report(const sgx_target_info_t* p_qe3_target,
                const uint8_t* p_some_data,
		sgx_report_t* p_report)
{
    sgx_report_data_t data = {*p_some_data};
    return sgx_create_report(p_qe3_target, &data, p_report);
}
