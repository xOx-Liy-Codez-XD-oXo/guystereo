int loadAlphabet(struct liymParseResults *parse, FILE *infile, float *alphabetVertpos[], int alphabetTricounts[]) {
	//printf("parse...\r");

	for(int j = 0; j < 34; j++) {

		*parse = openLiymFromFile(j, "data/mdl/meta/alphabet.liym", infile);
		if (parse->filetype == 0) {
			printf("Load failed\r");
			return -1;
		}
		alphabetVertpos[j] = (float *)malloc((parse->tricount * 9) * sizeof(float));
		if (alphabetVertpos[j] == NULL) {
			printf("Load failed\r");
			return -1;
		}

		for(int i = 0; i < (parse->tricount * 9); i++) {
			alphabetVertpos[j][i] = parse->vertpos[i];
		}
		alphabetTricounts[j] = parse->tricount;
		

		freeLiym(*parse);
	}

	return 0;
}

