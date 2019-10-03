#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "../include/huffman.hpp"
#include "../include/shannon.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Too few arguements. Add file name as argument.\n";
        exit(0);
    }

    std::string filename {argv[1]};

    std::ifstream ifs("../"+filename);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );

    if (content.empty())
    {
        std::cerr << "File empty.\n";
        exit(0);
    }

    int choice = 0;
    std::cout << "Select the compression algorithm: Huffman (1), Shannon (2): ";
    std::cin >> choice;


    switch (choice)
    {
    case 1:
        {
            compression::huffman huff(content);
            std::string encoded_msg = huff.encode();
            std::string decode_msg = huff.decode(encoded_msg);
            std::cout << "Here is the decoded message:\n" << decode_msg << std::endl;
            break;
        }

    case 2:
        {
            compression::shannon shan(content);
            std::string encoded_msg = shan.encode();
            std::string decode_msg = shan.decode(encoded_msg);
            break;
        }

    default:
        std::cerr << "Wrong selection.\n";
        exit(0);
        break;
    }
}