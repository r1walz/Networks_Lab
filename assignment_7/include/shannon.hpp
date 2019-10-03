#ifndef SHANNON
#define SHANNON

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <memory>
#include <algorithm>


#include "utility.hpp"

namespace compression
{
    class shannon
    {
    public:
        shannon(std::string content) :
                text (std::move(content))
        {
            std::unordered_map<char, int> freqs;
            int total = 0;
            std::for_each(text.begin(), text.end(),
                        [&](char ch)
                        {
                            freqs[ch]++;
                            total++;
                        });
            tableSize = static_cast<int>(freqs.size());

            ptable.reserve(tableSize);

            std::for_each(freqs.begin(), freqs.end(),
                        [&](auto kv)
                        {
                           ptable.emplace_back(kv.first,
                                    (static_cast<float>(kv.second) / static_cast<float>(total)))
                           ;
                        });

            std::sort(ptable.begin(), ptable.end(), utility::shannon::comparator);
        }

        std::string encode()
        {
            generate_code(0, tableSize - 1);
            std::string encoded = "";

            std::ofstream start("../output.txt");
            std::cout << "Probability table and the code assigned are as follows:\n";
            for (int i = 0; i < tableSize; ++i)
            {
                start << ptable[i].ch
                        << "\t"
                        << ptable[i].p
                        << "\t"
                        << shannon_code[ptable[i].ch]
                        << "\n";
                std::cout << ptable[i].ch
                        << "\t"
                        << ptable[i].p
                        << "\t"
                        << shannon_code[ptable[i].ch]
                        << std::endl;
            }
            start.close();

            std::for_each(text.begin(), text.end(),
                        [&](char ch)
                        {
                            encoded += shannon_code[ch];
                        });
            std::cout << "Encoded message:\n";
            std::cout << encoded << std::endl;

            std::ofstream out;
            out.open("../output.txt", std::ios_base::app);
            out << encoded;
            out.close();

            std::cout << "The original text will take "
                      << text.size() * 8
                      << '\n'
                      << "The compressed text will take "
                      << encoded.size()
                      << '\n';
            return encoded + '\0';
        }

        std::string decode(std::string encoded_msg)
        {
            std::cout << "Decoding encodded message:\n";
            std::string accum = "";
            std::string decoded = "";
            std::for_each(encoded_msg.begin(), encoded_msg.end(),
                        [&](char ch)
                        {
                            char str[1] {ch};
                            accum.append(str);

                            for(auto kv: shannon_code)
                            {
                                if(accum == kv.second)
                                {
                                    accum = "";
                                    std::cout << kv.first;
                                    decoded.append(kv.second);
                                }
                            }
                        });
            std::cout << std::endl;
            return decoded;
        }


    private:
        void generate_code(const int li, const int ri)
        {
            int i, isp;
            float p;
            float pfull;
            float phalf;
            if(li == ri)
                return;
            else if(ri - li == 1)
            {
                shannon_code[ptable[li].ch] += '0';
                shannon_code[ptable[ri].ch] += '1';
            }
            else
            {
                // Calculate sum of probabilities at specified interval
                pfull = 0.0;
                for (i = li; i <= ri; ++i)
                    pfull += ptable[i].p;

                // Search for center
                p = 0;
                isp = -1;   // splitting index

                phalf = pfull * 0.5f;

                for (i = li; i <= ri; ++i)
                {
                    p += ptable[i].p;
                    if(p <= phalf)
                    {
                        shannon_code[ptable[i].ch] += '0';
                    }
                    else
                    {
                        shannon_code[ptable[i].ch] += '1';
                        if(isp < 0) isp = i;
                    }
                }

                if (isp < 0)    isp = li + 1;

                generate_code(li, isp - 1);
                generate_code(isp, ri);
            }
        }

        std::string text;
        std::unordered_map<char, std::string> shannon_code;
        int tableSize;
        std::vector<utility::ProbabilityNode> ptable;
    };
} // namespace shannon


#endif