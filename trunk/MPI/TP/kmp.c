algorithm kmp_search:
    input:
		an array of characters, S (the text to be searched)
		an array of characters, W (the word sought)
    output:
				an integer (the zero-based position in S at which W is found)

    define variables:
				an integer, i ← 0 (the position of the current character in W)
				an integer, j ← 0 (the beginning of the current match in S)				
						an array of integers, T (the table, computed elsewhere)

    while m + i is less than the length of S, do:
			if W[i] = S[m + i],
				let i ← i + 1
				if i equals the length of W,
					return m
			otherwise,
				let m ← m + i - T[i],
				if i is greater than 0,
					let i ← T[i]
	
		(if we reach here, we have searched all of S unsuccessfully)
		return the length of S
		 
 int m = 0;
 int i = 0;
 int T[];
 
 while (j + i < S.length) {
	 
	 if (W[i] == S[j+i]) {
		 i++;
		 
		 if (i == W.length)
			 return j
					 
	 } else {
			 j += i - T[i] + 1;
			 if (i > 0)
				 i += T[i];
			 
	 }
	 
 }