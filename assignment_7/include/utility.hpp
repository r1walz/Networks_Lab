#ifndef UTILITY
#define UTILITY

#include <string>
#include <memory>

namespace utility
{
    class BinaryTree
    {
    public:

        int count;
        std::string char_code;
        std::string code;
        std::shared_ptr<BinaryTree> right;
        std::shared_ptr<BinaryTree> left;

        BinaryTree(int count, std::string ch) : 
                    count (count),
                    char_code (ch),
                    right (nullptr),
                    left (nullptr) {}

        BinaryTree(std::shared_ptr<BinaryTree> right,
                std::shared_ptr<BinaryTree> left) :
                    count (right -> count + left -> count),
                    char_code (right -> char_code + left -> char_code),
                    right (right),
                    left (left) {}
    };

    class ProbabilityNode
    {
    public:
        ProbabilityNode() {};
        ProbabilityNode(char ch, float p) : ch(ch), p(p) {}
    
        char ch;
        float p;
    };

    namespace shannon
    {
        bool comparator (ProbabilityNode p1,
                            ProbabilityNode p2)
        {
            return (p1.p > p2.p);
        }
    }

    namespace huffman
    {
        struct comparator
        {
            bool operator() (std::shared_ptr<BinaryTree> l, 
                        std::shared_ptr<BinaryTree> r)
            {
                return (l -> count > r -> count);
            }
        };
    }
} // namespace utility


#endif