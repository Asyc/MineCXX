#include "engine/gui/font/ascii_font.hpp"

#include <unordered_map>

#include "engine/log.hpp"

namespace engine::gui::font {

std::array<CharNode, 256> ASCII_CHAR_SIZES = {
    CharNode{192, 0, 0, 6},
    CharNode{193, 1, 0, 6},
    CharNode{194, 2, 0, 6},
    CharNode{200, 3, 0, 6},
    CharNode{202, 4, 0, 6},
    CharNode{203, 5, 0, 6},
    CharNode{205, 6, 0, 4},
    CharNode{211, 7, 0, 6},
    CharNode{212, 8, 0, 6},
    CharNode{213, 9, 0, 6},
    CharNode{218, 10, 0, 6},
    CharNode{223, 11, 0, 6},
    CharNode{227, 12, 0, 6},
    CharNode{245, 13, 0, 6},
    CharNode{287, 14, 0, 6},
    CharNode{304, 15, 0, 4},
    CharNode{305, 0, 1, 4},
    CharNode{338, 1, 1, 6},
    CharNode{339, 2, 1, 7},
    CharNode{350, 3, 1, 6},
    CharNode{351, 4, 1, 6},
    CharNode{372, 5, 1, 6},
    CharNode{373, 6, 1, 6},
    CharNode{382, 7, 1, 6},
    CharNode{519, 8, 1, 6},
    CharNode{0, 9, 1, 0},
    CharNode{0, 10, 1, 0},
    CharNode{0, 11, 1, 0},
    CharNode{0, 12, 1, 0},
    CharNode{0, 13, 1, 0},
    CharNode{0, 14, 1, 0},
    CharNode{0, 15, 1, 0},
    CharNode{32, 0, 2, 4},
    CharNode{33, 1, 2, 2},
    CharNode{34, 2, 2, 5},
    CharNode{35, 3, 2, 6},
    CharNode{36, 4, 2, 6},
    CharNode{37, 5, 2, 6},
    CharNode{38, 6, 2, 6},
    CharNode{39, 7, 2, 3},
    CharNode{40, 8, 2, 5},
    CharNode{41, 9, 2, 5},
    CharNode{42, 10, 2, 5},
    CharNode{43, 11, 2, 6},
    CharNode{44, 12, 2, 2},
    CharNode{45, 13, 2, 6},
    CharNode{46, 14, 2, 2},
    CharNode{47, 15, 2, 6},
    CharNode{48, 0, 3, 6},
    CharNode{49, 1, 3, 6},
    CharNode{50, 2, 3, 6},
    CharNode{51, 3, 3, 6},
    CharNode{52, 4, 3, 6},
    CharNode{53, 5, 3, 6},
    CharNode{54, 6, 3, 6},
    CharNode{55, 7, 3, 6},
    CharNode{56, 8, 3, 6},
    CharNode{57, 9, 3, 6},
    CharNode{58, 10, 3, 2},
    CharNode{59, 11, 3, 2},
    CharNode{60, 12, 3, 5},
    CharNode{61, 13, 3, 6},
    CharNode{62, 14, 3, 5},
    CharNode{63, 15, 3, 6},
    CharNode{64, 0, 4, 7},
    CharNode{65, 1, 4, 6},
    CharNode{66, 2, 4, 6},
    CharNode{67, 3, 4, 6},
    CharNode{68, 4, 4, 6},
    CharNode{69, 5, 4, 6},
    CharNode{70, 6, 4, 6},
    CharNode{71, 7, 4, 6},
    CharNode{72, 8, 4, 6},
    CharNode{73, 9, 4, 4},
    CharNode{74, 10, 4, 6},
    CharNode{75, 11, 4, 6},
    CharNode{76, 12, 4, 6},
    CharNode{77, 13, 4, 6},
    CharNode{78, 14, 4, 6},
    CharNode{79, 15, 4, 6},
    CharNode{80, 0, 5, 6},
    CharNode{81, 1, 5, 6},
    CharNode{82, 2, 5, 6},
    CharNode{83, 3, 5, 6},
    CharNode{84, 4, 5, 6},
    CharNode{85, 5, 5, 6},
    CharNode{86, 6, 5, 6},
    CharNode{87, 7, 5, 6},
    CharNode{88, 8, 5, 6},
    CharNode{89, 9, 5, 6},
    CharNode{90, 10, 5, 6},
    CharNode{91, 11, 5, 4},
    CharNode{92, 12, 5, 6},
    CharNode{93, 13, 5, 4},
    CharNode{94, 14, 5, 6},
    CharNode{95, 15, 5, 6},
    CharNode{96, 0, 6, 3},
    CharNode{97, 1, 6, 6},
    CharNode{98, 2, 6, 6},
    CharNode{99, 3, 6, 6},
    CharNode{100, 4, 6, 6},
    CharNode{101, 5, 6, 6},
    CharNode{102, 6, 6, 5},
    CharNode{103, 7, 6, 6},
    CharNode{104, 8, 6, 6},
    CharNode{105, 9, 6, 2},
    CharNode{106, 10, 6, 6},
    CharNode{107, 11, 6, 5},
    CharNode{108, 12, 6, 3},
    CharNode{109, 13, 6, 6},
    CharNode{110, 14, 6, 6},
    CharNode{111, 15, 6, 6},
    CharNode{112, 0, 7, 6},
    CharNode{113, 1, 7, 6},
    CharNode{114, 2, 7, 6},
    CharNode{115, 3, 7, 6},
    CharNode{116, 4, 7, 4},
    CharNode{117, 5, 7, 6},
    CharNode{118, 6, 7, 6},
    CharNode{119, 7, 7, 6},
    CharNode{120, 8, 7, 6},
    CharNode{121, 9, 7, 6},
    CharNode{122, 10, 7, 6},
    CharNode{123, 11, 7, 5},
    CharNode{124, 12, 7, 2},
    CharNode{125, 13, 7, 5},
    CharNode{126, 14, 7, 7},
    CharNode{0, 15, 7, 0},
    CharNode{199, 0, 8, 6},
    CharNode{252, 1, 8, 6},
    CharNode{233, 2, 8, 6},
    CharNode{226, 3, 8, 6},
    CharNode{228, 4, 8, 6},
    CharNode{224, 5, 8, 6},
    CharNode{229, 6, 8, 6},
    CharNode{231, 7, 8, 6},
    CharNode{234, 8, 8, 6},
    CharNode{235, 9, 8, 6},
    CharNode{232, 10, 8, 6},
    CharNode{239, 11, 8, 4},
    CharNode{238, 12, 8, 6},
    CharNode{236, 13, 8, 3},
    CharNode{196, 14, 8, 6},
    CharNode{197, 15, 8, 6},
    CharNode{201, 0, 9, 6},
    CharNode{230, 1, 9, 6},
    CharNode{198, 2, 9, 6},
    CharNode{244, 3, 9, 6},
    CharNode{246, 4, 9, 6},
    CharNode{242, 5, 9, 6},
    CharNode{251, 6, 9, 6},
    CharNode{249, 7, 9, 6},
    CharNode{255, 8, 9, 6},
    CharNode{214, 9, 9, 6},
    CharNode{220, 10, 9, 6},
    CharNode{248, 11, 9, 6},
    CharNode{163, 12, 9, 6},
    CharNode{216, 13, 9, 6},
    CharNode{215, 14, 9, 4},
    CharNode{402, 15, 9, 6},
    CharNode{225, 0, 10, 6},
    CharNode{237, 1, 10, 3},
    CharNode{243, 2, 10, 6},
    CharNode{250, 3, 10, 6},
    CharNode{241, 4, 10, 6},
    CharNode{209, 5, 10, 6},
    CharNode{170, 6, 10, 6},
    CharNode{186, 7, 10, 6},
    CharNode{191, 8, 10, 6},
    CharNode{174, 9, 10, 7},
    CharNode{172, 10, 10, 6},
    CharNode{189, 11, 10, 6},
    CharNode{188, 12, 10, 6},
    CharNode{161, 13, 10, 2},
    CharNode{171, 14, 10, 6},
    CharNode{187, 15, 10, 6},
    CharNode{9617, 0, 11, 8},
    CharNode{9618, 1, 11, 9},
    CharNode{9619, 2, 11, 9},
    CharNode{9474, 3, 11, 6},
    CharNode{9508, 4, 11, 6},
    CharNode{9569, 5, 11, 6},
    CharNode{9570, 6, 11, 8},
    CharNode{9558, 7, 11, 8},
    CharNode{9557, 8, 11, 6},
    CharNode{9571, 9, 11, 8},
    CharNode{9553, 10, 11, 8},
    CharNode{9559, 11, 11, 8},
    CharNode{9565, 12, 11, 8},
    CharNode{9564, 13, 11, 8},
    CharNode{9563, 14, 11, 6},
    CharNode{9488, 15, 11, 6},
    CharNode{9492, 0, 12, 9},
    CharNode{9524, 1, 12, 9},
    CharNode{9516, 2, 12, 9},
    CharNode{9500, 3, 12, 9},
    CharNode{9472, 4, 12, 9},
    CharNode{9532, 5, 12, 9},
    CharNode{9566, 6, 12, 9},
    CharNode{9567, 7, 12, 9},
    CharNode{9562, 8, 12, 9},
    CharNode{9556, 9, 12, 9},
    CharNode{9577, 10, 12, 9},
    CharNode{9574, 11, 12, 9},
    CharNode{9568, 12, 12, 9},
    CharNode{9552, 13, 12, 9},
    CharNode{9580, 14, 12, 9},
    CharNode{9575, 15, 12, 9},
    CharNode{9576, 0, 13, 9},
    CharNode{9572, 1, 13, 9},
    CharNode{9573, 2, 13, 9},
    CharNode{9561, 3, 13, 9},
    CharNode{9560, 4, 13, 9},
    CharNode{9554, 5, 13, 9},
    CharNode{9555, 6, 13, 9},
    CharNode{9579, 7, 13, 9},
    CharNode{9578, 8, 13, 9},
    CharNode{9496, 9, 13, 6},
    CharNode{9484, 10, 13, 9},
    CharNode{9608, 11, 13, 9},
    CharNode{9604, 12, 13, 9},
    CharNode{9612, 13, 13, 5},
    CharNode{9616, 14, 13, 9},
    CharNode{9600, 15, 13, 9},
    CharNode{945, 0, 14, 8},
    CharNode{946, 1, 14, 7},
    CharNode{915, 2, 14, 7},
    CharNode{960, 3, 14, 8},
    CharNode{931, 4, 14, 7},
    CharNode{963, 5, 14, 8},
    CharNode{956, 6, 14, 8},
    CharNode{964, 7, 14, 8},
    CharNode{934, 8, 14, 7},
    CharNode{920, 9, 14, 8},
    CharNode{937, 10, 14, 8},
    CharNode{948, 11, 14, 7},
    CharNode{8734, 12, 14, 9},
    CharNode{8709, 13, 14, 9},
    CharNode{8712, 14, 14, 6},
    CharNode{8745, 15, 14, 7},
    CharNode{8801, 0, 15, 7},
    CharNode{177, 1, 15, 7},
    CharNode{8805, 2, 15, 7},
    CharNode{8804, 3, 15, 7},
    CharNode{8992, 4, 15, 9},
    CharNode{8993, 5, 15, 6},
    CharNode{247, 6, 15, 7},
    CharNode{8776, 7, 15, 8},
    CharNode{176, 8, 15, 7},
    CharNode{8729, 9, 15, 6},
    CharNode{183, 10, 15, 6},
    CharNode{8730, 11, 15, 9},
    CharNode{8319, 12, 15, 7},
    CharNode{178, 13, 15, 6},
    CharNode{9632, 14, 15, 7},
    CharNode{0, 15, 15, 0}
};

std::unique_ptr<std::unordered_map<char16_t, uint8_t>> ASCII_INDEXES;

void initAsciiIndexes() {
    ASCII_INDEXES = std::make_unique<std::unordered_map<char16_t, uint8_t>>();

    uint8_t index = 0;
    for (const auto& node : ASCII_CHAR_SIZES) {
        ASCII_INDEXES->emplace(static_cast<char16_t>(node.value), index++);
    }
}

uint8_t map(int8_t value) {
    switch (value) {
        case 192:return 0;
        case 193:return 1;
        case 194:return 2;
        case 200:return 3;
        case 202:return 4;
        case 203:return 5;
        case 205:return 6;
        case 211:return 7;
        case 212:return 8;
        case 213:return 9;
        case 218:return 10;
        case 223:return 11;
        case 227:return 12;
        case 245:return 13;
        case 287:return 14;
        case 304:return 15;
        case 305:return 16;
        case 338:return 17;
        case 339:return 18;
        case 350:return 19;
        case 351:return 20;
        case 372:return 21;
        case 373:return 22;
        case 382:return 23;
        case 519:return 24;
        case 32:return 32;
        case 33:return 33;
        case 34:return 34;
        case 35:return 35;
        case 36:return 36;
        case 37:return 37;
        case 38:return 38;
        case 39:return 39;
        case 40:return 40;
        case 41:return 41;
        case 42:return 42;
        case 43:return 43;
        case 44:return 44;
        case 45:return 45;
        case 46:return 46;
        case 47:return 47;
        case 48:return 48;
        case 49:return 49;
        case 50:return 50;
        case 51:return 51;
        case 52:return 52;
        case 53:return 53;
        case 54:return 54;
        case 55:return 55;
        case 56:return 56;
        case 57:return 57;
        case 58:return 58;
        case 59:return 59;
        case 60:return 60;
        case 61:return 61;
        case 62:return 62;
        case 63:return 63;
        case 64:return 64;
        case 65:return 65;
        case 66:return 66;
        case 67:return 67;
        case 68:return 68;
        case 69:return 69;
        case 70:return 70;
        case 71:return 71;
        case 72:return 72;
        case 73:return 73;
        case 74:return 74;
        case 75:return 75;
        case 76:return 76;
        case 77:return 77;
        case 78:return 78;
        case 79:return 79;
        case 80:return 80;
        case 81:return 81;
        case 82:return 82;
        case 83:return 83;
        case 84:return 84;
        case 85:return 85;
        case 86:return 86;
        case 87:return 87;
        case 88:return 88;
        case 89:return 89;
        case 90:return 90;
        case 91:return 91;
        case 92:return 92;
        case 93:return 93;
        case 94:return 94;
        case 95:return 95;
        case 96:return 96;
        case 97:return 97;
        case 98:return 98;
        case 99:return 99;
        case 100:return 100;
        case 101:return 101;
        case 102:return 102;
        case 103:return 103;
        case 104:return 104;
        case 105:return 105;
        case 106:return 106;
        case 107:return 107;
        case 108:return 108;
        case 109:return 109;
        case 110:return 110;
        case 111:return 111;
        case 112:return 112;
        case 113:return 113;
        case 114:return 114;
        case 115:return 115;
        case 116:return 116;
        case 117:return 117;
        case 118:return 118;
        case 119:return 119;
        case 120:return 120;
        case 121:return 121;
        case 122:return 122;
        case 123:return 123;
        case 124:return 124;
        case 125:return 125;
        case 126:return 126;
        case 199:return 128;
        case 252:return 129;
        case 233:return 130;
        case 226:return 131;
        case 228:return 132;
        case 224:return 133;
        case 229:return 134;
        case 231:return 135;
        case 234:return 136;
        case 235:return 137;
        case 232:return 138;
        case 239:return 139;
        case 238:return 140;
        case 236:return 141;
        case 196:return 142;
        case 197:return 143;
        case 201:return 144;
        case 230:return 145;
        case 198:return 146;
        case 244:return 147;
        case 246:return 148;
        case 242:return 149;
        case 251:return 150;
        case 249:return 151;
        case 255:return 152;
        case 214:return 153;
        case 220:return 154;
        case 248:return 155;
        case 163:return 156;
        case 216:return 157;
        case 215:return 158;
        case 402:return 159;
        case 225:return 160;
        case 237:return 161;
        case 243:return 162;
        case 250:return 163;
        case 241:return 164;
        case 209:return 165;
        case 170:return 166;
        case 186:return 167;
        case 191:return 168;
        case 174:return 169;
        case 172:return 170;
        case 189:return 171;
        case 188:return 172;
        case 161:return 173;
        case 171:return 174;
        case 187:return 175;
        case 9617:return 176;
        case 9618:return 177;
        case 9619:return 178;
        case 9474:return 179;
        case 9508:return 180;
        case 9569:return 181;
        case 9570:return 182;
        case 9558:return 183;
        case 9557:return 184;
        case 9571:return 185;
        case 9553:return 186;
        case 9559:return 187;
        case 9565:return 188;
        case 9564:return 189;
        case 9563:return 190;
        case 9488:return 191;
        case 9492:return 192;
        case 9524:return 193;
        case 9516:return 194;
        case 9500:return 195;
        case 9472:return 196;
        case 9532:return 197;
        case 9566:return 198;
        case 9567:return 199;
        case 9562:return 200;
        case 9556:return 201;
        case 9577:return 202;
        case 9574:return 203;
        case 9568:return 204;
        case 9552:return 205;
        case 9580:return 206;
        case 9575:return 207;
        case 9576:return 208;
        case 9572:return 209;
        case 9573:return 210;
        case 9561:return 211;
        case 9560:return 212;
        case 9554:return 213;
        case 9555:return 214;
        case 9579:return 215;
        case 9578:return 216;
        case 9496:return 217;
        case 9484:return 218;
        case 9608:return 219;
        case 9604:return 220;
        case 9612:return 221;
        case 9616:return 222;
        case 9600:return 223;
        case 945:return 224;
        case 946:return 225;
        case 915:return 226;
        case 960:return 227;
        case 931:return 228;
        case 963:return 229;
        case 956:return 230;
        case 964:return 231;
        case 934:return 232;
        case 920:return 233;
        case 937:return 234;
        case 948:return 235;
        case 8734:return 236;
        case 8709:return 237;
        case 8712:return 238;
        case 8745:return 239;
        case 8801:return 240;
        case 177:return 241;
        case 8805:return 242;
        case 8804:return 243;
        case 8992:return 244;
        case 8993:return 245;
        case 247:return 246;
        case 8776:return 247;
        case 176:return 248;
        case 8729:return 249;
        case 183:return 250;
        case 8730:return 251;
        case 8319:return 252;
        case 178:return 253;
        case 9632:return 254;
        default:
            MCE_LOG_ERROR("Reached End of Font Map(I) function"); // soft error
            return value;
    }
}

}   // namespace engine::font::font