#ifdef _WIN32
#define PRAGMA(x) __pragma(x) //it seems like _Pragma isn't supported in MSVC
#else
#define PRAGMA(x) _Pragma(#x)
#endif

#ifdef _WIN32
#define CODELABEL(label) PRAGMA(region label)
#else
#define CODELABEL(label) PRAGMA(mark label)
#endif