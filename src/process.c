#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

int mygetch ( void ) 
{
  int ch;
  struct termios oldt, newt;

  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}

int main(){
	printf("1st RUID: %d\n", getuid());
	printf("1st EUID: %d\n", geteuid());
	uid_t ruid = 1337, euid = 1337;
	int rtn = setreuid(1337, 1337);

    printf("Press any key to continue.\n");
    mygetch();
    printf("Return value: %d\n", rtn);
    fprintf(stderr, "Value of errno: %d\n", errno);
    fprintf(stderr, "Errno String: %s\n", strerror(errno));
	printf("2nd RUID: %d\n", getuid());
	printf("2nd EUID: %d\n", geteuid());
}