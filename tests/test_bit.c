#include <stdio.h>
#include <string.h>
#include <time.h>

#include "merc.h"

bool str_cmp(const char *astr, const char *bstr)
{
    return strcmp(astr, bstr) != 0;
}

struct sector_vector {
    const char *name;
    int value;
};

int main(void)
{
    const struct sector_vector vectors[] = {
        {"inside", SECT_INSIDE},
        {"city", SECT_CITY},
        {"field", SECT_FIELD},
        {"forest", SECT_FOREST},
        {"hills", SECT_HILLS},
        {"mountain", SECT_MOUNTAIN},
        {"swim", SECT_WATER_SWIM},
        {"noswim", SECT_WATER_NOSWIM},
        {"underwater", SECT_UNDERWATER},
        {"air", SECT_AIR},
        {"desert", SECT_DESERT}
    };
    unsigned int index;

    for (index = 0; index < sizeof(vectors) / sizeof(vectors[0]); index++) {
        int actual = sector_number((char *)vectors[index].name);
        if (actual != vectors[index].value) {
            fprintf(stderr, "%s mapped to %d, expected %d\n",
                    vectors[index].name, actual, vectors[index].value);
            return 1;
        }
        if (strcmp(sector_name(vectors[index].value), vectors[index].name) != 0) {
            fprintf(stderr, "%d did not map back to %s\n",
                    vectors[index].value, vectors[index].name);
            return 1;
        }
    }

    if (sector_number("invalid") != SECT_MAX ||
            strcmp(sector_name(SECT_MAX), "unknown") != 0) {
        fprintf(stderr, "invalid sector fallback failed\n");
        return 1;
    }

    printf("PASS: %zu sector mapping vectors and invalid fallback\n",
           sizeof(vectors) / sizeof(vectors[0]));
    return 0;
}
