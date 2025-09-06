#include <stdio.h>
#include "set.h"

int main() {
	int* aaa = set_create();
	set_add(&aaa, 5);
	set_add(&aaa, 5);
	set_add(&aaa, 6);
	
	printf("%d %d\n", aaa[0], aaa[1]);

	return 0;
}
