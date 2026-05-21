#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp_in, *fp_out;
    unsigned char buf[4];
    int i, pairs;

    fp_in = fopen("frame.raw", "rb");
    if (fp_in == NULL) {
        perror("fopen frame.raw");
        return -1;
    }

    fp_out = fopen("y_plane.raw", "wb");
    if (fp_out == NULL) {
        perror("fopen y_plane.raw");
        fclose(fp_in);
        return -1;
    }

    pairs = (640 * 480) / 2;

    for (i = 0; i < pairs; i++) {
        if (fread(buf, 1, 4, fp_in) != 4) {
            fprintf(stderr, "read error at pair %d\n", i);
            break;
        }
        fwrite(&buf[0], 1, 1, fp_out);
        fwrite(&buf[2], 1, 1, fp_out);
    }

    fclose(fp_in);
    fclose(fp_out);

    printf("Done: extracted Y plane to y_plane.raw\n");
    printf("Expected size: %d bytes\n", 640 * 480);

    return 0;
}
