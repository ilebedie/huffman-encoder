#include "decoder.h"
#include <stdexcept>
#include <vector>

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ " : " S2(__LINE__)


namespace huffmanator
{
static bool g_initialized = false;


class huffman_tree
{
    friend class bit_reader;

    struct tree_node
    {
        tree_node* left;
        tree_node* right;
        char symbol;

        tree_node() : left(nullptr), right(nullptr), symbol(0)
        {}
    };

    tree_node* m_root;
public:
    huffman_tree()
    :m_root(new tree_node){}

	void destroy_tree(tree_node * node) noexcept
    {
        if ( node != nullptr )
        {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }

    }
    ~huffman_tree()
    {
        destroy_tree(m_root);
    }

    huffman_tree(std::vector<std::pair<std::string, char>>& huff_code)
        :m_root(new tree_node)
    {
        insert_codes(huff_code);
    }

    void insert_codes(std::vector<std::pair<std::string, char>>& huff_code)
    {
        for(auto &code: huff_code )
            insert_path(code);
    }

    void insert_path(std::pair<std::string, char> path)
    {
        if (!isalpha(path.second))
            throw std::invalid_argument(std::string("can compress only alphabetic characters") + LOCATION );;

        tree_node* cursor = m_root;

        try
        {
            for(char c: path.first)
            {
                if (c=='0')
                {
                    tree_node * node = new tree_node;
                    cursor->left = node;
                    cursor = node;
                }
                else if(c=='1')
                {
                    tree_node * node = new tree_node;
                    cursor->right=node;
                    cursor = node;
                }
                else
                    throw std::invalid_argument(std::string("Wrong initialization string") + LOCATION );
            }
        }
        catch(...)
        {
            destroy_tree(m_root);
            throw;
        }

        cursor->symbol = path.second;
    }
};

static huffman_tree g_ht;

const huffman_tree& get_huffman_tree()
{
	return g_ht;
}


class bit_reader
{
	int  m_bit_position;
	char m_current_character;
	bool m_found_eos;
	const huffman_tree& m_ht;
	huffman_tree::tree_node* tree_walker;
public:
	bit_reader(char c)
	 : m_ht(get_huffman_tree())
	{
		put_char(c);
	}

	bit_reader()
		: m_ht(get_huffman_tree())
	{
		m_found_eos = true;
		m_bit_position = -1;
	}

	void put_char(char c)
	{
		m_current_character = c;
		m_bit_position = 7;
		m_found_eos = false;
	}

	bool need_next_char()
	{
		return m_bit_position == -1;
	}

	bool end_of_stream()
	{
		return m_found_eos;
	}

	char read_character()
	{
		while(m_bit_position!=-1)
		{
			if(m_current_character & 1<<m_bit_position)
				tree_walker = tree_walker->left;
			else
				tree_walker = tree_walker->right;

			m_bit_position--;

			if (tree_walker->symbol != 0)
				return tree_walker->symbol;
		}
		return 0; //end of character
	}

	void clear()
	{
		m_bit_position = 7;
		m_found_eos = true;
	}
};


class decoder_impl
{
	std::string m_result;
	bit_reader m_br;
public:
	decoder_impl()
	{
	}

	void push_text(const std::string& in_text )
	{
		auto text_cursor = in_text.begin();
		while(text_cursor != in_text.end())
		{
			m_br.put_char( *text_cursor++ );
			char res = 0;
			do
			{
				res = m_br.read_character();
			}
			while( res==0 );
			if(!m_br.end_of_stream())
				break;

			m_result.push_back(res);
		}
	}

	std::string get_result()
	{
		return m_result;
	}

	void flush()
	{
		m_result.clear();
		m_br.clear();
	}
};

struct decoder
{
	decoder_impl* _di;
};

//create instance of decoder
decoder * create_decoder( data_type dt)
{
	if(!g_initialized)
		throw std::logic_error(std::string("Not initialized")+LOCATION);

	decoder_impl *pDecoderImpl = new decoder_impl;

	decoder* dec = new decoder;
	dec->_di = pDecoderImpl;

	return dec;
}

//perform huffman decoding of text bitstream
std::string decode_text(decoder * dec, std::string text )
{
	dec->_di->push_text(text);
	return dec->_di->get_result();
}

//close instance of decoder
void close_decoder(decoder * dec)
{
	if(dec)
	{
		delete dec->_di;
		delete dec;
	}

}

void init_env()
{
    if(g_initialized)
        return;

    using namespace std;
	vector<pair<string,char> > huffman_code =
	{
		{"000",    'e'},
		{"0010",   't'},
		{"0011",   'a'},
		{"0100",   'o'},
		{"0101",   'n'},
		{"0110",   'r'},
		{"0111",   'i'},
		{"10000",  'h'},
		{"10001",  's'},
		{"10010",  'd'},
		{"10011",  'l'},
		{"10100",  'c'},
		{"10101",  'u'},
		{"10110",  'm'},
		{"10111",  'f'},
		{"11000",  'p'},
		{"11001",  'y'},
		{"11010",  'b'},
		{"11011",  'w'},
		{"11100",  'g'},
		{"11101",  'v'},
		{"111100", 'j'},
		{"111101", 'k'},
		{"111110", 'x'},
		{"1111110",'q'},
		{"1111111",'z'}
	};
    g_ht.insert_codes(huffman_code);
    g_initialized = true;
}

}
