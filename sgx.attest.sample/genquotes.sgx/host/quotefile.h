#pragma once

#include <openenclave/host.h>

class quote_file
{
  private:
    uint8_t* _enclave_held_data;
    size_t _enclave_held_data_size;
    uint8_t* _quote;
    size_t _quote_size;

  public:
    quote_file(int8_t *quote, size_t quote_size, uint8_t *ehd, size_t ehd_size);
    void write_to_json_file(const char *directory, const char* json_file_name);
    void write_to_json_file(FILE *fp);

  private:
    const char *format_hex_buffer (char *buffer, uint max_size, uint8_t *data, size_t size);
};
