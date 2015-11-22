#include <inttypes.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <boost/format.hpp>

using format = boost::format;

class CPack {
    protected:
        std::vector<char> buf;
        std::stack<size_t> lpsStack;
        CPack& append(const char* data, size_t len) {
            buf.insert(buf.end(), &data[0], &data[len]);
            return *this;
        }
        template <typename T>
        CPack& charSize(const T val) {
            return append(reinterpret_cast<const char*>(&val), sizeof(val));
        }
    public:
        CPack() {}
        CPack(size_t cap) {buf.reserve(cap);}
        ~CPack() {}
        std::string hexdump() {
            std::ostringstream out;
            const size_t lwidth = 16;
            const size_t cwidth = 8;
            std::ostringstream strl;
            int i;
            for (i = 0; i<buf.size(); i++) {
                if (i % lwidth == 0) {
                    out << format("%|04x|") % i;
                    strl.put('|');
                }
                if (i % cwidth == 0) {
                    out << " ";
                    strl.put(' ');
                }
                out << format(" %|02x|") % (uint32_t)buf[i];
                strl.put(isprint(buf[i]) ? buf[i] : '.');
                if (i % lwidth == lwidth-1) {
                    out << "  " << strl.str() << std::endl;
                    strl.str("");
                    strl.clear();
                }
            }
            if (i % lwidth != 0) {
                for (int j = 0; j < (lwidth-i%lwidth)*3 + 1; j++)
                    out << " ";
                out << "  " << strl.str() << std::endl;
            }
            return out.str();
        }
        CPack& Str(const std::string& str) {append(str.c_str(), str.size());}
        CPack& U8(const uint8_t val) {charSize(val);}
        CPack& U16(const uint16_t val) {charSize(val);}
        CPack& U32(const uint32_t val) {charSize(val);}
        CPack& U64(const uint64_t val) {charSize(val);}
        CPack& I8(const  int8_t val) {charSize(val);}
        CPack& I16(const int16_t val) {charSize(val);}
        CPack& I32(const int32_t val) {charSize(val);}
        CPack& I64(const int64_t val) {charSize(val);}

        CPack& lpsStart() {
            lpsStack.push(buf.size());
            return this->U32(0);
        };

        CPack& lpsStop() {
            size_t off = lpsStack.top();
            lpsStack.pop();
            uint32_t* lenp = reinterpret_cast<uint32_t*>(&buf[off]);
            *lenp = buf.size() - off - sizeof(uint32_t);
            return *this;
        }
};

int main()
{
    CPack pk{10};
    pk
        .Str("zxczxc")
        .U32(4)
        .U32(2)
        .lpsStart()
        .Str("asd")
        .lpsStop()
        .lpsStart()
        .lpsStop();
    std::cout << pk.hexdump();
    return 0;
}
