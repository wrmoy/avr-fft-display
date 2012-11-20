/*
 * goerztel.h
 * Goertzel Algorithm header file
 * targets 156, 250, 500, 1000, 2000, 4000, 8000, 16000 Hz freq
 *
 * by William Moy
 */

#ifndef GOERTZEL_H
#define GOERTZEL_H

#include "types.h"
#include <avr/pgmspace.h>

#define N_SAMPLES 400

static DWORD samplesProcessed = 0;
static BYTE goertzelReady = 0;

static int16_t q_0[8] = {0};
static int16_t q_1[8] = {0};
static int16_t q_2[8] = {0};

const static BYTE coeff_mult[5][256] PROGMEM = \
{ {0 ,  0 ,  1 ,  2 ,  2 ,  3 ,  4 ,  4 ,  5 ,  6 ,  7 ,  7 ,  8 ,  9 ,  9 ,  10 ,  11 ,  12 ,  12 ,  13 ,  14 ,  14 ,  15 ,  16 ,  16 ,  17 ,  18 ,  19 ,  19 ,  20 ,  21 ,  21 ,  22 ,  23 ,  24 ,  24 ,  25 ,  26 ,  26 ,  27 ,  28 ,  28 ,  29 ,  30 ,  31 ,  31 ,  32 ,  33 ,  33 ,  34 ,  35 ,  36 ,  36 ,  37 ,  38 ,  38 ,  39 ,  40 ,  41 ,  41 ,  42 ,  43 ,  43 ,  44 ,  45 ,  45 ,  46 ,  47 ,  48 ,  48 ,  49 ,  50 ,  50 ,  51 ,  52 ,  53 ,  53 ,  54 ,  55 ,  55 ,  56 ,  57 ,  57 ,  58 ,  59 ,  60 ,  60 ,  61 ,  62 ,  62 ,  63 ,  64 ,  65 ,  65 ,  66 ,  67 ,  67 ,  68 ,  69 ,  70 ,  70 ,  71 ,  72 ,  72 ,  73 ,  74 ,  74 ,  75 ,  76 ,  77 ,  77 ,  78 ,  79 ,  79 ,  80 ,  81 ,  82 ,  82 ,  83 ,  84 ,  84 ,  85 ,  86 ,  86 ,  87 ,  88 ,  89 ,  89 ,  90 ,  91 ,  91 ,  92 ,  93 ,  94 ,  94 ,  95 ,  96 ,  96 ,  97 ,  98 ,  98 ,  99 ,  100 ,  101 ,  101 ,  102 ,  103 ,  103 ,  104 ,  105 ,  106 ,  106 ,  107 ,  108 ,  108 ,  109 ,  110 ,  111 ,  111 ,  112 ,  113 ,  113 ,  114 ,  115 ,  115 ,  116 ,  117 ,  118 ,  118 ,  119 ,  120 ,  120 ,  121 ,  122 ,  123 ,  123 ,  124 ,  125 ,  125 ,  126 ,  127 ,  127 ,  128 ,  129 ,  130 ,  130 ,  131 ,  132 ,  132 ,  133 ,  134 ,  135 ,  135 ,  136 ,  137 ,  137 ,  138 ,  139 ,  140 ,  140 ,  141 ,  142 ,  142 ,  143 ,  144 ,  144 ,  145 ,  146 ,  147 ,  147 ,  148 ,  149 ,  149 ,  150 ,  151 ,  152 ,  152 ,  153 ,  154 ,  154 ,  155 ,  156 ,  156 ,  157 ,  158 ,  159 ,  159 ,  160 ,  161 ,  161 ,  162 ,  163 ,  164 ,  164 ,  165 ,  166 ,  166 ,  167 ,  168 ,  168 ,  169 ,  170 ,  171 ,  171 ,  172 ,  173 ,  173 ,  174 ,  175 ,  176 ,  176 ,  177 ,  178 ,  178 ,  179}, 
  {0 ,  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 ,  10 ,  11 ,  12 ,  12 ,  13 ,  14 ,  15 ,  16 ,  17 ,  18 ,  19 ,  20 ,  21 ,  22 ,  23 ,  24 ,  24 ,  25 ,  26 ,  27 ,  28 ,  29 ,  30 ,  31 ,  32 ,  33 ,  34 ,  35 ,  36 ,  36 ,  37 ,  38 ,  39 ,  40 ,  41 ,  42 ,  43 ,  44 ,  45 ,  46 ,  47 ,  48 ,  48 ,  49 ,  50 ,  51 ,  52 ,  53 ,  54 ,  55 ,  56 ,  57 ,  58 ,  59 ,  60 ,  60 ,  61 ,  62 ,  63 ,  64 ,  65 ,  66 ,  67 ,  68 ,  69 ,  70 ,  71 ,  72 ,  72 ,  73 ,  74 ,  75 ,  76 ,  77 ,  78 ,  79 ,  80 ,  81 ,  82 ,  83 ,  84 ,  84 ,  85 ,  86 ,  87 ,  88 ,  89 ,  90 ,  91 ,  92 ,  93 ,  94 ,  95 ,  96 ,  97 ,  97 ,  98 ,  99 ,  100 ,  101 ,  102 ,  103 ,  104 ,  105 ,  106 ,  107 ,  108 ,  109 ,  109 ,  110 ,  111 ,  112 ,  113 ,  114 ,  115 ,  116 ,  117 ,  118 ,  119 ,  120 ,  121 ,  121 ,  122 ,  123 ,  124 ,  125 ,  126 ,  127 ,  128 ,  129 ,  130 ,  131 ,  132 ,  133 ,  133 ,  134 ,  135 ,  136 ,  137 ,  138 ,  139 ,  140 ,  141 ,  142 ,  143 ,  144 ,  145 ,  145 ,  146 ,  147 ,  148 ,  149 ,  150 ,  151 ,  152 ,  153 ,  154 ,  155 ,  156 ,  157 ,  157 ,  158 ,  159 ,  160 ,  161 ,  162 ,  163 ,  164 ,  165 ,  166 ,  167 ,  168 ,  169 ,  169 ,  170 ,  171 ,  172 ,  173 ,  174 ,  175 ,  176 ,  177 ,  178 ,  179 ,  180 ,  181 ,  182 ,  182 ,  183 ,  184 ,  185 ,  186 ,  187 ,  188 ,  189 ,  190 ,  191 ,  192 ,  193 ,  194 ,  194 ,  195 ,  196 ,  197 ,  198 ,  199 ,  200 ,  201 ,  202 ,  203 ,  204 ,  205 ,  206 ,  206 ,  207 ,  208 ,  209 ,  210 ,  211 ,  212 ,  213 ,  214 ,  215 ,  216 ,  217 ,  218 ,  218 ,  219 ,  220 ,  221 ,  222 ,  223 ,  224 ,  225 ,  226 ,  227 ,  228 ,  229 ,  230 ,  230 ,  231 ,  232 ,  233 ,  234}, 
  {0 ,  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 ,  10 ,  11 ,  12 ,  13 ,  14 ,  15 ,  16 ,  17 ,  18 ,  19 ,  20 ,  21 ,  22 ,  23 ,  24 ,  25 ,  26 ,  27 ,  28 ,  29 ,  30 ,  31 ,  32 ,  33 ,  34 ,  35 ,  36 ,  37 ,  38 ,  39 ,  40 ,  41 ,  42 ,  43 ,  44 ,  45 ,  46 ,  47 ,  48 ,  49 ,  50 ,  51 ,  51 ,  52 ,  53 ,  54 ,  55 ,  56 ,  57 ,  58 ,  59 ,  60 ,  61 ,  62 ,  63 ,  64 ,  65 ,  66 ,  67 ,  68 ,  69 ,  70 ,  71 ,  72 ,  73 ,  74 ,  75 ,  76 ,  77 ,  78 ,  79 ,  80 ,  81 ,  82 ,  83 ,  84 ,  85 ,  86 ,  87 ,  88 ,  89 ,  90 ,  91 ,  92 ,  93 ,  94 ,  95 ,  96 ,  97 ,  98 ,  99 ,  100 ,  101 ,  102 ,  102 ,  103 ,  104 ,  105 ,  106 ,  107 ,  108 ,  109 ,  110 ,  111 ,  112 ,  113 ,  114 ,  115 ,  116 ,  117 ,  118 ,  119 ,  120 ,  121 ,  122 ,  123 ,  124 ,  125 ,  126 ,  127 ,  128 ,  129 ,  130 ,  131 ,  132 ,  133 ,  134 ,  135 ,  136 ,  137 ,  138 ,  139 ,  140 ,  141 ,  142 ,  143 ,  144 ,  145 ,  146 ,  147 ,  148 ,  149 ,  150 ,  151 ,  152 ,  153 ,  153 ,  154 ,  155 ,  156 ,  157 ,  158 ,  159 ,  160 ,  161 ,  162 ,  163 ,  164 ,  165 ,  166 ,  167 ,  168 ,  169 ,  170 ,  171 ,  172 ,  173 ,  174 ,  175 ,  176 ,  177 ,  178 ,  179 ,  180 ,  181 ,  182 ,  183 ,  184 ,  185 ,  186 ,  187 ,  188 ,  189 ,  190 ,  191 ,  192 ,  193 ,  194 ,  195 ,  196 ,  197 ,  198 ,  199 ,  200 ,  201 ,  202 ,  203 ,  204 ,  204 ,  205 ,  206 ,  207 ,  208 ,  209 ,  210 ,  211 ,  212 ,  213 ,  214 ,  215 ,  216 ,  217 ,  218 ,  219 ,  220 ,  221 ,  222 ,  223 ,  224 ,  225 ,  226 ,  227 ,  228 ,  229 ,  230 ,  231 ,  232 ,  233 ,  234 ,  235 ,  236 ,  237 ,  238 ,  239 ,  240 ,  241 ,  242 ,  243 ,  244 ,  245 ,  246 ,  247 ,  248 ,  249}, 
  {0 ,  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 ,  10 ,  11 ,  12 ,  13 ,  14 ,  15 ,  16 ,  17 ,  18 ,  19 ,  20 ,  21 ,  22 ,  23 ,  24 ,  25 ,  26 ,  27 ,  28 ,  29 ,  30 ,  31 ,  32 ,  33 ,  34 ,  35 ,  36 ,  37 ,  38 ,  39 ,  40 ,  41 ,  42 ,  43 ,  44 ,  45 ,  46 ,  47 ,  48 ,  49 ,  50 ,  51 ,  52 ,  53 ,  54 ,  55 ,  56 ,  57 ,  58 ,  59 ,  60 ,  61 ,  62 ,  63 ,  64 ,  65 ,  66 ,  67 ,  68 ,  69 ,  70 ,  71 ,  72 ,  73 ,  74 ,  75 ,  76 ,  77 ,  78 ,  79 ,  80 ,  81 ,  82 ,  83 ,  84 ,  85 ,  86 ,  87 ,  88 ,  89 ,  90 ,  91 ,  92 ,  93 ,  94 ,  95 ,  96 ,  97 ,  98 ,  99 ,  100 ,  101 ,  102 ,  103 ,  104 ,  105 ,  106 ,  107 ,  108 ,  109 ,  110 ,  111 ,  112 ,  113 ,  114 ,  115 ,  116 ,  117 ,  118 ,  119 ,  120 ,  121 ,  122 ,  123 ,  124 ,  125 ,  126 ,  127 ,  128 ,  129 ,  130 ,  131 ,  132 ,  133 ,  134 ,  135 ,  136 ,  137 ,  138 ,  139 ,  140 ,  141 ,  142 ,  143 ,  144 ,  145 ,  146 ,  147 ,  148 ,  149 ,  150 ,  151 ,  152 ,  153 ,  154 ,  155 ,  156 ,  157 ,  158 ,  159 ,  160 ,  161 ,  162 ,  163 ,  164 ,  165 ,  166 ,  167 ,  168 ,  169 ,  170 ,  171 ,  172 ,  173 ,  174 ,  175 ,  176 ,  177 ,  178 ,  179 ,  180 ,  181 ,  182 ,  183 ,  184 ,  185 ,  186 ,  187 ,  188 ,  189 ,  190 ,  191 ,  192 ,  193 ,  194 ,  195 ,  196 ,  197 ,  198 ,  199 ,  200 ,  201 ,  202 ,  203 ,  204 ,  205 ,  206 ,  206 ,  207 ,  208 ,  209 ,  210 ,  211 ,  212 ,  213 ,  214 ,  215 ,  216 ,  217 ,  218 ,  219 ,  220 ,  221 ,  222 ,  223 ,  224 ,  225 ,  226 ,  227 ,  228 ,  229 ,  230 ,  231 ,  232 ,  233 ,  234 ,  235 ,  236 ,  237 ,  238 ,  239 ,  240 ,  241 ,  242 ,  243 ,  244 ,  245 ,  246 ,  247 ,  248 ,  249 ,  250 ,  251 ,  252}, 
  {0 ,  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 ,  10 ,  11 ,  12 ,  13 ,  14 ,  15 ,  16 ,  17 ,  18 ,  19 ,  20 ,  21 ,  22 ,  23 ,  24 ,  25 ,  26 ,  27 ,  28 ,  29 ,  30 ,  31 ,  32 ,  33 ,  34 ,  35 ,  36 ,  37 ,  38 ,  39 ,  40 ,  41 ,  42 ,  43 ,  44 ,  45 ,  46 ,  47 ,  48 ,  49 ,  50 ,  51 ,  52 ,  53 ,  54 ,  55 ,  56 ,  57 ,  58 ,  59 ,  60 ,  61 ,  62 ,  63 ,  64 ,  65 ,  66 ,  67 ,  68 ,  69 ,  70 ,  71 ,  72 ,  73 ,  74 ,  75 ,  76 ,  77 ,  78 ,  79 ,  80 ,  81 ,  82 ,  83 ,  84 ,  85 ,  86 ,  87 ,  88 ,  89 ,  90 ,  91 ,  92 ,  93 ,  94 ,  95 ,  96 ,  97 ,  98 ,  99 ,  100 ,  101 ,  102 ,  103 ,  104 ,  105 ,  106 ,  107 ,  108 ,  109 ,  110 ,  111 ,  112 ,  113 ,  114 ,  115 ,  116 ,  117 ,  118 ,  119 ,  120 ,  121 ,  122 ,  123 ,  124 ,  125 ,  126 ,  127 ,  128 ,  129 ,  130 ,  131 ,  132 ,  133 ,  134 ,  135 ,  136 ,  137 ,  138 ,  139 ,  140 ,  141 ,  142 ,  143 ,  144 ,  145 ,  146 ,  147 ,  148 ,  149 ,  150 ,  151 ,  152 ,  153 ,  154 ,  155 ,  156 ,  157 ,  158 ,  159 ,  160 ,  161 ,  162 ,  163 ,  164 ,  165 ,  166 ,  167 ,  168 ,  169 ,  170 ,  171 ,  172 ,  173 ,  174 ,  175 ,  176 ,  177 ,  178 ,  179 ,  180 ,  181 ,  182 ,  183 ,  184 ,  185 ,  186 ,  187 ,  188 ,  189 ,  190 ,  191 ,  192 ,  193 ,  194 ,  195 ,  196 ,  197 ,  198 ,  199 ,  200 ,  201 ,  202 ,  203 ,  204 ,  205 ,  206 ,  207 ,  208 ,  209 ,  210 ,  211 ,  212 ,  213 ,  214 ,  215 ,  216 ,  217 ,  218 ,  219 ,  220 ,  221 ,  222 ,  223 ,  224 ,  225 ,  226 ,  227 ,  228 ,  229 ,  230 ,  231 ,  232 ,  233 ,  234 ,  235 ,  236 ,  237 ,  238 ,  239 ,  240 ,  241 ,  242 ,  243 ,  244 ,  245 ,  246 ,  247 ,  248 ,  249 ,  250 ,  251 ,  252 ,  253} };

void goertzel_process_sample(BYTE);
BYTE goertzel_is_ready(void);
void goertzel_process_magnitudes(BYTE*);

#endif
