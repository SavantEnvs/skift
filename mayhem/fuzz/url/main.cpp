// Karm::Ref: URL / path / UUID parsers and the magic-byte sniffer.
#include <karm/macros>
import Karm.Core;
import Karm.Ref;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    Str s{reinterpret_cast<char const*>(data), len};
    auto url = Ref::Url::parse(s);
    (void)url.str();
    (void)url.join(Str{"../a/b?c#d"});
    (void)Ref::Path::parse(s);
    (void)Ref::Uuid::parse(s);
    (void)Ref::sniffBytes(Bytes{data, len});
    return 0;
}
