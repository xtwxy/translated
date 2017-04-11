#include <stdio.h>
#include <stdlib.h>
#include <config.h>

int main(int argc, char* argv[]) {

	printf("package\t\t: '%s' \n"
			"package name\t: '%s' \n"
			"version\t\t: '%s' \n"
			"bug report\t: '%s' \n"
			"package version\t: '%s,' \n"
			"version string\t: '%s' \n"
			"tar name\t: '%s' \n"
			"url\t\t: '%s' \n",
			PACKAGE,
			PACKAGE_NAME,
			VERSION,
			PACKAGE_BUGREPORT,
			PACKAGE_VERSION,
			PACKAGE_STRING,
			PACKAGE_TARNAME,
			PACKAGE_URL);

    return EXIT_SUCCESS;
}
