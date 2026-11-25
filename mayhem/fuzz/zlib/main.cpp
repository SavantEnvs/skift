// Karm::Archive::zlibDecompress(Bytes): zlib framing, DEFLATE (stored/fixed/dynamic Huffman), adler32.
#include <karm/macros>
import Karm.Core;
import Karm.Archive;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    (void)Archive::zlibDecompress(Bytes{data, len});
    return 0;
}
