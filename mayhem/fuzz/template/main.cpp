// Karm::Template::Document::parse(Str) + eval: the template language (if/for/print nodes, expressions).
#include <karm/macros>
import Karm.Core;
import Karm.Template;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    Str s{reinterpret_cast<char const*>(data), len};
    auto doc = Template::Document::parse(s);
    if (not doc)
        return 0;
    Serde::Object ctx{};
    (void)doc.unwrap().eval(ctx);
    return 0;
}
