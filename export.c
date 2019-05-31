/* code used to export the current sudoku
 */
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>

int Exporthtml (int *field00) {
	if (access("export",00) == -1) {
		mkdir("export", 0777);
		printf("Directory \"export/\" created\n");
	}
	if (chdir("export/") == -1) {
		printf("Changing to directory \"export\" failed\n-> Abort!\n");
		return -1;
	}

	if (access("html",00) == -1) {
		mkdir("html", 0777);
		printf("Directory \"export/html\" created\n");
	}
	if (chdir("html/") == -1) {
		printf("Changing to directory \"export/html\" failed\n-> Abort\n");
		return -1;
	}


/* create and write style.css if it does not exist */
	if (access("style.css", 06) == -1) {
		FILE *fp = NULL;

		fp = fopen("style.css", "w");
		if (fp == NULL) {
			printf("Creating \"style.css\" failed\n");
			return -1;
		} else {
			printf("Write \"style.css\"...\n");

			fprintf(fp, "body {\n   margin: 0mm;\n   padding: 0mm;\n}\n\n");
			fprintf(fp, "table, td {\n   border-color: black;\n   border-style: solid;\n   border-collapse: collapse;\n}\n\n");
			fprintf(fp, "table.mainframe {\n   width: 81mm;\n   height: 81mm;\n   float: left;\n   padding: 0mm;\n   margin: 0mm 9mm 5mm 0mm;\n   border-width: 1mm;\n}\n\n");
			fprintf(fp, "table.mainframe tr, td {\n   margin: 0mm;\n   padding: 0mm;\n   border-width: 0.75mm;\n}\n\n");
			fprintf(fp, "table.block {\n   width: 27mm;\n   height: 27mm;\n   margin: 0mm;\n   padding: 0mm;\n   border-width: 0mm;\n}\n\n");
			fprintf(fp, "table.block td {\n   width: 9mm;\n   height: 9mm;\n   text-align: center;\n   border-width: 0.5mm;\n}\n\n");
		}
		fclose(fp);
	}
/* Create index.html, if it does not exist */
	FILE *fp;
	if (access("index.html", 06) == -1) {
		fp = fopen("index.html", "w");
		if (fp == NULL) {
			printf("Creating \"index.html\" failed\n");
			return -1;
		} else {
			printf("Create \"index.html\"...\n");

			fprintf(fp, "<html>\n<head>\n   <title>Sudoku Export</title>\n   <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" media=\"all\"/>\n</head>\n\n<body>\n");
		}

	} else {
		fp = fopen("index.html", "a");
	}

	if (fp == NULL) {
		printf("Can not access \"index.html\"\n");
		return -1;
	} else {
		printf("Append Sudoku to \"index.html\"...\n");

		fprintf(fp, "  <table class=\"mainframe\">\n");

		for (int ym = 0; ym < 3; ym++) {
			fprintf(fp, "    <tr>\n");
			for (int xm = 0; xm < 3; xm++) {
				fprintf(fp, "      <td>\n        <table class=\"block\">\n");
				for (int yb = 0; yb < 3; yb++) {
					fprintf(fp, "          <tr>\n");
					for (int xb = 0; xb < 3; xb++) {
						// leave empty cells blank
						if (*(field00+ym*3+yb+(xm*3+xb)*9) == 0) {
							fprintf(fp, "            <td></td>\n");
						} else {
							fprintf(fp, "            <td>%i</td>\n", *(field00+ym*3+yb+(xm*3+xb)*9));
						}
					}
					fprintf(fp, "          </tr>\n");
				}

				fprintf(fp, "        </table>\n      </td>\n");
			}
			fprintf(fp, "    </tr>\n");
		}
		fprintf(fp, "  </table>\n");
	}
	fclose(fp);


	chdir("../..");

	printf("Appending Sudoku to \"index.html\" successfull\n");
	return 0;
}
