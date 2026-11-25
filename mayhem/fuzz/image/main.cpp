// Karm::Image::load(Bytes): PNG/JPEG/BMP/GIF/QOI/TGA decoders (+ zlib inflate for PNG),
// the loader opstart uses for boot-config icons.
#include <karm/macros>
import Karm.Core;
import Karm.Image;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    auto res = Image::load(Bytes{data, len});
    if (not res)
        return 0;
    return 0;
}
