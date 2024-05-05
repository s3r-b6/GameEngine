#pragma once

#include "./game.h"
#include "tiles.h"
#include "types.h"
#include <vector>

_global u16 room1[] = {
    404, 405,     405,     0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       406,     404,     405,     405, 0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       406,     404,     0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       406, 404,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       406,     404,     0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   406,     404,     0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       406,     404, 0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       406,     404,     0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    406, 404,     0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       406,     404,     0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       406,     404,     0,       0,       0,       0,       0,   0,
    0,   MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   406,
    404, 0,       0,       0,       0,       0,       0,       0,       282,     283, 283,
    283, 283,     283,     283,     284,     0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       406,     404,     0,       0,   0,
    0,   0,       0,       0,       282,     283,     283,     283,     283,     283, 283,
    284, 0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       406,     404,     0,       0,       0,       0,       0,       0,   405,
    282, 283,     283,     283,     283,     283,     283,     284,     0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       406, 404,
    0,   0,       0,       0,       0,       0,       405,     283,     40,      283, 40,
    283, 283,     283,     MAX_U16, 0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       406,     404,     0,       0,       0,   0,
    0,   0,       0,       MAX_U16, 283,     283,     283,     283,     283,     283, 0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   406,     404,     0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       MAX_U16, MAX_U16, 0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       406,     404, 0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       406,     404,     0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    406, 404,     0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       0,       0,       0,   0,
    0,   0,       0,       0,       0,       0,       0,       406,     444,     445, 445,
    445, 445,     445,     445,     445,     445,     445,     445,     445,     445, 445,
    445, 445,     445,     445,     445,     445,     445,     445,     445,     445, 445,
    445, 445,     445,     445,     445,     445,     445,     445,     445,     445, 445,
    445, 445,     445,     446,     524,     524,     524,     524,     524,     524, 524,
    525, 526,     526,     526,     526,     526,     526,     526,     526,     526, 526,
    526, 526,     526,     526,     526,     526,     526,     526,     526,     526, 526,
    526, 526,     526,     526,     526,     526,     526,     526,     526,     526, 526,
    564, 564,     564,     564,     564,     564,     564,     565,     566,     566, 566,
    566, 566,     566,     566,     566,     566,     566,     566,     566,     566, 566,
    566, 566,     566,     566,     566,     566,     566,     566,     566,     526, 566,
    566, 566,     566,     566,     566,     566,     566};

_global u16 room1Collisions[] = {
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, 6,       7,       8,       9,       10,      MAX_U16,
    MAX_U16, 11,      12,      13,      9,       10,      MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, 46,      47,      48,      49,      50,      MAX_U16,
    MAX_U16, 51,      52,      53,      49,      50,      MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, 86,      87,      88,      89,      90,      MAX_U16,
    MAX_U16, 91,      92,      88,      89,      90,      MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, 126,     127,     128,     129,     130,     MAX_U16,
    MAX_U16, 126,     127,     128,     129,     130,     MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, 166,     167,     168,     169,     170,     MAX_U16,
    MAX_U16, 166,     167,     168,     169,     170,     MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 242,     243,
    243,     243,     243,     243,     243,     244,     MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 248,     249,     249,
    249,     249,     249,     249,     249,     249,     250,     MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 328,     329,     329,
    329,     329,     329,     329,     329,     329,     330,     MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 282,     MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 284,     MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 282,     MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 284,     MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 322,     323,
    323,     323,     323,     323,     323,     324,     MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
};

_global u16 room1Front[] = {
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, 288,     289,     289,
    289,     289,     289,     289,     289,     289,     290,     MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, 44,      45,      MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, 84,      85,      MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
    MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16, MAX_U16,
};

// This is bad, but I do not want to break hot-reload and rn I do not want to spend more time
// in a system that should be disposable (hot-reload is just for the development?).
TileID *loadRoom(BumpAllocator *bumpAlloc) {
    engine_log("%lu", bumpAlloc->used);
    TileID *room = (TileID *)bumpAlloc->alloc(sizeof(TileID) * TILES_CHUNK_x * TILES_CHUNK_y);
    for (size_t i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) { room[i] = room1[i]; }

    bumpAlloc->alloc(sizeof(TileID));

    engine_log("%lu", bumpAlloc->used);
    return room;
}

PosTile *loadFront(BumpAllocator *bumpAlloc, u16 *count) {
    PosTile *front = nullptr;
    engine_log("%lu", bumpAlloc->used);
    for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
        u16 id = room1Front[i];
        if (id == MAX_U16) { continue; }
        int x = i % TILES_CHUNK_x, y = i / TILES_CHUNK_x;

        // Allocate memory for each valid tile found
        PosTile *t = (PosTile *)(bumpAlloc->alloc(sizeof(PosTile)));
        t->posX = (u16)x, t->posY = (u16)y;
        t->id = id;

        // Link the new tile to the existing list
        if (front == nullptr) { front = t; }

        // Increment the count
        (*count)++;
    }
    bumpAlloc->alloc(sizeof(PosTile));

    engine_log("%lu", bumpAlloc->used);
    return front;
}

PosTile *loadCollisions(BumpAllocator *bumpAlloc, u16 *count) {
    PosTile *collisions = nullptr;
    engine_log("%lu", bumpAlloc->used);
    for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
        u16 id = room1Collisions[i];
        if (id == MAX_U16) { continue; }
        int x = i % TILES_CHUNK_x, y = i / TILES_CHUNK_x;

        // Allocate memory for each valid tile found
        PosTile *t = (PosTile *)(bumpAlloc->alloc(sizeof(PosTile)));
        t->posX = (u16)x, t->posY = (u16)y;
        t->id = id;

        // Link the new tile to the existing list
        if (collisions == nullptr) { collisions = t; }

        // Increment the count
        (*count)++;
    }
    bumpAlloc->alloc(sizeof(PosTile));

    engine_log("%lu", bumpAlloc->used);
    return collisions;
}

void placeRoom(Direction dirToParent, int x, int y);
void initRooms();
