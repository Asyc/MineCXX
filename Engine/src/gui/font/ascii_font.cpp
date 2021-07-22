#include "engine/gui/font/ascii_font.hpp"

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

}   // namespace engine::font::font