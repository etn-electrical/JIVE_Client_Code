unsigned util_match_key(const uint8_t* const* key_set, unsigned key_count
	,const uint8_t* datum, unsigned datum_len)
{
	/* Supremum is the least element GREATER than the datum. */
	unsigned supremum = key_count;
	/* Minimum is the greatest element LESS THAN OR EQUAL to the datum. */
	unsigned minimum = 0;

	const uint8_t* x = datum;
	const uint8_t* end = datum + datum_len;
	while(x != end){

		/* If we have a single key match and reached the end of the key,
		 * then declare a prefix match. */
		if((1 == supremum - minimum) && '\0' == key_set[minimum][x - datum])
			return minimum;

		/* Set alphabetic character to lower case if appropriate. */
		uint8_t test = *x;
		if(key_set[key_count][0] == 'l'){
			if(test >= 'A' && test <= 'Z')
				test |= 0x20;
		}

		/* Look for a key that matches the current character. */
		while(test != key_set[minimum][x - datum]){
			unsigned mid = (minimum + supremum) >> 1;

			if(test > key_set[mid][x - datum]){
				/* mid is too low, so the minimum MAY be the next element. */
				minimum = mid + 1;
			}

		}
			
			&& test != key_set[supremum][x - datum]){
			unsigned mid = (minimum + supremum) >> 1;

			/*  */
			if(test > key_set[mid][x - datum]){
				/* mid is too low, so the minimum MAY be the next element. */
				minimum = mid + 1;
			}
			else{
				/* mid is too great, so this is our new supremum. */
				supremum = mid;
			}

			/* If */
			if(minimum >= supremum)
				return key_count;
			mid = (minimum + supremum) >> 1;
		}

		/* Go onto next character. */
		++x;
	}

	/* At this point, we reached the end of the datum.
	 * However if there are more characters in the key then this is NOT a match. */
	return key_set[minimum][x - datum] ? key_count : minimum;
}

