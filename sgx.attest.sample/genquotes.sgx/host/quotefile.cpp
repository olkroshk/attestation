#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "quotefile.h"

quote_file::quote_file(uint8_t *quote, size_t quote_size, uint8_t *ehd, size_t ehd_size)
: _quote(quote),
  _quote_size(quote_size),
  _enclave_held_data(ehd),
  _enclave_held_data_size(ehd_size)
{
}

void quote_file::write_to_json_file(const char *directory, const char* json_file_name)
{
    struct stat st = {0};
    if (stat(directory, &st) == -1) {
        mkdir(directory, 0700);
    }

    char json[1024];
    sprintf(json, "%s/%s", directory, json_file_name);

    FILE *fp = fopen(json, "w");
    write_to_json_file(fp);
    fclose(fp);
}

void quote_file::write_to_json_file (FILE *fp)
{
    const int hex_buffer_size = 1024*64;
    char hexBuffer[hex_buffer_size];

    // TODO olkroshk: PARSE QUOTE, GET ANEEDED DATA
    fprintf(fp, "%s\n", "{");
    fprintf(fp, "  \"Type\": %d,\n", (int) _parsedReport.type);
    fprintf(fp, "  \"MrEnclaveHex\": \"%s\",\n", format_hex_buffer(hexBuffer, hex_buffer_size, _parsedReport.identity.unique_id, OE_UNIQUE_ID_SIZE));
    fprintf(fp, "  \"MrSignerHex\": \"%s\",\n", format_hex_buffer(hexBuffer, hex_buffer_size, _parsedReport.identity.signer_id, OE_SIGNER_ID_SIZE));
    fprintf(fp, "  \"ProductIdHex\": \"%s\",\n", format_hex_buffer(hexBuffer, hex_buffer_size, _parsedReport.identity.product_id, OE_PRODUCT_ID_SIZE));
    fprintf(fp, "  \"SecurityVersion\": %u,\n", (int) _parsedReport.identity.security_version);
    fprintf(fp, "  \"Attributes\": %u,\n", (int) _parsedReport.identity.attributes);
    fprintf(fp, "  \"QuoteHex\": \"%s\",\n", format_hex_buffer(hexBuffer, hex_buffer_size, _quote, _quote_size));
    fprintf(fp, "  \"EnclaveHeldDataHex\": \"%s\"\n", format_hex_buffer(hexBuffer, hex_buffer_size, _enclave_held_data, _enclave_held_data_size));
    fprintf(fp, "%s\n", "}");
}

const char *quote_file::format_hex_buffer (char *buffer, uint max_size, uint8_t *data, size_t size)
{
    if (size * 2 >= max_size)
        return "DEADBEEF";

    for (int i=0; i < size; i++)
    {
        sprintf(&buffer[i*2], "%02X", data[i]);
    }
    buffer[size*2+1] = '\0';
    return buffer;
}


