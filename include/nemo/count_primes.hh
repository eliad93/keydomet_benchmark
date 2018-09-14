#ifndef NEMO_FOO_H_
#define NEMO_FOO_H_

#include <iostream>
#include <string>
#include <cstring>
#include <experimental/string_view>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
#include <fstream>
#include "MyString.hh"

using namespace std;

namespace nemo {

    using std::experimental::string_view;

    enum class KeyDometSize : uint8_t
    {
        SIZE_16BIT  = 2,
        SIZE_32BIT  = 4,
        SIZE_64BIT  = 8,
        SIZE_128BIT = 16
    };

    //constexpr auto KeyDometSizeToUse = KeyDometSize::SIZE_128BIT;
    constexpr auto KeyDometSizeToUse = KeyDometSize::SIZE_64BIT;
    //constexpr auto KeyDometSizeToUse = KeyDometSize::SIZE_16BIT;

    template<KeyDometSize>
    struct KeyDometStorage; // Unexpected KeyDometSize value!

    template<> struct KeyDometStorage<KeyDometSize::SIZE_16BIT>  { using type = uint16_t; };
    template<> struct KeyDometStorage<KeyDometSize::SIZE_32BIT>  { using type = uint32_t; };
    template<> struct KeyDometStorage<KeyDometSize::SIZE_64BIT>  { using type = uint64_t; };

    struct kdmt128_t
    {
        using HalfType = typename KeyDometStorage<KeyDometSize::SIZE_64BIT>::type;
        HalfType msbs;
        HalfType lsbs;

        kdmt128_t() = default;
        constexpr kdmt128_t(HalfType l) : msbs{0}, lsbs{l} {}
        constexpr kdmt128_t(HalfType m, HalfType l) : msbs{m}, lsbs{l} {}
        constexpr kdmt128_t operator&(const kdmt128_t& other) const
        {
            return {(msbs & other.msbs), (lsbs & other.lsbs)};
        }
        constexpr bool operator<(const kdmt128_t& other) const
        {
            return (msbs < other.msbs) || (msbs == other.msbs && lsbs < other.lsbs);
        }
        constexpr bool operator==(const kdmt128_t& other) const
        {
            return (msbs == other.msbs) && (lsbs == other.lsbs);
        }
        constexpr bool operator!=(const kdmt128_t& other) const
        {
            return (msbs != other.msbs) || (lsbs != other.lsbs);
        }
    };
    template<> struct KeyDometStorage<KeyDometSize::SIZE_128BIT> { using type = kdmt128_t; };

    //
    // Helper functions to provide access to the raw c-string array.
    // New string types should add an overload, which uses that type's API.
    //
static const char* getRawStr(const char* str) { return str; }
static const char* getRawStr(const std::string& str) { return str.data(); }
static const char* getRawStr(const string_view& str) { return str.data(); }

    //
    // Helper function for swapping bytes, turning the big endian order in the string into
    // a proper little endian number. For now, only GCC is supported due to the use of intrinsics.
    //
static void flipBytes(uint16_t& val) { val = __builtin_bswap16(val); }
static void flipBytes(uint32_t& val) { val = __builtin_bswap32(val); }
static void flipBytes(uint64_t& val) { val = __builtin_bswap64(val); }
static void flipBytes(kdmt128_t& val) {
    val.lsbs = __builtin_bswap64(val.lsbs);
    val.msbs = __builtin_bswap64(val.msbs);
}

    //
    // Helper function that turns the string's keydomet into a number.
    // TODO - Can be optimized, e.g., by using a cast when the string's buffer is known to be large enough
    // (namely, SSO is used; zero padding still required based on actual string size).
    //
    template<typename KeyDometT, typename StrImp>
    KeyDometT strToPrefix(const StrImp& str)
    {
        const char* cstr = getRawStr(str);
        KeyDometT trg;
        strncpy((char*)&trg, cstr, sizeof(trg)); // short strings are padded with zeros
        flipBytes(trg);
        return trg;
    }

    template<KeyDometSize SIZE>
    class KeyDomet
    {

    public:

        using KeyDometType = typename KeyDometStorage<SIZE>::type;

        template<typename StrImp>
        explicit KeyDomet(const StrImp& str) : val{strToPrefix<KeyDometType>(str)}
        {
        }

        KeyDomet(const KeyDomet& other) : val(other.val)
        {}

        KeyDomet(KeyDomet&&) noexcept = default;

        KeyDomet& operator=(KeyDomet&&) noexcept = default;
        KeyDomet& operator=(const KeyDomet&) noexcept = default;

        KeyDometType getVal() const
        {
            return val;
        }

        bool operator<(const KeyDomet<SIZE>& other) const
        {
            return this->val < other.val;
        }

        bool operator==(const KeyDomet<SIZE>& other) const
        {
            return this->val == other.val;
        }

        bool operator!=(const KeyDomet<SIZE>& other) const
        {
            return this->val != other.val;
        }

        bool stringShorterThanKeydomet() const
        {
            // if the last byte/char of the keydomet is all zeros, the string must
            // have been shorter than the keydomet's capacity.
            // note: this won't always be correct when working with Unicode strings!
            constexpr KeyDometType LastByteMask = 0xFF;
            return (val & LastByteMask) == 0;
        }

    private:

        KeyDometType val; // non-const to allow move assignment (useful in vector resizing and algorithms)

    };

    template<class StrImp_, KeyDometSize Size_>
    class KeyDometStr
    {

    public:

        using StrImp = StrImp_;
        static constexpr KeyDometSize Size = Size_;

        explicit KeyDometStr(const StrImp_& other) :
                prefix{other}, str{other}
        {}

        KeyDometStr(const KeyDometStr& other) :
                prefix{other.prefix}, str{other.str}
        {}

        KeyDometStr(KeyDometStr&&) noexcept = default;

        KeyDometStr& operator=(KeyDometStr&&) noexcept = default;
        KeyDometStr& operator=(const KeyDometStr&) noexcept = default;

        template<typename OtherImp, KeyDometSize OtherSize>
        friend class KeyDometStr;

        template<typename Imp>
        int compare(const KeyDometStr<Imp, Size_>& other) const
        {
            if (this->prefix != other.prefix)
            {
                return diffAsOneOrMinusOne(this->prefix, other.prefix);
            }
            else
            {
                if (this->prefix.stringShorterThanKeydomet())
                {
                    return 0;
                }
                return strcmp(getRawStr(str) + sizeof(Size_), getRawStr(other.str) + sizeof(Size_));
            }
        }

        template<typename Imp>
        bool operator<(const KeyDometStr<Imp, Size_>& other) const
        {
            return compare(other) < 0;
        }

        template<typename Imp>
        bool operator==(const KeyDometStr<Imp, Size_>& other) const
        {
            return compare(other) == 0;
        }

        const KeyDomet<Size_>& getPrefix() const
        {
            return prefix;
        }

        const StrImp_& getStr() const
        {
            return str;
        }

        size_t size() const noexcept {
            return str.size() + sizeof(prefix);
        }

        size_t capacity() const noexcept{
            return str.capacity() + sizeof(prefix);
        }

    private:

        // using composition instead of inheritance (from StrImp) to make sure
        // the keydomet is at the beginning of the object rather than at the end
        KeyDomet<Size_> prefix;
        StrImp_ str;

        static int diffAsOneOrMinusOne(const KeyDomet<Size_>& v1, const KeyDomet<Size_>& v2)
        {
            // return -1 if v1 < v2 and 1 otherwise
            // this is done without conditional branches and without risking wrap-around
            return ((int)!(v1 < v2) << 1) - 1;
        }

    };

    using KeyDometStr16 = KeyDometStr<std::string, KeyDometSize::SIZE_16BIT>;
    using KeyDometStr32 = KeyDometStr<std::string, KeyDometSize::SIZE_32BIT>;
    using KeyDometStr64 = KeyDometStr<std::string, KeyDometSize::SIZE_64BIT>;
    using KeyDometStr128 = KeyDometStr<std::string, KeyDometSize::SIZE_128BIT>;

    std::ostream& operator<<(std::ostream& os, const kdmt128_t& val);

    template<typename StrImp, KeyDometSize Size>
std::ostream& operator<<(std::ostream& os, const KeyDometStr<StrImp, Size>& hk){
    const StrImp& str = hk.getStr();
    os << str;
    return os;
}

    //
    // Hasher allowing the use of the keydomet as a (poorly distributed) hash value
    //
    struct KeyDometHasher
    {
        template<typename StrImp, KeyDometSize Size>
        size_t operator()(const KeyDometStr<StrImp, Size>& k) const
        {
            return k.getPrefix().val;
        }
    };

    //
    // A comparator allowing bucket lookups in a hash table
    //
    struct KeyDometComparator
    {
        template<typename StrImp, KeyDometSize Size>
        bool operator()(const KeyDometStr<StrImp, Size>& lhs, const KeyDometStr<StrImp, Size>& rhs) const
        {
            return lhs == rhs;
        }
    };

    namespace imp
    {
        // Searching associative containers (namely, sorted rather than hashed) could only be done using
        // the exact key type till C++14. From C++14, any type that can be compared with the key type can
        // be used. For instance, a std::string_view can be used in order to find a std::string key.
        // this is very useful when a key is given as a std::string but a KeyDomet should be constructed
        // in order to do the lookup: instead of creating a KeyDomet that will own a copy of the std::string
        // (with all the mess involved with the creation of the copy - allocation etc.), a KeyDometView is
        // used, merely referencing the original std::string.
        // To use the above feature, the comparator used by the container must be transparent, namely have
        // a is_transparent member. The code below detects the presence of that member, and the result is
        // used to determine whether a KeyDomet or KeyDometView should be constructed for lookups.
        template<typename...>
        using VoidT = void;

        struct NonTransparent {};

        auto getComparator(...) -> NonTransparent;

        template<template<class, class...> class Container, class KD, class... Args>
        auto getComparator(Container<KD, Args...>&) -> typename Container<KD, Args...>::key_compare;

        template<class Comparator, class = VoidT<>>
        struct HasTransparentFlagHelper : std::false_type {};

        template<class Comparator>
        struct HasTransparentFlagHelper<Comparator, VoidT<typename Comparator::is_transparent>> : std::true_type {};

        template<template<class, class...> class Container, class KD, class... Args>
        auto isTransparent(Container<KD, Args...>& c) ->
        std::enable_if_t<HasTransparentFlagHelper<decltype(getComparator(c))>::value, std::true_type>;

        auto isTransparent(...) -> std::false_type;

        template<template<class, class...> class Container, KeyDometSize Size, class... Args>
        auto getFindKeyType(Container<KeyDometStr<std::string, Size>, Args...>& s) ->
        // if container's comparator is transparent, use string_view-based key
        // else, use a full-blown string-based key
        std::conditional_t<
                decltype(isTransparent(s))::value,
                KeyDometStr<string_view, Size>,
                KeyDometStr<std::string, Size>
        >;
    }

    template<template<class, class...> class Container, class KD, class... Args>
    auto makeFindKey(Container<KD, Args...>& s, const std::string& key)
    {
        // associative containers (maps, sets) can use a transparent comparator. such a comperator can
        // compare the internal key type with other types (as long as there's an appropriate operator).
        // this allows such containers to hold strings but search using string_views, saving the allocation.
        // *** Note: to make a container associative, define it with the less<> comparator ***
        using KeyDometStrType = decltype(imp::getFindKeyType(s));
        return KeyDometStrType{key};
    }

    string getRandStr(size_t len);

    vector<string> getInput(size_t keysNum, size_t keyLen);

    vector<string> vectorCopy(vector<string> data);

    /////////////////////////////////////////////////////////BENCH 2////////////////////////////////////////////////////////////
    vector<string> getInputFromData(size_t keysNum, vector<string> data);
    vector<string> getInputFromParsedAndUnparsed(size_t keysNum, const string& path);

    template <class Container>
    Container parseCSV(const string& path, double portion){
        string line;
        ifstream infile(path.c_str());
        if(infile.bad()) {
            __throw_invalid_argument("can't open file - maybe path is wrong?");
        }
        Container c;
        while(!infile.eof()){
            getline(infile,line);
            c.push_back(line);
        }
        random_shuffle(c.begin(), c.end()); // TODO: Oren is this OK for the purpose of the benchmark?
        typename Container::const_iterator first = c.begin();
        typename Container::const_iterator last = c.begin() + (c.size()*portion);
        Container newC(first, last);
        random_shuffle(newC.begin(), newC.end());
        return newC;
    }
    /////////////////////////////////////////////////////////BENCH 2 END ////////////////////////////////////////////////////////////












template<template<class, typename...> class Container,
        typename StrT, KeyDometSize Size, typename... ContainerArgs>
void buildContainer(Container<KeyDometStr<StrT, Size>, ContainerArgs...>& container, const vector<string>& input)
{
    transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const string& str) {
        return KeyDometStr<StrT, Size>{str};
    });
}

template<template<class, typename...> class Container,
        typename... ContainerArgs>
void buildContainer(Container<string, ContainerArgs...>& container, const vector<string>& input)
{
    transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const string& str) {
        return str;
    });
}

// TODO: fix code duplication for lookup

template<template<class, class...> class Container, class StrType, class... Args>
bool lookup(Container<StrType, Args...>& s, const string& key)
{
    auto iter = s.find(key);
    return iter != s.end();
}

// specialized for keydomet-ed containers, which take a keydomet for lookups
template<template<class, class...> class Container, class... Args>
bool lookup(Container<KeyDometStr16, Args...>& s, const string& key)
{
    auto hkey = makeFindKey(s, key);
    auto iter = s.find(hkey);
    return iter != s.end();
}

template<template<class, class...> class Container, class... Args>
bool lookup(Container<KeyDometStr32, Args...>& s, const string& key)
{
    auto hkey = makeFindKey(s, key);
    auto iter = s.find(hkey);
    return iter != s.end();
}

template<template<class, class...> class Container, class... Args>
bool lookup(Container<KeyDometStr64, Args...>& s, const string& key)
{
    auto hkey = makeFindKey(s, key);
    auto iter = s.find(hkey);
    return iter != s.end();
}

template<template<class, class...> class Container, class... Args>
bool lookup(Container<KeyDometStr128, Args...>& s, const string& key)
{
    auto hkey = makeFindKey(s, key);
    auto iter = s.find(hkey);
    return iter != s.end();
}

template <class Container>
void stringCompare(Container& c, const vector<string>& lookups){
    Container* c2 = &c;  // just a workaround template arguments deduction fail.
    for (const string& s : lookups){
        lookup(*c2, s);
    }
}

template <class Container, class T>
void stringCompareAndInsert(Container& c, const vector<string>& lookups, const int readToWriteRatio, const size_t strLen){
    Container* c2 = &c;  // just a workaround template arguments deduction fail.
    int counter = 0; 
    for (const string& s : lookups){
        if(counter == readToWriteRatio){
            (*c2).insert(T(getRandStr(strLen)));
            counter = 0;
        }
        lookup(*c2, s);
        counter++;
    }
}

template <class C>
C buildContainerWrapper(const vector<string>& input){
    C c;
    buildContainer(c, input);
    return c;
}



/////////////////////////////////////////////////////// FUNCTIONS FOR WSTRING COMPARE /////////////////////////////////////////////////
vector<wstring> getInputFromParsedAndUnparsed_wstring(size_t keysNum, const string& path);

template<template<class, typename...> class Container,
            typename... Args>
    bool lookup(Container<wstring, Args...>& s, const wstring& key)
    {
        auto iter = s.find(key);
        return iter != s.end();
    }
 
template <class Container>
    void wstringCompare(Container& c, const vector<wstring>& lookups){
    Container* c2 = &c;  // just a workaround template arguments deduction fail.
    for (const wstring& s : lookups){
        lookup(*c2, s);
    }
} 

template <class Container>
    Container parseCSV_wstring(const string& path, double portion){
        string line; 
        ifstream infile(path.c_str());
        if(infile.fail()) {
            __throw_invalid_argument("can't open file - maybe path is wrong?");
        }
        Container c;
        while(infile.good()){
            getline(infile,line);
            wstring lineW (line.begin(), line.end());
            c.push_back(lineW);
        }
        random_shuffle(c.begin(), c.end()); // TODO: Oren is this OK for the purpose of the benchmark?
        typename Container::const_iterator first = c.begin();
        typename Container::const_iterator last = c.begin() + (c.size()*portion);
        Container newC(first, last);
        random_shuffle(newC.begin(), newC.end());
        return c;
    }


template<template<class, typename...> class Container,
            typename... ContainerArgs>
    void buildContainer(Container<wstring, ContainerArgs...>& container, const vector<wstring>& input)
    {
        transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const wstring& str) {
            return str;
        });
    }


template <class C>
    C buildContainerWrapper(const vector<wstring>& input){
        C c;
        buildContainer(c, input);
        return c;
    }

/////////////////////////////////////////////////////// FUNCTIONS FOR WSTRING COMPARE END/////////////////////////////////////////////////
 

/////////////////////////////////////////////////////// FUNCTIONS FOR MyString COMPARE /////////////////////////////////////////////////
vector<MyString> getInputFromParsedAndUnparsed_MyString(size_t keysNum, const string& path);

template<template<class, typename...> class Container,
            typename... Args>
    bool lookup(Container<MyString, Args...>& s, const MyString& key)
    {
        auto iter = s.find(key);
        return iter != s.end();
    }

template <class Container>
    void MyStringCompare(Container& c, const vector<MyString>& lookups){
        Container* c2 = &c;  // just a workaround template arguments deduction fail.
        for (const MyString& s : lookups){
            lookup(*c2, s);
        }
    } 
        
template <class Container>
    Container parseCSV_MyString(const string& path, double portion){
        string line; 
        ifstream infile(path.c_str());
        if(infile.fail()) {
            __throw_invalid_argument("can't open file - maybe path is wrong?");
        }
        Container c;
        while(!infile.eof()){
            getline(infile,line);
            MyString* lineS = new MyString(line);
            c.push_back(*lineS);
            }
        random_shuffle(c.begin(), c.end()); // TODO: Oren is this OK for the purpose of the benchmark?
        typename Container::const_iterator first = c.begin();
        typename Container::const_iterator last = c.begin() + (c.size()*portion);
        Container newC(first, last);
        random_shuffle(newC.begin(), newC.end());
        return c;
    }

template<template<class, typename...> class Container,
            typename... ContainerArgs>
    void buildContainer(Container<MyString, ContainerArgs...>& container, const vector<MyString>& input)
    {
        transform(input.begin(), input.end(), std::inserter(container, container.begin()), [](const MyString& str) {
            return str;
        });
    }

template <class C>
    C buildContainerWrapper(const vector<MyString>& input){
        C c;
        buildContainer(c, input);
        return c;
    }

/////////////////////////////////////////////////////// FUNCTIONS FOR MyString COMPARE END /////////////////////////////////////////////////


// template <typename C>
// size_t containerMemoryUsage(const C& c);

// template <>
// size_t containerMemoryUsage<typename C, int>(const C& c);

template <typename C>
string containerName();

template <>
string containerName<set<string>>();

template <>
string containerName<set<KeyDometStr16>>();

template <>
string containerName<set<KeyDometStr32>>();

template <>
string containerName<set<KeyDometStr64>>();

template <>
string containerName<set<KeyDometStr128>>();

} /* end namespace nemo */

#endif
