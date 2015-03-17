#ifndef DECODER_H
#define DECODER_H

#include <string>

namespace huffmanator
{
    enum data_type
    {
        dt_text
    };

    //opaque pointer to decoder
    struct decoder;

    //must be called to initialize internal
    //encode\decode(decode only for now) data structures
    void init_env();

    //create instance of decoder
    decoder * create_decoder(data_type dt);

    //perform huffman decoding of text bitstream
    std::string decode_text(decoder * dec, std::string text);

    //close instance of decoder
    void close_decoder(decoder * dec);
}
#endif // DECODER_H
