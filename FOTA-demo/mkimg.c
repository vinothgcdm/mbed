#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>

#define MAGIC_BYTES 0x55cc55cc
#define FIRMWARE_ADDR 0x40000
#define HEADER_SIZE 512

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

uint32_t sum_buf(char buf[], uint32_t len)
{
	    uint32_t sum = 0;

		for (int i = 0; i < len; i++)
				sum += buf[i] & 0xFF;

		return sum;
}

int main(int argc, char *argv[])
{
		FILE *img;
		FILE *hdr;
		uint32_t header[128] = {0};
		uint32_t sum;
		char buf[512];
		int ret;
		int len;

		header[0] = MAGIC_BYTES;
		header[1] = 0;
		header[2] = FIRMWARE_ADDR;
		header[3] = fsize(argv[1]) + HEADER_SIZE;

		hdr = fopen(argv[2], "wb");
		assert(NULL != hdr);
		img = fopen(argv[1], "rb");
		assert(NULL != img);

		ret = fwrite(header, sizeof(header), 1, hdr);
		assert(ret >= 0);

		sum = 0;
		do {
				len = fread(buf, 1, sizeof(buf), img);
				sum += sum_buf(buf, len);
				ret = fwrite(buf, len, 1, hdr);
				assert(ret >= 0);
		} while (len > 0);
		
		ret = fseek(hdr, 0, SEEK_SET);
		assert(ret == 0);
		
		header[1] = sum;
		ret = fwrite(header, sizeof(header), 1, hdr);
		assert(ret >= 0);

		fclose(hdr);
		fclose(img);
		
		return 0;
}
