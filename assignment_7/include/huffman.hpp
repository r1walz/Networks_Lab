#ifndef HUFFMAN
#define HUFFMAN

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <memory>
#include <algorithm>


#include "utility.hpp"

namespace compression
{
    class huffman
    {
    public:
        huffman(std::string code) :
                text(std::move(code)),
                char_count(256),
                root (nullptr)
        {
            std::for_each(text.begin(), text.end(),
                            [&](char ch) {
                                char_count[static_cast<int>(ch)]++;
                            });
        }

        void generate_hashmap(std::shared_ptr<utility::BinaryTree> top)
        {
			if(top->left == NULL && top->right == NULL) {
				huffman_codes[top->char_code] = top->code;
				return;
			}

			generate_hashmap(top->left);
			generate_hashmap(top->right);
        }

        void generate_codes(std::shared_ptr<utility::BinaryTree> top)
        {
			if(top->left == nullptr && top->right == nullptr)
				return;

			top->left->code.append(top->code + "0");
			top->right->code.append(top->code + "1");

			generate_codes(top->left);
			generate_codes(top->right);
        }

        void print_codes(std::shared_ptr<utility::BinaryTree> top) {

			if(top->left == nullptr && top->right == nullptr) {
				std::cout << top->char_code << " : " << "count: " <<  top->count << ", code: " << top->code << std::endl;
				return;
			}

			print_codes(top->right);
			print_codes(top->left);

		}

        std::string compress()
        {
			std::string size_ = "";
			std::cout << "\n\nHuffman code should look like: \n";

			for(int i = 0 ; i < text.size() ; i++) {

				char ch[2] = {text.at(i), '\0'};
				std::string to_compress = {ch};

				size_ += huffman_codes[to_compress];
			}

			std::cout << size_;

			std::cout << "\nHere is the computer genereated code for all the characters: \n";
			print_codes(root);

			std::cout << "\nThe original text will take " << text.size() * 8 << " bits\n";
			std::cout << "The compressed text will take " << size_.size() << " bits\n";

            return size_;
        }

        std::string generate_tree(std::priority_queue<
                        std::shared_ptr<utility::BinaryTree>,
                        std::vector<std::shared_ptr<utility::BinaryTree>>,
                        utility::huffman::comparator> minHeap)
        {
            std::shared_ptr<utility::BinaryTree> right, left, top;

            while(minHeap.size() > 1)
            {
                right = minHeap.top();
                minHeap.pop();

                left = minHeap.top();
                minHeap.pop();

                top = std::make_shared<utility::BinaryTree>(right, left);
                minHeap.push(top);
            }
            root = top;

            generate_codes(root);
            generate_hashmap(root);

            std::string encoding = compress();

            std::ofstream out("../output.txt");
            out << encoding;
            out.close();

            return encoding;
        }

        std::string encode()
        {
            std::priority_queue<
                        std::shared_ptr<utility::BinaryTree>,
                        std::vector<std::shared_ptr<utility::BinaryTree>>,
                        utility::huffman::comparator> minHeap;

            for (int b = 0; b != 256; ++b)
            {
                if (char_count[b] != 0)
                {
                    char str[2] {static_cast<char>(b), '\0'};
                    std::string to_push {str};

                    std::shared_ptr<utility::BinaryTree> node =
                        std::make_shared<utility::BinaryTree>(char_count[b], to_push);

                    minHeap.push(node);
                }
            }

            std::string encoded = generate_tree(std::move(minHeap));
            return encoded;
        }

        std::string decode(std::string encoded_msg)
        {
            std::string decoded_msg {};

            std::shared_ptr<utility::BinaryTree> curr = root;

            std::for_each(encoded_msg.begin(), encoded_msg.end(),
                        [&](char ch) {
                            if (ch == '0')
                                curr = curr -> left;
                            else
                                curr = curr -> right;

                            if (curr -> left == nullptr && curr -> right == nullptr)
                            {
                                decoded_msg += curr -> char_code;
                                curr = root;
                            }
                        });
            return decoded_msg + '\0';
        }

    private:
        std::string text;
        std::vector<int> char_count;
        std::unordered_map<std::string, std::string> huffman_codes;
        std::shared_ptr<utility::BinaryTree> root;
    };
} // namespace compression


#endif