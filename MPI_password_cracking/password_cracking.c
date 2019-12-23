#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <time.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

/***********************************************************************
*******
  Demonstrates how to crack an encrypted password using a simple
  "brute force" algorithm. Works on passwords that consist only of 2
uppercase
  letters and a 2 digit integer. Your personalised data set is included
in the
  code.

  Compile with:
    mpicc -o password_cracking password_cracking.c -lcrypt

  If you want to analyse the results then use the redirection operator
to send
  output to a file that you can view using an editor or the less
utility:

     mpirun -n 3 ./password_cracking >password_cracking.txt

  Dr Kevan Buckley, University of Wolverhampton, 2018
************************************************************************
******/
int n_passwords = 4;
pthread_t t1, t2;
char *encrypted_passwords[] = {

"$6$KB$3MiAO5oLs/.coZCPQ2QYOy8Ozo3v7QzGdwBEv3N7E0pJen3CJ63DmYXIZz6KEsykHmGsu3Dh1KCNe0niN0wvx/",
"$6$KB$jyDvGJlpBoZ7V0LmBQMe8IRWBBOs5iptBLdOhT4LNJClRiXwfx4ul/IlCXEgzYOUjIhmBUJKNfHPVmJP3dueR1",
"$6$KB$iyAdOw/ziDVBE0sXz8H3YRvGMVpqgV0DTg0dVbtPUyheOGYQGWP0C0g4hXnGTMZtUT0NXtmeaMY1Q6ykJqcTw0",
"$6$KB$Uz4cD9uzcYjtg9/zNnA4wdLtqlTWw42taHPdqzfJYQOmv2Ct79UJ8e11XtqdxzH3E58trHonpZFDOwYRwJPGs1"
};

/**
 Required by lack of standard function in C.   
*/

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}


void function_1(char *salt_and_encrypted){
  int x, y, z;     // Loop counters
  char salt[7];    // String used in hashing the password. Need space

  char plain[7];   // The combination of letters currently being checked
  char *enc;       // Pointer to the encrypted password
  int count = 0;   // The number of combinations explored so far

  substr(salt, salt_and_encrypted, 0, 6);

  for(x='A'; x<='M'; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
       printf("Instance 1");
        sprintf(plain, "%c%c%02d", x, y, z); 
        enc = (char *) crypt(plain, salt);
        count++;
        if(strcmp(salt_and_encrypted, enc) == 0){
          printf("#%-8d%s %s\n", count, plain, enc);
        } else {
          printf(" %-8d%s %s\n", count, plain, enc);
        }
      }
    }
  }
  printf("%d solutions explored\n", count);
}

void function_2(char *salt_and_encrypted){
  int x, y, z;     // Loop counters
  char salt[7];    // String used in hashing the password. Need space

  char plain[7];   // The combination of letters currently being checked
  char *enc;       // Pointer to the encrypted password
  int count = 0;   // The number of combinations explored so far

  substr(salt, salt_and_encrypted, 0, 6);

  for(x='N'; x<='Z'; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
        printf("Instance 2");
        sprintf(plain, "%c%c%02d", x, y, z); 
        enc = (char *) crypt(plain, salt);
        count++;
        if(strcmp(salt_and_encrypted, enc) == 0){
          printf("#%-8d%s %s\n", count, plain, enc);
        } else {
          printf(" %-8d%s %s\n", count, plain, enc);
        }
      }
    }
  }
  printf("%d solutions explored\n", count);
}

int time_difference(struct timespec *start, struct timespec *finish,
                    long long int *difference) {
  long long int ds =  finish->tv_sec - start->tv_sec; 
  long long int dn =  finish->tv_nsec - start->tv_nsec; 

  if(dn < 0 ) {
    ds--;
    dn += 1000000000; 
  } 
  *difference = ds * 1000000000 + dn;
  return !(*difference > 0);
}


int main(int argc, char *argv[]){

  struct timespec start, finish;
  long long int difference;   
  int account = 0;
  clock_gettime(CLOCK_MONOTONIC, &start);

 

int size, rank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(size != 3) {
    if(rank == 0) {
      printf("This program needs to run on exactly 3 processes\n");
    }
  } else
   {
    if(rank ==0){
      int x;
      int y;
      int i;
      MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);  
      MPI_Send(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    } 
    else 
    {
      if(rank == 1){
      int i;
      int number = rank + 10;
      MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      for ( i = 0; i<n_passwords;i<i++)
      {
		    function_1(encrypted_passwords[i]);
      }
    }
    else if(rank == 2)
    {
      int i;
      int number = rank + 10;
      MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      for ( i = 0; i<n_passwords;i<i++)
      {
		    function_2(encrypted_passwords[i]);
      }
	   }
    }
  }
  MPI_Finalize(); 

  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &difference);

  printf("Elapsed Time: %9.5lfs\n", difference/1000000000.0);
 

  return 0;
}
