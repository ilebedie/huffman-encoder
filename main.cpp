#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "decoder.h"


class dec_handler
{
    huffmanator::decoder * m_dec;
public:
    dec_handler()
        : m_dec(create_decoder(huffmanator::dt_text))
    {
    }
    ~dec_handler()
    {
        huffmanator::close_decoder(m_dec);
    }
    void decode(std::string text)
    {
        huffmanator::decode_text(m_dec, text);
    }
};


void DecodeTextFile(std::string in_file_path, std::string out_file_path)
{
    using namespace std;
    ifstream in(in_file_path.c_str());
    string str;
    string text;
    while(getline(in, str))
    {
        text += str;
        text.push_back('\n');
    }

    dec_handler dec;
    dec.decode(text);
}

int main(int ac, char ** av)
{
    using namespace std;
    namespace po = boost::program_options;

    if(ac < 2)
        cout<<"huffmanator --help --in=input.txt --out=output.bs --op=encode --datatype=text\n";

    huffmanator::init_env();

    string op;
    string datatype;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("in", po::value<string>(), "input file" )
        ("out", po::value<string>(), "output file" )
        ("op",po::value<string>(&op)->default_value( "decode"), "opertion type (encode | decode)")
        ("datatype", po::value<string>(&datatype)->default_value("text"), "data type (text)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("in"))
    {
        cout << "input file:" << vm["in"].as<string>() << ".\n";
    }
    else
    {
        cout << "input file was not set\n";
        return 1;
    }

    if (vm.count("out"))
    {
        cout << "output file:" << vm["out"].as<string>() << ".\n";
    }
    else
    {
        cout << "output file was not set\n";
        return 1;
    }

    cout << "op:" << vm["op"].as<string>() << ".\n";
    if (vm["op"].as<string>() == string("decode") &&  vm["datatype"].as<string>() == string("text"))
        DecodeTextFile(vm["in"].as<string>(), vm["out"].as<string>());
    else
        cout<<"only decode of text files is implemented\n";

    return 0;
}
