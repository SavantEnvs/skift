// Karm::Vte::Terminal::write(Bytes): the VT/ANSI escape-sequence parser and the CSI handlers
// (cursor moves, erase, SGR attributes) applied to the terminal's line buffer.
// The Theme (two font faces) is loaded once and only read; a fresh Terminal (parser state,
// buffer, attributes) is built for every input, so each input's result depends only on it.
#include <karm/macros>
import Karm.Core;
import Karm.Vte;

using namespace Karm;

static Vte::Theme& theme() {
    static Vte::Theme t{};
    return t;
}

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    Vte::Terminal term{theme()};
    term.write(Bytes{data, len});
    return 0;
}
