// Karm::Font::Ttf::Parser::init(Bytes): TrueType/OpenType table directory, head/hhea/maxp,
// cmap, loca/glyf, hmtx and GPOS kerning, queried the way Ttf::Fontface does.
#include <karm/macros>
import Karm.Core;
import Karm.Gfx;
import Karm.Font.Ttf;

using namespace Karm;

extern "C" int LLVMFuzzerTestOneInput(u8 const* data, usize len) {
    // init() reads the 12-byte offset table through a panicking cursor without a length check,
    // so every input shorter than 12 bytes panics in the same place (a known upstream bug,
    // recorded in the PR). libFuzzer runs a 1-byte input before it starts mutating, so without
    // this check the target would abort before it fuzzes anything.
    if (len < 12)
        return -1;
    auto res = Font::Ttf::Parser::init(Bytes{data, len});
    if (not res)
        return 0;
    auto& p = res.unwrap();
    (void)p.metrics();
    (void)p.unitPerEm();
    Gfx::Glyph prev = p.glyph('A');
    for (Rune r : {(Rune)'A', (Rune)'V', (Rune)'a', (Rune)' ', (Rune)0x00e9, (Rune)0x4e2d, (Rune)0x1f600}) {
        auto g = p.glyph(r);
        (void)p.glyphMetrics(g);
        (void)p.glyphKern(prev, g);
        prev = g;
    }
    return 0;
}
