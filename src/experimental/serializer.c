#define serialize_fn_cap 256
#define bucket_size 4

struct Serializer;
typedef void (*SerializeFn)(struct Serializer *, void * value);

typedef struct {
    char * ptr;
    long len;
} String;

typedef struct Serializer {
    char * buf;
    long len;
    long cap;
    SerializeFn fn[serialize_fn_cap];
    String types[serialize_fn_cap];
} Serializer;


