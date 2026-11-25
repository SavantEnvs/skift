// Karm::Json::parse(Str) + unparse: the JSON reader opstart's boot config (Configs::fromJson) goes through.
#include <karm/macros>
import Karm.Core;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    Str s{reinterpret_cast<char const*>(data), len};
    auto v = Json::parse(s);
    if (not v)
        return 0;
    Io::StringWriter sw;
    Io::Emit e{sw};
    (void)Json::unparse(e, v.unwrap());
    return 0;
}
