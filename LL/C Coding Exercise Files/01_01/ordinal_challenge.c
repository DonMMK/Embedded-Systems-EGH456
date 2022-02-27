#include <stdio.h>

char *ordinal(int v)
{
	if(v % 10 == 1 && v % 100 != 11){
		return "st";
	}
	else if(v % 10 == 2 && v % 100 != 12){
		return "nd";
	}
	else if(v % 10 == 3 && v % 100 != 13){
		return "rd";
	}
	else{
		return "th";
	}

}

int main()
{
	int c;
	
	for( c=1; c<=20; c++ )
	{
		printf("%3d%s\t%3d%s\t%3d%s\t%3d%s\t%3d%s\n",
				c,    ordinal(c),
				c+20, ordinal(c),
				c+40, ordinal(c),
				c+60, ordinal(c),
				c+80, ordinal(c)
			  );
	}

	return(0);
}
