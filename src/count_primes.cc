#include "nemo/count_primes.hh"


namespace nemo {

  bool lookup(set<string>& s, const string& key)
{
    auto iter = s.find(key);
    return iter != s.end();
}

string getRandStr(size_t len)
{
    static std::mt19937 gen{random_device{}()};
    uniform_int_distribution<short> dis('A', 'z');
    string s;
    s.resize(len);
    for (size_t i = 0; i < len; ++i)
        s += (char)dis(gen);
    return s;
}

vector<string> getInput(size_t keysNum, size_t keyLen)
{
    vector<string> input{keysNum};
    generate(input.begin(), input.end(), [keyLen] {
        return getRandStr(keyLen);
    });
    return input;
}

void buildContainer(set<string>& container, const vector<string>& input)
{
    transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const string& str) {
        return str;
    });
}

void stringCompare(){
  const size_t inputSize = 1000000,
    lookupsNum = 1000000,
    strLen = 16;
  vector<string> input = getInput(inputSize, strLen);
  vector<string> lookups = getInput(lookupsNum, strLen);
  set<string> ssc;
  buildContainer(ssc, input);
  for (const string& s : lookups)
    lookup(ssc, s);
}


    const char* getRawStr(const char* str) { return str; }
    const char* getRawStr(const std::string& str) { return str.data(); }
    const char* getRawStr(const string_view& str) { return str.data(); }

    //
    // Helper function for swapping bytes, turning the big endian order in the string into
    // a proper little endian number. For now, only GCC is supported due to the use of intrinsics.
    //
    void flipBytes(uint16_t& val) { val = __builtin_bswap16(val); }
    void flipBytes(uint32_t& val) { val = __builtin_bswap32(val); }
    void flipBytes(uint64_t& val) { val = __builtin_bswap64(val); }
    void flipBytes(kdmt128_t& val) {
        val.lsbs = __builtin_bswap64(val.lsbs);
        val.msbs = __builtin_bswap64(val.msbs);
    }
    
} /* end namespace nemo */
