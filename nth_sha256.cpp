#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <x86intrin.h>

#define ALWAYS_INLINE inline __attribute__((always_inline))

thread_local __m128i STATE0, STATE1;
thread_local __m128i MSG, TMP;
thread_local __m128i MSG0, MSG1, MSG2, MSG3;
thread_local __m128i ABEF_SAVE, CDGH_SAVE;

namespace {
    const __m128i INIT0 = _mm_set_epi32(0x6a09e667, 0xbb67ae85, 0x510e527f, 0x9b05688c);
    const __m128i INIT1 = _mm_set_epi32(0x3c6ef372, 0xa54ff53a, 0x1f83d9ab, 0x5be0cd19);

    const __m128i MASK = _mm_set_epi64x(0x0c0d0e0f08090a0bULL, 0x0405060700010203ULL);

    const __m128i ADD0 = _mm_set_epi64x(0xe9b5dba5b5c0fbcfULL, 0x71374491428a2f98ULL);
    const __m128i ADD1 = _mm_set_epi64x(0xab1c5ed5923f82a4ULL, 0x59f111f13956c25bULL);
    const __m128i ADD2 = _mm_set_epi64x(0x550c7dc3243185beULL, 0x12835b01d807aa98ULL);
    const __m128i ADD3 = _mm_set_epi64x(0xc19bf1749bdc06a7ULL, 0x80deb1fe72be5d74ULL);
    const __m128i ADD4 = _mm_set_epi64x(0x240ca1cc0fc19dc6ULL, 0xefbe4786e49b69c1ULL);
    const __m128i ADD5 = _mm_set_epi64x(0x76f988da5cb0a9dcULL, 0x4a7484aa2de92c6fULL);
    const __m128i ADD6 = _mm_set_epi64x(0xbf597fc7b00327c8ULL, 0xa831c66d983e5152ULL);
    const __m128i ADD7 = _mm_set_epi64x(0x1429296706ca6351ULL, 0xd5a79147c6e00bf3ULL);
    const __m128i ADD8 = _mm_set_epi64x(0x53380d134d2c6dfcULL, 0x2e1b213827b70a85ULL);
    const __m128i ADD9 = _mm_set_epi64x(0x92722c8581c2c92eULL, 0x766a0abb650a7354ULL);
    const __m128i ADD10 = _mm_set_epi64x(0xc76c51a3c24b8b70ULL, 0xa81a664ba2bfe8a1ULL);
    const __m128i ADD11 = _mm_set_epi64x(0x106aa070f40e3585ULL, 0xd6990624d192e819ULL);
    const __m128i ADD12 = _mm_set_epi64x(0x34b0bcb52748774cULL, 0x1e376c0819a4c116ULL);
    const __m128i ADD13 = _mm_set_epi64x(0x682e6ff35b9cca4fULL, 0x4ed8aa4a391c0cb3ULL);
    const __m128i ADD14 = _mm_set_epi64x(0x8cc7020884c87814ULL, 0x78a5636f748f82eeULL);
    const __m128i ADD15 = _mm_set_epi64x(0xc67178f2bef9a3f7ULL, 0xa4506ceb90befffaULL);

    const __m128i PAD0 = _mm_set_epi64x(0x0000000000000000ULL, 0x0000000080000000ULL);
    const __m128i PAD1 = _mm_set_epi64x(0x0000010000000000ULL, 0x0000000000000000ULL);

    const __m128i CONST0 = _mm_add_epi32(PAD0, ADD2);
    const __m128i CONST1 = _mm_shuffle_epi32(CONST0, 0x0e);
    const __m128i CONST2 = _mm_add_epi32(PAD1, ADD3);
    const __m128i CONST3 = _mm_alignr_epi8(PAD1, PAD0, 4);
    const __m128i CONST4 = _mm_shuffle_epi32(CONST2, 0x0e);
    const __m128i CONST5 = _mm_sha256msg1_epu32(PAD0, PAD1);

    ALWAYS_INLINE void first_hash(const uint8_t start[], uint64_t length) {
        /* Load initial values */
        STATE0 = INIT0;
        STATE1 = INIT1;

        while (length >= 64) {
            /* Save current state */
            ABEF_SAVE = STATE0;
            CDGH_SAVE = STATE1;

            /* Rounds 0-3 */
            MSG = _mm_loadu_si128((const __m128i*) (start+0));
            MSG0 = _mm_shuffle_epi8(MSG, MASK);
            MSG = _mm_add_epi32(MSG0, ADD0);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

            /* Rounds 4-7 */
            MSG1 = _mm_loadu_si128((const __m128i*) (start+16));
            MSG1 = _mm_shuffle_epi8(MSG1, MASK);
            MSG = _mm_add_epi32(MSG1, ADD1);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

            /* Rounds 8-11 */
            MSG2 = _mm_loadu_si128((const __m128i*) (start+32));
            MSG2 = _mm_shuffle_epi8(MSG2, MASK);
            MSG = _mm_add_epi32(MSG2, ADD2);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

            /* Rounds 12-15 */
            MSG3 = _mm_loadu_si128((const __m128i*) (start+48));
            MSG3 = _mm_shuffle_epi8(MSG3, MASK);
            MSG = _mm_add_epi32(MSG3, ADD3);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
            MSG0 = _mm_add_epi32(MSG0, TMP);
            MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

            /* Rounds 16-19 */
            MSG = _mm_add_epi32(MSG0, ADD4);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
            MSG1 = _mm_add_epi32(MSG1, TMP);
            MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

            /* Rounds 20-23 */
            MSG = _mm_add_epi32(MSG1, ADD5);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
            MSG2 = _mm_add_epi32(MSG2, TMP);
            MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

            /* Rounds 24-27 */
            MSG = _mm_add_epi32(MSG2, ADD6);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
            MSG3 = _mm_add_epi32(MSG3, TMP);
            MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

            /* Rounds 28-31 */
            MSG = _mm_add_epi32(MSG3, ADD7);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
            MSG0 = _mm_add_epi32(MSG0, TMP);
            MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

            /* Rounds 32-35 */
            MSG = _mm_add_epi32(MSG0, ADD8);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
            MSG1 = _mm_add_epi32(MSG1, TMP);
            MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

            /* Rounds 36-39 */
            MSG = _mm_add_epi32(MSG1, ADD9);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
            MSG2 = _mm_add_epi32(MSG2, TMP);
            MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

            /* Rounds 40-43 */
            MSG = _mm_add_epi32(MSG2, ADD10);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
            MSG3 = _mm_add_epi32(MSG3, TMP);
            MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

            /* Rounds 44-47 */
            MSG = _mm_add_epi32(MSG3, ADD11);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
            MSG0 = _mm_add_epi32(MSG0, TMP);
            MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

            /* Rounds 48-51 */
            MSG = _mm_add_epi32(MSG0, ADD12);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
            MSG1 = _mm_add_epi32(MSG1, TMP);
            MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
            MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

            /* Rounds 52-55 */
            MSG = _mm_add_epi32(MSG1, ADD13);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
            MSG2 = _mm_add_epi32(MSG2, TMP);
            MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

            /* Rounds 56-59 */
            MSG = _mm_add_epi32(MSG2, ADD14);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
            MSG3 = _mm_add_epi32(MSG3, TMP);
            MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

            /* Rounds 60-63 */
            MSG = _mm_add_epi32(MSG3, ADD15);
            STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
            MSG = _mm_shuffle_epi32(MSG, 0x0e);
            STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

            /* Combine state  */
            STATE0 = _mm_add_epi32(STATE0, ABEF_SAVE);
            STATE1 = _mm_add_epi32(STATE1, CDGH_SAVE);

            start += 64;
            length -= 64;
        }

        TMP = _mm_shuffle_epi32(STATE0, 0x1b);     /* FEBA */
        STATE1 = _mm_shuffle_epi32(STATE1, 0xb1);  /* DCHG */
        MSG0 = _mm_blend_epi16(TMP, STATE1, 0xf0); /* DCBA */
        MSG1 = _mm_alignr_epi8(STATE1, TMP, 8);    /* ABEF */
    }

    ALWAYS_INLINE void sha_step() {
        /* Rounds 0-3 */
        MSG = _mm_add_epi32(MSG0, ADD0);
        STATE1 = _mm_sha256rnds2_epu32(INIT1, INIT0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(INIT0, STATE1, MSG);

        /* Rounds 4-7 */
        MSG = _mm_add_epi32(MSG1, ADD1);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        /* Rounds 8-11 */
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, CONST0);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, CONST1);
        MSG1 = _mm_sha256msg1_epu32(MSG1, PAD0);

        /* Rounds 12-15 */
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, CONST2);
        MSG0 = _mm_add_epi32(MSG0, CONST3);
        MSG0 = _mm_sha256msg2_epu32(MSG0, PAD1);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, CONST4);

        /* Rounds 16-19 */
        MSG = _mm_add_epi32(MSG0, ADD4);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, PAD1, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(PAD1, MSG0);

        /* Rounds 20-23 */
        MSG = _mm_add_epi32(MSG1, ADD5);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(CONST5, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        /* Rounds 24-27 */
        MSG = _mm_add_epi32(MSG2, ADD6);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        /* Rounds 28-31 */
        MSG = _mm_add_epi32(MSG3, ADD7);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        /* Rounds 32-35 */
        MSG = _mm_add_epi32(MSG0, ADD8);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        /* Rounds 36-39 */
        MSG = _mm_add_epi32(MSG1, ADD9);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        /* Rounds 40-43 */
        MSG = _mm_add_epi32(MSG2, ADD10);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        /* Rounds 44-47 */
        MSG = _mm_add_epi32(MSG3, ADD11);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        /* Rounds 48-51 */
        MSG = _mm_add_epi32(MSG0, ADD12);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        /* Rounds 52-55 */
        MSG = _mm_add_epi32(MSG1, ADD13);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        /* Rounds 56-59 */
        MSG = _mm_add_epi32(MSG2, ADD14);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        /* Rounds 60-63 */
        MSG = _mm_add_epi32(MSG3, ADD15);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0e);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        /* Combine state  */
        STATE0 = _mm_add_epi32(STATE0, INIT0);
        STATE1 = _mm_add_epi32(STATE1, INIT1);

        TMP = _mm_shuffle_epi32(STATE0, 0x1b);     /* FEBA */
        STATE1 = _mm_shuffle_epi32(STATE1, 0xb1);  /* DCHG */
        MSG0 = _mm_blend_epi16(TMP, STATE1, 0xf0); /* DCBA */
        MSG1 = _mm_alignr_epi8(STATE1, TMP, 8);    /* ABEF */
    }
}

void nth_sha256(uint8_t digest[], const uint8_t text[], uint64_t length, uint64_t n) {
    // sha256 padding
    uint64_t bitlength = length * 8;
    uint64_t padding_length = (bitlength + 65 + 511) / 512 * 64;

    uint8_t *start = (uint8_t*)calloc(padding_length, sizeof(*start));
    memcpy(start, text, length);

    start[length] = 0x80;
    start[padding_length - 4] = (bitlength >> 24) & 0xff;
    start[padding_length - 3] = (bitlength >> 16) & 0xff;
    start[padding_length - 2] = (bitlength >> 8) & 0xff;
    start[padding_length - 1] = bitlength & 0xff;

    first_hash(start, padding_length);
    free(start);

    // unroll for loop
    for (uint64_t i = 0; i < (n - 1) / 16; ++i) {
        sha_step(); sha_step(); sha_step(); sha_step();
        sha_step(); sha_step(); sha_step(); sha_step();
        sha_step(); sha_step(); sha_step(); sha_step();
        sha_step(); sha_step(); sha_step(); sha_step();
    }
    for (uint64_t i = 0; i < (n - 1) % 16; ++i) {
        sha_step();
    }

    // Change byte order
    MSG0 = _mm_shuffle_epi8(MSG0, MASK);
    MSG1 = _mm_shuffle_epi8(MSG1, MASK);

    // Save state
    _mm_storeu_si128((__m128i*) &digest[0], MSG0);
    _mm_storeu_si128((__m128i*) &digest[16], MSG1);
}

#ifdef TEST_MAIN
#include <cstdio>
#include <sys/time.h>
int main() {
    struct timeval t1, t2;
    double elapsedTime;

    uint8_t digest[32];
    uint8_t text[1] = { 0 };

    gettimeofday(&t1, NULL);
    nth_sha256(digest, text, 0, 100000000);
    gettimeofday(&t2, NULL);

    for (uint8_t i = 0; i < sizeof(digest); ++i) {
        printf("%02x", digest[i]);
    }
    printf("\n");

    elapsedTime = t2.tv_sec - t1.tv_sec;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000000.0;
    printf("Time: %.3fs\n", elapsedTime);
}
#endif // TEST_MAIN
