/* One sky palette per three hours. Adjacent palettes are blended per hour. */
static const uint16_t sky_at_hour[8][4] = {
    {HEX565(0x1b2044), HEX565(0x292f57), HEX565(0x40456d), HEX565(0x575783)},
    {HEX565(0x252344), HEX565(0x393557), HEX565(0x585479), HEX565(0x727397)},
    {HEX565(0xc28eaf), HEX565(0xe8a5a6), HEX565(0xf6c19e), HEX565(0xffd3a4)},
    {HEX565(0x79b7dc), HEX565(0x98cbe2), HEX565(0xb2daeb), HEX565(0xc5e6ec)},
    {HEX565(0x5da6dc), HEX565(0x7dbce9), HEX565(0xa0d3f2), HEX565(0xbbe4f3)},
    {HEX565(0x87b2d6), HEX565(0xa6c4df), HEX565(0xd4d3e4), HEX565(0xf3ded8)},
    {HEX565(0xefac99), HEX565(0xf3b49b), HEX565(0xf6c19e), HEX565(0xf9cda4)},
    {HEX565(0x3a325a), HEX565(0x4f4271), HEX565(0x705980), HEX565(0x906d90)}
};

/* Garden palette entries 8–23, from the distant hill to the flowers. */
static const uint16_t garden_at_season[4][16] = {
    {0xd492, 0xac12, 0x8470, 0x536c, 0x646e, 0x850f, 0x642e, 0x7ccc,
     0x954f, 0x642c, 0x5bab, 0x538a, 0xa5af, 0x536a, 0xfe93, 0xf531},
    {HEX565(0xa0b29a), HEX565(0x799d83), HEX565(0x698e72), HEX565(0x426559),
     HEX565(0x45775b), HEX565(0x81a86c), HEX565(0x548b63), HEX565(0x69994c),
     HEX565(0x8cb454), HEX565(0x648a42), HEX565(0x456c3c), HEX565(0x466b40),
     HEX565(0xa2bc59), HEX565(0x507644), HEX565(0xffe494), HEX565(0xf7b94b)},
    {HEX565(0xc59490), HEX565(0xa87f70), HEX565(0xa38264), HEX565(0x755447),
     HEX565(0xb66143), HEX565(0xffb46a), HEX565(0xd38345), HEX565(0xb39856),
     HEX565(0xd1ab61), HEX565(0x98754a), HEX565(0x7f5e44), HEX565(0x8f6542),
     HEX565(0xeabd65), HEX565(0x8e6945), HEX565(0xf7c76e), HEX565(0xd06a47)},
    {HEX565(0xafa7bf), HEX565(0x939eb8), HEX565(0xb9c9d4), HEX565(0x68798c),
     HEX565(0x96b4bf), HEX565(0xf4faf9), HEX565(0xaecbd1), HEX565(0xe5eff4),
     HEX565(0xf6fbff), HEX565(0xc5dce6), HEX565(0x9bb9cf), HEX565(0xacccd7),
     HEX565(0xebf7f6), HEX565(0xa7c6d8), HEX565(0xf8fbff), HEX565(0xd5e7ef)}
};

static const struct {
    uint16_t body, light;
    char rows[9][10];
} fruit_art[] = {
    {HEX565(0xe75157), HEX565(0xff9d89), {
        ".....gg..", "....gx...", "..xx.xxx.", ".xfhxffx.", "xfhhffffx",
        "xffffffff", "xfffffffx", ".xfffxxx.", "..xxx...."}},
    {HEX565(0xffd364), HEX565(0xffeda0), {
        ".......gx", "......xx.", ".....xfx.", "....xfhx.", "....xfhx.",
        "x..xfhhx.", "xxxhhfx..", ".xffxx...", "..xx....."}},
    {HEX565(0xf79434), HEX565(0xffca79), {
        "....gg...", "...xgxx..", "..xfhffx.", ".xfhhfffx", ".xffffffx",
        ".xfhffffx", ".xffffffx", "..xffffff", "...xxxxx."}},
    {HEX565(0xa76bd4), HEX565(0xd7a5f0), {
        "...xgg...", "..xxxg...", ".xhxfx...", "..xxxfxx.", "..xfxxfx.",
        "..xxxxxx.", "...xhfx..", "...xfx...", "....x...."}},
    {HEX565(0xf45d71), HEX565(0xffbe8d), {
        "..g.g.g..", ".xgggggx.", "xffgfgffx", "xfhfffhfx", "xffhffffx",
        ".xffffhx.", "..xfhfx..", "...xfx...", "....x...."}},
    {HEX565(0xabc654), HEX565(0xe4e982), {
        ".....gx..", "....gx...", "...xxx...", "...xhfx..", "..xfhfx..",
        ".xfhfffx.", "xfffffffx", ".xfffxxx.", "..xxx...."}},
    {HEX565(0xd64057), HEX565(0xff8b87), {
        "....ggg..", "...g.g...", "..g..g...", "..g..gg..", ".xx..xgx.",
        "xhfx.xhfx", "xffx.xffx", ".xx...xx.", "........."}},
    {HEX565(0xfb6c77), HEX565(0xffac97), {
        ".........", ".........", "xxx.xxxxx", "xfhffhffx", "gffxfhfxg",
        ".gfffffg.", ".xgfffgx.", "..xgggx..", "...xxx..."}}
};

static const uint16_t digits[] = {
    0x7b6f, 0x2c97, 0x73e7, 0x73cf, 0x5bc9, 0x79cf, 0x79ef, 0x7249, 0x7bef, 0x7bcf
};

static const char season_icons[4][7][8] = {
    {".xx.xx.", "xx.x.xx", "xx.x.xx", ".xxxxx.", "...x...", "...x...", "..xxx.."},
    {"x..x..x", "...x...", "..xxx..", "xxxxxxx", "..xxx..", "...x...", "x..x..x"},
    {"....xxx", "...xxxx", "..xxxxx", ".xxxxx.", ".xxxx..", ".xx....", "x......"},
    {"x..x..x", ".x.x.x.", "..xxx..", "xxxxxxx", "..xxx..", ".x.x.x.", "x..x..x"}
};
