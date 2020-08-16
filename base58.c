#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz);

int main(int argc, char** argv)
{
	char in[25] = {0x05,};
	char out[35];
	size_t outSize = sizeof(out);
	size_t inSize = sizeof(in);
	int i;
	bool result;

//	05 00000000 00000000 00000000 00000000 00000000 00000000h = 31h1vYVSYuKP6AhS86fbRdMw9XHiZAvAaj
//	05 00000000 00000000 00000000 00000000 00000000 00000001h = 31h1vYVSYuKP6AhS86fbRdMw9XHiZAvAak
//	05 00000000 00000000 00000000 00000000 00000000 00000002h = 31h1vYVSYuKP6AhS86fbRdMw9XHiZAvAam
//	...
//	05 ffffffff ffffffff ffffffff ffffffff ffffffff fffffffdh = 3R2cuenjG5nFubqX9Wzuukdin2YfGQVzu2
//	05 ffffffff ffffffff ffffffff ffffffff ffffffff fffffffeh = 3R2cuenjG5nFubqX9Wzuukdin2YfGQVzu3
//	05 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffffh = 3R2cuenjG5nFubqX9Wzuukdin2YfGQVzu4

	result = b58enc(out, &outSize, (void *)in, inSize);
	if (result)
		printf("%s", out);
	else
		printf("error(%zu)", outSize);
}


static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz)
{
	const uint8_t *bin = data;
	int carry;
	ssize_t i, j, high, zcount = 0;
	size_t size;
	
	while (zcount < binsz && !bin[zcount])
		++zcount;
	
	size = (binsz - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
	
	for (i = zcount, high = size - 1; i < binsz; ++i, high = j)
	{
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j)
		{
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
		}
	}
	
	for (j = 0; j < size && !buf[j]; ++j);
	
	if (*b58sz <= zcount + size - j)
	{
		*b58sz = zcount + size - j + 1;
		return false;
	}
	
	if (zcount)
		memset(b58, '1', zcount);
	for (i = zcount; j < size; ++i, ++j)
		b58[i] = b58digits_ordered[buf[j]];
	b58[i] = '\0';
	*b58sz = i + 1;
	
	return true;
}
