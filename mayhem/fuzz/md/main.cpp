// Karm::Md::parse(Str) + renderHtmlFragment: Markdown parser and its HTML renderer.
#include <karm/macros>
import Karm.Core;
import Karm.Md;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    Str s{reinterpret_cast<char const*>(data), len};
    auto doc = Md::parse(s);
    Io::StringWriter sw;
    Io::Emit e{sw};
    Md::renderHtmlFragment(doc, e);
    return 0;
}
